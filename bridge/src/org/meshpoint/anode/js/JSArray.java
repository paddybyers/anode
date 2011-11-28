package org.meshpoint.anode.js;

import org.meshpoint.node.idl.Types.JSType;
import org.meshpoint.node.type.IIndexedCollection;

public abstract class JSArray<T> implements IIndexedCollection<T> {
	public abstract JSType getComponentType();
}
