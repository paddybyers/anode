package org.meshpoint.anode;

import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.Collection;
import java.util.HashMap;

import org.meshpoint.anode.Runtime.IllegalStateException;
import org.meshpoint.anode.Runtime.InitialisationException;
import org.meshpoint.anode.Runtime.NodeException;
import org.meshpoint.anode.Runtime.StateListener;
import org.meshpoint.anode.util.ArgProcessor;
import org.meshpoint.anode.util.ModuleUtils;
import org.meshpoint.anode.util.ModuleUtils.ModuleType;

import android.app.IntentService;
import android.content.Intent;
import android.util.Log;

public class AnodeService extends IntentService {

	private static String TAG = "anode::AnodeService";
	
	/**********************
	 * Instance table
	 ***********************/
	private static int counter;
	private static HashMap<String, Isolate> instances = new HashMap<String, Isolate>();
	
	static synchronized String addInstance(String instance, Isolate isolate) {
		if(instance == null) instance = String.valueOf(counter++);
		instances.put(instance, isolate);
		return instance;
	}
	
	static synchronized Isolate getInstance(String instance) {
		return instances.get(instance);
	}
	
	static synchronized void removeInstance(String instance) {
		instances.remove(instance);
	}
	
	static synchronized String soleInstance() {
		String instance = null;
		if(instances.size() == 1)
			instance = instances.keySet().iterator().next();
		return instance;
	}
	
	static synchronized Collection<Isolate> getAll() {
		return instances.values();
	}

	/**********************
	 * Service
	 **********************/
	public AnodeService() {
		super(":anode.AnodeService");
		/* android.os.Debug.waitForDebugger();*/
		(new File(Constants.APP_DIR)).mkdirs();
		(new File(Constants.MODULE_DIR)).mkdirs();
		(new File(Constants.RESOURCE_DIR)).mkdirs();
	}

    private void initRuntime(String[] opts) {
    	try {
    		Runtime.initRuntime(this, opts);
		} catch (InitialisationException e) {
			Log.v(TAG, "AnodeService.initRuntime: exception: " + e + "; cause: " + e.getCause());
		}
    }

	@Override
	protected void onHandleIntent(Intent intent) {
		/* we should not get a stop action; should have been intercepted by the receiver */
		String action = intent.getAction();
		if(AnodeReceiver.ACTION_STOP.equals(action)) {
			Log.v(TAG, "AnodeService.onHandleIntent::stop: internal error");
			return;
		}
		
		if(AnodeReceiver.ACTION_START.equals(action)) {
			/* get system options before handling this invocation */
			String options = intent.getStringExtra(AnodeReceiver.OPTS);
			String[] opts = options == null ? null : options.split("\\s");
			initRuntime(opts);
			handleStart(intent);
		} else if(AnodeReceiver.ACTION_INSTALL.equals(action)) {
			handleInstall(intent);
		} else if(AnodeReceiver.ACTION_UNINSTALL.equals(action)) {
			handleUninstall(intent);
		}
	}
	
	private void handleStart(Intent intent) {

		/* get the launch commandline */
		String args = intent.getStringExtra(AnodeReceiver.CMD);
		
		/* if no cmdline was sent, then launch the activity for interactive behaviour */
		if(args == null || args.isEmpty()) {
			intent.setClassName(getApplication(), AnodeActivity.class.getName());
			getApplication().startActivity(intent);
			return;
		}

		/* create a new instance based on the supplied args */
		ArgProcessor argProcessor = new ArgProcessor(intent.getExtras(), args);
		String[] processedArgs = argProcessor.processArray();

		/* launch directly */
		try {
			Isolate isolate = Runtime.createIsolate();
			String instance = intent.getStringExtra(AnodeReceiver.INST);
			isolate.addStateListener(new ServiceListener(addInstance(instance, isolate)));
			isolate.start(processedArgs);
		} catch (IllegalStateException e) {
			Log.v(TAG, "AnodeReceiver.onReceive::start: exception: " + e + "; cause: " + e.getCause());
		} catch (NodeException e) {
			Log.v(TAG, "AnodeReceiver.onReceive::start: exception: " + e + "; cause: " + e.getCause());
		}
	}

