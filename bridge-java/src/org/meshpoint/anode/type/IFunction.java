package org.meshpoint.anode.type;

public interface IFunction extends IValue {
	public IValue callAsFunction(IValue[] args);
	public IValue callAsConstructor(IValue[] args);
}
