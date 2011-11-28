package org.meshpoint.anode.js;

import java.util.Date;

import org.meshpoint.node.idl.Types;
import org.meshpoint.node.idl.Types.JSType;
import org.meshpoint.node.type.IBoolean;
import org.meshpoint.node.type.IDate;
import org.meshpoint.node.type.INumber;
import org.meshpoint.node.type.IString;
import org.meshpoint.node.type.IValue;

/**
 * A variant to hold an arbitrary value, plus a pool of free
 * variant instances. The pool is grown on demand.
 * 
 * The re-pooled objects are freed if the number available
 * exceeds a hard-coded count.
 */
final class JSValue implements IValue, IBoolean, INumber, IString, IDate {
	
	/************************
	 * private state
	 ************************/

	int type;
	long longValue;
	double dblValue;
	Object obValue;

	/************************
	 * public API 
	 ************************/

	/**
	 * JSString
	 */
	public static IString asJSString(String s) {
		JSValue result = get();
		result.obValue = s;
		result.type = Types.TYPE_STRING;
		return result;
	}

	@Override
	public String getStringValue() {
		if(type == Types.TYPE_STRING)
			return (String)obValue;
		throw new Types.TypeError();
	}

	/**
	 * JSNumber
	 */
	public static INumber asJSNumber(long l) {
		JSValue result = get();
		result.longValue = l;
		result.type = Types.TYPE_LONG;
		return result;
	}

	public static INumber asJSNumber(double d) {
		JSValue result = get();
		result.dblValue = d;
		result.type = Types.TYPE_DOUBLE;
		return result;
	}

	public static INumber asJSNumber(Number n) {
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

	@Override
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

	@Override
	public double getDoubleValue() {
		if(type == Types.TYPE_DOUBLE)
			return dblValue;
		throw new Types.TypeError();
	}

	@Override
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
	public static IBoolean asJSBoolean(boolean b) {
		JSValue result = get();
		result.longValue = b ? 1 : 0;
		result.type = Types.TYPE_BOOL;
		return result;
	}

	public static IBoolean asJSBoolean(Boolean b) {
		JSValue result = get();
		result.longValue = b.booleanValue() ? 1 : 0;
		result.type = Types.TYPE_BOOL;
		return result;
	}

	@Override
	public boolean getBooleanValue() {
		if(type == Types.TYPE_BOOL)
			return longValue != 0;
		throw new Types.TypeError();
	}


	/**
	 * JSDate
	 */
	public static IDate asJSDate(long l) {
		JSValue result = get();
		result.longValue = l;
		result.type = Types.TYPE_DATE;
		return result;
	}

	public static IDate asJSDate(Date d) {
		JSValue result = get();
		result.longValue = d.getTime();
		result.type = Types.TYPE_DATE;
		return result;
	}

	@Override
	public Date getDateValue() {
		return new Date(longValue);
	}

	@Override
	public JSType getType() {
		// TODO Auto-generated method stub
		return Types.toJSType(type);
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
	
	@Override
	public void release() {
		put(this);
	}
}
