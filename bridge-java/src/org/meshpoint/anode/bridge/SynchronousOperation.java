package org.meshpoint.anode.bridge;

public interface SynchronousOperation extends Runnable {
	public boolean isPending();
}
