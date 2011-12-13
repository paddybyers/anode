package org.meshpoint.anode.module;

public interface IModule {
	public Object startModule(IModuleContext ctx);
	public void stopModule();
}
