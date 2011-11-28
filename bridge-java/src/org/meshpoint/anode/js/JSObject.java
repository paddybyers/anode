package org.meshpoint.anode.js;

import java.util.Collection;

import org.meshpoint.node.bridge.Env;
import org.meshpoint.node.bridge.NativeBinding;
import org.meshpoint.node.idl.Types.JSType;
import org.meshpoint.node.type.ICollection;
import org.meshpoint.node.type.IFunction;
import org.meshpoint.node.type.IIndexedCollection;
import org.meshpoint.node.type.IValue;

/**
 * An object that is a wrapper of a native JS object
 * @author paddy
 *
 */
public class JSObject implements IFunction, ICollection, IIndexedCollection<IValue> {

	/*********************
	 * private state
	 *********************/

	long instHandle; /* (long)Persistent<Object>* */
	Env env;

	/*********************
	 * private API
	 *********************/

	JSObject(long instHandle, Env env) {
		this.instHandle = instHandle;
		this.env = env;
		NativeBinding.wrapJSObject(instHandle, this);
	}

	public void finalize() {
		env.finalizeQueue.put(instHandle);
	}

	/*********************
	 * public API
	 *********************/

	@Override
	public JSType getType() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void release() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public IValue getIndexedProperty(int idx) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void setIndexedProperty(int idx, IValue value) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void deleteIndexedProperty(int idx) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean containsIndex(int idx) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public int length() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public IValue getProperty(String key) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void setProperty(String key, IValue value) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void deleteProperty(String key) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean containsProperty(String key) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public Collection<String> properties() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public IValue callAsFunction(IValue[] args) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public IValue callAsConstructor(IValue[] args) {
		// TODO Auto-generated method stub
		return null;
	}

}
