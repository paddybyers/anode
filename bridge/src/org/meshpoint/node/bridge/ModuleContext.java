package org.meshpoint.node.bridge;

import org.meshpoint.node.type.IValue;

public interface ModuleContext {
	public IValue getExports();
	public void SetExports(IValue export);
	public long getEventThreadId();
}
