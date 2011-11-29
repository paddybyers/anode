package org.meshpoint.anode.bridge;

import org.meshpoint.anode.type.IValue;

public interface ModuleContext {
	public IValue getExports();
	public void SetExports(IValue export);
	public long getEventThreadId();
}
