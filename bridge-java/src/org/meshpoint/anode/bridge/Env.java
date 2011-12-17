package org.meshpoint.anode.bridge;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.InterfaceManager;
import org.meshpoint.anode.idl.Types;
import org.meshpoint.anode.js.JSObject;
import org.meshpoint.anode.module.IModule;
import org.meshpoint.anode.util.Log;
import org.meshpoint.anode.util.PrintStreamLog;

/**
 * Class encapsulating the Java state corresponding to
 * a node::Isolate.
 * This class contains all entrypoints that are entered
 * from JNI by the node event thread.
 * @author paddy
 *
 */
public class Env {

	/*********************
	 * private state
	 *********************/

	private static ThreadLocal<Env> currentEnv  = new ThreadLocal<Env>();
	private Thread eventThread;
	long envHandle;
	private InterfaceManager interfaceManager;
	private List<SynchronousOperation> pendingOps;
	private HashMap<String, ModuleContext> modules;
	private boolean allowPending = true;
	private boolean isDisposed;
	private boolean entryRequested;
	private String TAG = "Env";
	
	static {
		try {
			logger = (Log)Class.forName("org.meshpoint.anode.util.AndroidLog").newInstance();
		} catch(InstantiationException e) {
			/* unexpected case ... */
		} catch (IllegalAccessException e) {
			/* unexpected case ... */
		} catch (ClassNotFoundException e) {
			/* probably not on Android then ... */
			logger = new PrintStreamLog(System.err);
		}
	}
	
	/********************
	 * public API
	 ********************/

	public FinalizeQueue finalizeQueue;
	public static Log logger;
	
	/**
	 * Create an Env. Called by the bridge addon
	 * @param nodeIsolate
	 * @param v8Isolate
	 * @return
	 */
	static synchronized Env create(long nodeIsolate) {
		Env result = new Env(nodeIsolate);
		currentEnv.set(result);
		return result;
	}
	
	/**
	 * Get the env associated with this event thread
	 * @return
	 */
	public static Env getCurrent() {
		return currentEnv.get();
	}
	
	public long getHandle() {return envHandle;}

	/**
	 * Release this env. Called by the bridge addon
	 */
	void release() {
		dispose();
	}
	
	public Object loadModule(String moduleClassname, ModuleContext moduleContext) {
		try {
			IModule moduleInst = (IModule)Class.forName(moduleClassname).newInstance();
			moduleContext.setModule(moduleInst);
			Object val = moduleInst.startModule(moduleContext);
			if(val != null)
				modules.put(moduleClassname, moduleContext);
			return val;
		} catch (ClassCastException e) {
			/* the given class was not an implementation of IModule */
			logger.e(TAG + ":loadModule()", "Requested module class (" + moduleClassname + ") is not an instance of IModule");
		} catch (ClassNotFoundException e) {
			logger.e(TAG + ":loadModule()", "Requested module class (" + moduleClassname + ") could not be found");
		} catch (InstantiationException e) {
			logger.e(TAG + ":loadModule()", "Requested module class (" + moduleClassname + ") does not have a no-argument constructor");
		} catch (IllegalAccessException e) {
			logger.e(TAG + ":loadModule()", "Requested module class (" + moduleClassname + ") does not have a public no-argument constructor");
		} catch (Throwable e) {
			logger.e(TAG + ":loadModule()", "Unexpected exception initialising module (" + moduleClassname + ")", e);
		}
		return null;
	}

	public boolean unloadModule(String moduleClassname) {
		boolean result = false;
		ModuleContext ctx = modules.get(moduleClassname);
		if(ctx != null) {
			IModule module = ctx.getModule();
			try {
				module.stopModule();
			} catch(Throwable t) {}
			modules.remove(moduleClassname);
			result = true;
			if(modules.isEmpty()) dispose();
		}
		return result;
	}

	public InterfaceManager getInterfaceManager() {
		return interfaceManager;
	}
	
	public int findClass(Class<?> javaClass) {
		return Types.fromJavaType(interfaceManager, javaClass);
	}

	public boolean isEventThread() {return (Thread.currentThread() == eventThread);}

	/********************
	 * Sync ops queue
	 ********************/

	private void requestEntry() {
		BridgeNative.requestEntry(envHandle);
	}
	
	public void waitForOperation(SynchronousOperation op) {
		if(isEventThread()) {
			synchronized(op) {
				if(op.isPending()) op.run();
				return;
			}
		}
		synchronized(pendingOps) {
			if(!allowPending) {
				op.cancel();
				return;
			}
			if(!pendingOps.contains(op))
				pendingOps.add(op);
			if(!entryRequested) {
				requestEntry();
				entryRequested = true;
			}
		}
		synchronized(op) {
			while(op.isPending())
				try {op.wait();} catch(InterruptedException e) {}
		}
		synchronized(pendingOps) {
			pendingOps.remove(op);
		}
	}
	
	void onEntry() {
		synchronized(pendingOps) {
			entryRequested = false;
			for(SynchronousOperation op : pendingOps) {
				synchronized(op) {
					if(op.isPending()) {
						op.run();
						op.notifyAll();
					}
				}
			}
		}
	}
	
	private void cancelScheduledOps() {
		synchronized(pendingOps) {
			for(SynchronousOperation op : pendingOps) {
				synchronized(op) {
					op.cancel();
					op.notifyAll();
				}
			}
			allowPending = false;
		}
	}

	/********************
	 * instance management
	 ********************/
	Class<?> getJSObjectClass() {
		return JSObject.class;
	}
	
	Class<?> getStubClass(int classId, int mode) {
		Class<?> result = null;
		IDLInterface iface = interfaceManager.getById(classId);
		if(iface != null) {
			try {
				result = interfaceManager.getStubClass(iface, mode);
			} catch(ClassNotFoundException e) {
				logger.e(TAG, "Exception attempting to locate stub class", e);
			}
		}
		return result;
	}
	
	/********************
	 * private
	 ********************/
	private Env(long envHandle) {
		this.envHandle  = envHandle;
		this.eventThread = Thread.currentThread();
		interfaceManager = new InterfaceManager(this, null);
		pendingOps = new ArrayList<SynchronousOperation>();
		pendingOps.add(finalizeQueue = new FinalizeQueue(this));
		modules = new HashMap<String, ModuleContext>();
	}
	
	public void finalize() {dispose();}

	private void dispose() {
		/* synchronize to ensure only one thread
		 * performs the underlying disposal */
		boolean iWillDispose;
		synchronized(this) {
			iWillDispose = !isDisposed;
			isDisposed = true;
		}
		if(iWillDispose) {
			cancelScheduledOps();
			for(ModuleContext ctx : modules.values())
				ctx.getModule().stopModule();
			interfaceManager.dispose();
		}
	}

}
