package org.meshpoint.node.bridge;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.meshpoint.anode.js.JSInterface;
import org.meshpoint.node.idl.InterfaceManager;
import org.meshpoint.node.type.IValue;

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
	private static HashMap <Thread, Env> envsByThread = new HashMap <Thread, Env>();
	private long threadid;
	private long nodeIsolate;
	private long v8Isolate;
	private InterfaceManager interfaceManager;
	private List<SynchronousOperation> pendingOps;
	
	/********************
	 * public API
	 ********************/

	public FinalizeQueue finalizeQueue;
	public WrapQueue wrapQueue;
	
	static synchronized Env create(long nodeIsolate, long v8Isolate) {
		Env result = new Env(nodeIsolate, v8Isolate);
		envsByThread.put(Thread.currentThread(), result);
		return result;
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
	}
	
	private void requestEntry() {
		NativeBinding.requestEntry(nodeIsolate);
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
	
	private void dispose() {
		interfaceManager.dispose();
	}
}
