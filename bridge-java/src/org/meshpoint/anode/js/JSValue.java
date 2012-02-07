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

import org.meshpoint.anode.idl.Types;

/**
 * A variant to hold an arbitrary value, plus a pool of free
 * variant instances. The pool is grown on demand.
 * 
 * The re-pooled objects are freed if the number available
 * exceeds a hard-coded count.
 */
public final class JSValue {
	
	/************************
	 * private state
	 ************************/

	int type;
	public long longValue;
	public double dblValue;

	/************************
	 * public API 
	 ************************/

	/**
	 * JSNumber
	 */
	public static JSValue asJSNumber(long l) {
		JSValue result = get();
		result.longValue = l;
		result.type = Types.TYPE_LONG;
		return result;
	}

	public static JSValue asJSNumber(double d) {
		JSValue result = get();
		result.dblValue = d;
		result.type = Types.TYPE_DOUBLE;
		return result;
	}

	public static JSValue asJSNumber(Number n) {
		JSValue result = get();
		Class<? extends Number> nClass = n.getClass();
		if(nClass == Float.class || nClass == Double.class) {
			result.dblValue = n.doubleValue();
		} else {
			result.longValue = n.longValue();
			if(nClass == Short.class)
				result.type = Types.TYPE_SHORT;
			else if(nClass == Integer.class)
				result.type = Types.TYPE_INT;
			else
				result.type = Types.TYPE_LONG;
		}
		return result;
	}

	public long getLongValue() {
		switch(type) {
		case Types.TYPE_BYTE:
		case Types.TYPE_SHORT:
		case Types.TYPE_INT:
		case Types.TYPE_LONG:
			return longValue;
		}
		throw new Types.TypeError();
	}

	public double getDoubleValue() {
		if(type == Types.TYPE_DOUBLE)
			return dblValue;
		throw new Types.TypeError();
	}

	public Number getNumberValue() {
		Number result;
		switch(type) {
		case Types.TYPE_BYTE:
			result = new Byte((byte)longValue);
			break;
		case Types.TYPE_SHORT:
			result = new Short((short)longValue);
			break;
		case Types.TYPE_INT:
			result = new Integer((int)longValue);
			break;
		case Types.TYPE_LONG:
			result = new Long(longValue);
			break;
		case Types.TYPE_DOUBLE:
			result = new Double(dblValue);
			break;
		default:
			throw new Types.TypeError();
		}
		return result;
	}

	/**
	 * JSBoolean
	 */
	public static JSValue asJSBoolean(boolean b) {
		JSValue result = get();
		result.longValue = b ? 1 : 0;
		result.type = Types.TYPE_BOOL;
		return result;
	}

	public static JSValue asJSBoolean(Boolean b) {
		JSValue result = get();
		result.longValue = b.booleanValue() ? 1 : 0;
		result.type = Types.TYPE_BOOL;
		return result;
	}

	public boolean getBooleanValue() {
		if(type == Types.TYPE_BOOL)
			return longValue != 0;
		throw new Types.TypeError();
	}

	/**
	 * The maximum number of cached instances
	 */
	final static int MAX_POOL_COUNT = 256;
	
	/**
	 * The current count of cached instances
	 */
	private static int count;
	
	/**
	 * The head of the pool
	 */
	private static JSValue pool;
	
	/**
	 * Get a Variant instance from the pool, allocating one if necessary
	 * @return Variant instance
	 */
	static synchronized JSValue get() {
		if(pool == null) {
			pool = new JSValue();
			++count;
		}
		JSValue result = pool;
		pool = result.next;
		--count;
		return result;
	}
	
	/**
	 * Return a Variant to the pool
	 * @param val the Variant to be returned
	 */
	synchronized void put(JSValue val) {
		if(count < MAX_POOL_COUNT) {
			val.next = pool;
			pool = val;
			++count;
		}
	}

	/**
	 * Pointer for the pool list
	 */
	private JSValue next;
}
