package org.meshpoint.anode.module;

import org.meshpoint.anode.type.IIndexedCollection;
import org.meshpoint.anode.type.IValue;

public interface IModuleContext {
	public IModule getModule();
	public IIndexedCollection<IValue> getModuleExports();
	public long getEventThreadId();
}
