package org.meshpoint.anode.js;

import org.meshpoint.anode.idl.Types.JSType;
import org.meshpoint.anode.type.IIndexedCollection;

public abstract class JSArray<T> implements IIndexedCollection<T> {
	public abstract JSType getComponentType();
}
