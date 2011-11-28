package org.meshpoint.node.bridge;


public class FinalizeQueue implements SynchronousOperation {
	
	/********************
	 * private state
	 ********************/
	private Env env;
	private static final int QUEUE_LENGTH = 256;
	private long[] buffer = new long[QUEUE_LENGTH];
	private int count;
	
	/********************
	 * public API
	 *******************/
	
	public FinalizeQueue(Env env) {
		this.env = env;
	}
	
	/**
	 * Appends a new item to the queue; may be called
	 * from any thread
	 * @param h the handle
	 */
	public synchronized void put(long h) {
		if(count == QUEUE_LENGTH)
			env.waitForOperation(this);
		if(count == QUEUE_LENGTH)
			throw new RuntimeException("Fatal error processing FinalizeQueue");
		buffer[count++] = h;
	}

	/**
	 * Removes and releases all items currently queued
	 */
	@Override
	public synchronized void run() {
		for(int i = 0; i < count; i++) {
			NativeBinding.releaseObjectHandle(buffer[i]);
		}
		count = 0;
	}

	@Override
	public synchronized boolean isPending() {
		return count > 0;
	}
}
