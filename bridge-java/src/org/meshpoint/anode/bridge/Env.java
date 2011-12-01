package org.meshpoint.anode.bridge;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.meshpoint.anode.idl.InterfaceManager;
import org.meshpoint.anode.js.JSInterface;
import org.meshpoint.anode.module.IModule;
import org.meshpoint.anode.type.IValue;
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
	private long threadid;
	private long nodeIsolate;
	private long v8Isolate;
	private InterfaceManager interfaceManager;
	private List<SynchronousOperation> pendingOps;
	private HashMap<String, ModuleContext> modules;
	private boolean isDisposed;
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
	public WrapQueue wrapQueue;
	public static Log logger;
	
	/**
	 * Create an Env. Called by the bridge addon
	 * @param nodeIsolate
	 * @param v8Isolate
	 * @return
	 */
	static synchronized Env create(long nodeIsolate, long v8Isolate) {
		Env result = new Env(nodeIsolate, v8Isolate);
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

	/**
	 * Release this env. Called by the bridge addon
	 */
	void release() {
		dispose();
	}
	
	public IValue loadModule(String moduleClassname, ModuleContext moduleContext) {
		try {
			IModule moduleInst = (IModule)Class.forName(moduleClassname).newInstance();
			moduleContext.setModule(moduleInst);
			IValue val = moduleInst.startModule(moduleContext);
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

	public InterfaceManager getInterfaceManager() {
		return interfaceManager;
	}
	
	void idle() {
		synchronized(pendingOps) {
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
	
	void invoke(JSInterface obj, int methodIdx, IValue[] args) {
		idle();
	}

	/********************
	 * private
	 ********************/
	private Env(long nodeIsolate, long v8Isolate) {
		this.nodeIsolate = nodeIsolate;
		this.v8Isolate = v8Isolate;
		this.threadid = Thread.currentThread().getId();
		interfaceManager = new InterfaceManager(this);
		finalizeQueue = new FinalizeQueue(this);
		wrapQueue = new WrapQueue(this);
		pendingOps = new ArrayList<SynchronousOperation>();
		pendingOps.add(finalizeQueue);
		pendingOps.add(wrapQueue);
		modules = new HashMap<String, ModuleContext>();
	}
	
	private void requestEntry() {
		BridgeNative.requestEntry(nodeIsolate);
	}
	
	public void waitForOperation(SynchronousOperation op) {
		if(Thread.currentThread().getId() == threadid) {
			if(op.isPending()) op.run();
			return;
		}
		synchronized(pendingOps) {
			if(!pendingOps.contains(op))
				pendingOps.add(op);
		}
		synchronized(op) {
			requestEntry();
			try {op.wait();} catch(InterruptedException e) {}
		}
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
			for(ModuleContext ctx : modules.values())
				ctx.getModule().stopModule();
			interfaceManager.dispose();
		}
	}

}
