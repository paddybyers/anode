package org.meshpoint.node.bridge;

public interface Module {
	public void startModule(ModuleContext ctx);
	public void stopModule();
}
