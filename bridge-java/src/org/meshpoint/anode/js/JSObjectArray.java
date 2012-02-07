/*
 * Copyright 2011-2012 Paddy Byers
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.idl.Types;
import org.w3c.dom.ObjectArray;

public class JSObjectArray<T> extends JSArray implements ObjectArray<T> {
	
	private int componentType;
	JSObjectArray(long instHandle, int componentType) {
		super(instHandle);
		this.componentType = componentType;
	}

	public void finalize() { super.dispose(Types.TYPE_OBJECT|Types.TYPE_ARRAY); }

	@SuppressWarnings("unchecked")
	@Override
	public T getElement(int index) {
		if(env.isEventThread())
			return (T)BridgeNative.getIndexedProperty(env.getHandle(), instHandle, componentType, index);
		return (T)deferOp(OP.GET_ELEMENT, env, instHandle, componentType, index, null).ob;
	}

	@Override
	public void setElement(int index, T value) {
		if(env.isEventThread())
			BridgeNative.setIndexedProperty(env.getHandle(), instHandle, componentType, index, value);
		deferOp(OP.SET_ELEMENT, env, instHandle, componentType, index, value);
	}

}
