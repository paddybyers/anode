package org.meshpoint.node.bridge;

import org.meshpoint.anode.java.Base;

public class WrapQueue implements SynchronousOperation {

	/********************
	 * private state
	 ********************/
	private Env env;
	private static final int QUEUE_LENGTH = 256;
	private Base[] buffer = new Base[QUEUE_LENGTH];
	private int count;
	
	/********************
	 * public API
	 *******************/
	
	public WrapQueue(Env env) {
		this.env = env;
	}
	
	/**
	 * Appends a new item to the queue; may be called
	 * from any thread
	 * @param h the handle
	 */
	public synchronized void put(Base obj) {
		if(count == QUEUE_LENGTH)
			env.waitForOperation(this);
		if(count == QUEUE_LENGTH)
			throw new RuntimeException("Fatal error processing WrapQueue");
		buffer[count++] = obj;
	}

	/**
	 * Removes and releases all items currently queued
	 */
	@Override
	public synchronized void run() {
		for(int i = 0; i < count; i++) {
			Base obj = buffer[i];
			NativeBinding.wrapJavaInterface(obj, obj.getDeclaredType());
		}
		count = 0;
	}

	@Override
	public synchronized boolean isPending() {
		return count > 0;
	}
}