	class ServiceListener implements StateListener {
		String instance;
		
		private ServiceListener(String instance) {
			this.instance = instance;
		}

		@Override
		public void stateChanged(final int state) {
			/* exit remove the instance if exited */
			if(state == Runtime.STATE_STOPPED) {
				removeInstance(instance);
			}
		}
	}

	private void handleInstall(Intent intent) {

		/* get the specified arguments */
		String module = intent.getStringExtra(AnodeReceiver.MODULE);
		String path = intent.getStringExtra(AnodeReceiver.PATH);
		File moduleResource;
		boolean remove_tmp_resource = false;
		
		/* if no path was specified, it is an error */
		if(path == null || path.isEmpty()) {
			Log.v(TAG, "AnodeService.onHandleInstall: no path specified");
			return;
		}
		
		/* resolve expected module type from path */
		ModuleType modType = ModuleUtils.guessModuleType(path);
		if(modType == null) {
			Log.v(TAG, "AnodeService.onHandleInstall: unable to determine module type: path = " + path);
			return;
		}
		
		/* guess the module name, if not already specified */
		if(module == null || module.isEmpty()) {
			int pathEnd = path.lastIndexOf('/') + 1;
			module = path.substring(pathEnd, path.length()-modType.extension.length());
		}

		/* download module if http or https */
		if(path.startsWith("http://") || path.startsWith("https://")) {
			String filename = ModuleUtils.getResourceUriHash(path);
			try {
				moduleResource = ModuleUtils.getResource(new URI(path), filename);
				remove_tmp_resource = true;
			} catch(IOException e) {
				Log.v(TAG, "handleInstall: aborting (unable to download resource); exception: " + e + "; resource = " + path);
				return;
			} catch(URISyntaxException e) {
				Log.v(TAG, "handleInstall: aborting (invalid URI specified for resource); exception: " + e + "; resource = " + path);
				return;
			}
		} else {
			moduleResource = new File(path);
		}
		
		/* unpack if necessary */
		if(modType.unpacker != null) {
			try {
				moduleResource = ModuleUtils.unpack(moduleResource, module, modType);
				remove_tmp_resource = true;
			} catch(IOException e) {
				Log.v(TAG, "handleInstall: aborting (unable to unpack resource); exception: " + e + "; resource = " + path);
				return;
			}
		}

		/* copy processed package to modules dir */
		File installLocation = ModuleUtils.getModuleFile(module, modType);
		if(installLocation.exists()) {
			if(!ModuleUtils.deleteFile(installLocation)) {
				Log.v(TAG, "handleInstall: aborting (unable to delete old module version); resource = " + path + ", destination = " + installLocation.toString());
				return;
			}
		}
		if(ModuleUtils.copyFile(moduleResource, installLocation)) {
			if(remove_tmp_resource)
				ModuleUtils.deleteFile(moduleResource);
			Log.v(TAG, "handleInstall: success; resource = " + path + ", destination = " + installLocation.toString());
			return;
		}
		Log.v(TAG, "handleInstall: aborting (unable to copy resource); resource = " + path + ", destination = " + installLocation.toString());
	}

	private void handleUninstall(Intent intent) {
		String module = intent.getStringExtra(AnodeReceiver.MODULE);
		
		/* if no module was specified, it is an error */
		if(module == null || module.isEmpty()) {
			Log.v(TAG, "AnodeService.onHandleUninstall: no module specified");
			return;
		}

		File moduleLocation = ModuleUtils.locateModule(module, null);
		if(moduleLocation == null) {
			Log.v(TAG, "AnodeService.onHandleUninstall: specified module does not exist: " + module);
			return;
		}
		if(!ModuleUtils.deleteFile(moduleLocation)) {
			Log.v(TAG, "AnodeService.onHandleUninstall: unable to delete: " + module + "; attempting to delete " + moduleLocation.toString());
			return;
		}
		Log.v(TAG, "handleUninstall: success; module = " + module + ", location = " + moduleLocation.toString());
	}
}
