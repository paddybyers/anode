package org.meshpoint.anode.js;

import org.meshpoint.anode.type.IFunction;

public class JSFunction extends JSObject implements IFunction {

	JSFunction(long instHandle) {
		super(instHandle);
	}

	@Override
	public Object callAsFunction(Object target, Object[] args) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Object callAsConstructor(Object[] args) {
		// TODO Auto-generated method stub
		return null;
	}

}
