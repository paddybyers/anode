/*
 * Copyright 2011-2012 Paddy Byers
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *	   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

package org.meshpoint.anode.bridge;

import java.lang.reflect.Constructor;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.meshpoint.anode.idl.BoundInterface;
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
	private Object envContext;
	private ClassLoader moduleClassLoader;
	private InterfaceManager interfaceManager;
	private List<SynchronousOperation> pendingOps;
	private HashMap<String, ModuleContext> modules;
	private RandomAccessArray boundInterfaces;
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
	static synchronized Env create(long nodeIsolate, Object envContext) {
		Env result = new Env(nodeIsolate, envContext, InterfaceManager.getInstance());
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

	public static void setEnv(Env env) {
		currentEnv.set(env);
	}

	public static short getInterfaceId(Class<?> javaClass) {
		Env env = currentEnv.get();
		short classId = env.interfaceManager.getByClass(javaClass).getId();
		env.bindInterface(classId);
		return classId;
	}

	public long getHandle() {return envHandle;}

	/**
	 * Release this env. Called by the bridge addon
	 */
	void release() {
		dispose();
	}

	public ClassLoader getClassLoader() {
		return moduleClassLoader;
	}

	public Object loadModule(String moduleClassname, ModuleContext moduleContext) {
		try {
			IModule moduleInst = (IModule)Class.forName(moduleClassname, true, getClassLoader()).newInstance();
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

	Class<?> getStubClass(short classId, int mode) {
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

	public BoundInterface bindInterface(short classId) {
		int idx = InterfaceManager.classId2Idx(classId);
		BoundInterface result;
		if((result = boundInterfaces.get(idx)) == null) {
			result = new BoundInterface(this, classId);
			boundInterfaces.put(idx, result);
			result.bind();
		}
		return result;
	}

	/********************
	 * private
	 ********************/
	private Env(long envHandle, Object envContext, InterfaceManager interfaceManager) {
		this.envHandle = envHandle;
		this.envContext = envContext;
		try {
			Class<?> clazz = Class
					.forName("org.meshpoint.anode.bridge.ModuleClassLoader");
			Constructor<?> ctor = clazz.getConstructor(Object.class);
			this.moduleClassLoader = (ClassLoader) ctor.newInstance(envContext);
		} catch (Exception e) {
			logger.e(TAG, "Exception attempting to instance module classloader", e);
			/* This is ok, we can do without a dedicated module classloader */
		}
		this.interfaceManager = interfaceManager;
		this.eventThread = Thread.currentThread();
		pendingOps = new ArrayList<SynchronousOperation>();
		pendingOps.add(finalizeQueue = new FinalizeQueue(this));
		modules = new HashMap<String, ModuleContext>();
		boundInterfaces = new RandomAccessArray();
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
			for(ModuleContext ctx : modules.values()) {
				ctx.getModule().stopModule();
			}
			for(int i = 0; i < boundInterfaces.size(); i++) {
				BoundInterface b = boundInterfaces.get(i);
				if(b != null) b.dispose();
			}
		}
	}

	private class RandomAccessArray {
		private static final int increment = 16;
		private BoundInterface[] elements;
		private RandomAccessArray() { elements = new BoundInterface[increment]; }

		private int size() { return elements.length; }

		private BoundInterface get(int idx) {
			BoundInterface elt = null;
			if(idx < elements.length)
				elt = elements[idx];
			return elt;
		}

		private void put(int idx, BoundInterface elt) {
			if(idx >= elements.length) {
				synchronized(this) {
					if(idx >= elements.length) {
						BoundInterface[] newElements = new BoundInterface[(idx + increment) & -increment];
						System.arraycopy(elements, 0, newElements, 0, elements.length);
						elements = newElements;
					}
				}
			}
			elements[idx] = elt;
		}

	}
}
