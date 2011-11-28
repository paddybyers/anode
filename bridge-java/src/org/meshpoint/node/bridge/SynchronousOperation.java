package org.meshpoint.node.bridge;

public interface SynchronousOperation extends Runnable {
	public boolean isPending();
}
