package org.meshpoint.anode.bridge;

public interface Module {
	public void startModule(ModuleContext ctx);
	public void stopModule();
}
