package org.meshpoint.anode.module;

import org.meshpoint.anode.type.IValue;

public interface IModule {
	public IValue startModule(IModuleContext ctx);
	public void stopModule();
}
