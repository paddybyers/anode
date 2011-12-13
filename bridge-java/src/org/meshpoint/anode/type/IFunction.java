package org.meshpoint.anode.type;

public interface IFunction {
	public Object callAsFunction(Object target, Object[] args);
	public Object callAsConstructor(Object[] args);
}
