package org.meshpoint.anode;

import org.meshpoint.anode.Runtime.IllegalStateException;
import org.meshpoint.anode.Runtime.InitialisationException;
import org.meshpoint.anode.Runtime.NodeException;
import org.meshpoint.anode.Runtime.StateListener;

import android.app.IntentService;
import android.content.Intent;
import android.util.Log;

public class AnodeService extends IntentService implements StateListener {

	private static String TAG = "anode::AnodeService";
	public static final String ACTION_START = "org.meshpoint.anode.START";
	public static final String ACTION_STOP = "org.meshpoint.anode.STOP";
	public static final String CMD = "cmdline";
	
	private Runtime runtime;

	public AnodeService() {
		super(":anode.AnodeService");
		initRuntime();
	}

    private void initRuntime() {
    	try {
    		runtime = Runtime.getRuntime(this);
    		if(runtime != null)
    			runtime.addStateListener(this);
		} catch (InitialisationException e) {
			Log.v(TAG, "AnodeService.initRuntime: exception: " + e + "; cause: " + e.getCause());
		}
    }
    
	@Override
	protected void onHandleIntent(Intent intent) {
		String action = intent.getAction();

		/* unconditionally do a stop action */
		if(ACTION_STOP.equals(action)) {
			try {
				runtime.stop();
			} catch (IllegalStateException e) {
				Log.v(TAG, "AnodeReceiver.onReceive::stop: exception: " + e + "; cause: " + e.getCause());
				return;
			} catch (NodeException e) {
				Log.v(TAG, "AnodeReceiver.onReceive::stop: exception: " + e + "; cause: " + e.getCause());
				return;
			}
			return;
		}
		
		/* get the launch commandline */
		String args = intent.getStringExtra(CMD);
		
		/* if no cmdline was sent, then launch the activity for interactive behaviour */
		if(args == null || args.isEmpty()) {
			intent.setClassName(getApplication(), AnodeActivity.class.getName());
			getApplication().startActivity(intent);
			return;
		}

		/* otherwise, process args */
		ArgProcessor argProcessor = new ArgProcessor(intent.getExtras(), args );
		String[] processedArgs = argProcessor.process();

		/* launch directly */
		try {
			runtime.start(processedArgs);
		} catch (IllegalStateException e) {
			Log.v(TAG, "AnodeReceiver.onReceive::start: exception: " + e + "; cause: " + e.getCause());
		} catch (NodeException e) {
			Log.v(TAG, "AnodeReceiver.onReceive::start: exception: " + e + "; cause: " + e.getCause());
		}
	}

	@Override
	public void stateChanged(final int state) {
		/* exit the service if the runtime has exited */
		if(state == Runtime.STATE_STOPPED) {
			java.lang.Runtime.getRuntime().exit(0);
		}
	}

}
