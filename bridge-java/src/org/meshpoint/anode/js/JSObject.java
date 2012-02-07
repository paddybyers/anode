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

import java.util.Collection;

import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.type.ICollection;
import org.meshpoint.anode.type.IIndexedCollection;

/**
 * An object that is a wrapper of a native JS object
 * @author paddy
 *
 */
public class JSObject implements ICollection, IIndexedCollection<Object> {

	/*********************
	 * private state
	 *********************/

	long instHandle; /* (long)Persistent<Object>* */
	Env env;

	/*********************
	 * private API
	 *********************/

	JSObject(long instHandle) {
		this.instHandle = instHandle;
		env = Env.getCurrent();
	}

	public void finalize() {
		env.finalizeQueue.put(instHandle, -1);
	}

	/*********************
	 * public API
	 *********************/

	@Override
	public Object getIndexedProperty(int idx) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void setIndexedProperty(int idx, Object value) {
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
	public Object getProperty(String key) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void setProperty(String key, Object value) {
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

}
