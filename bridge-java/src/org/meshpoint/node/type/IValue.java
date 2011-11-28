package org.meshpoint.node.type;

import org.meshpoint.node.idl.Types.JSType;

public interface IValue {
	public JSType getType();
	void release();
}
