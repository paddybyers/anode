package org.meshpoint.anode.idl;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.bridge.Env;

/**
 * An interface defined in IDL for use within one or more modules
 * @author paddy
 *
 */
public class IDLInterface {
	/********************
	 * private state
	 ********************/

	private static final int MAX_NAME_LENGTH = 80;
	private InterfaceManager mgr;
	private long ifaceHandle;
	private int id = -1;
	private String name;
	private Class<?> javaClass;
	private boolean isCallback;
	private boolean isValueType;

	/********************
	 * interface metadata
	 ********************/

	int modifiers;
	IDLInterface parent;
	Attribute[] attributes;
	Operation[] operations;

	/**
	 * A class encapsulating the metadata for an attribute
	 */
	public class Attribute {
		public String name;
		public int type;
		public int modifiers;
	}

	/**
	 * A class encapsulating the metadata for an operation (method)
	 */
	public class Operation {
		public String name;
		public int type;
		public int modifiers;
		public int[] args;
	}

	/********************
	 * public API
	 ********************/
	
	public IDLInterface(InterfaceManager mgr, Class<?> javaClass) {
		this.mgr = mgr;
		name = javaClass.getCanonicalName();
		this.javaClass = javaClass;
		if(Callback.class.isAssignableFrom(javaClass))
			isCallback = true;
		if(Dictionary.class.isAssignableFrom(javaClass))
			isValueType = true;
		id = mgr.put(this);
	}

	public int getId() {
		return id;
	}

	public long getHandle() {
		return ifaceHandle;
	}

	public Class<?> getJavaClass() {
		return javaClass;
	}

	public String getName() {
		return name;
	}
	
	public int getModifiers() {
		return modifiers;
	}

	public Attribute[] getAttributes() {
		return attributes;
	}

	public Operation[] getOperations() {
		return operations;
	}

	public String getStubClassname() {
		String candidate = StubUtil.uclName(name.replace('.', '_'));
		if(candidate.length() > MAX_NAME_LENGTH) {
			/* hash the remainder of the name */
			/* FIXME: implement this */
		}
		return candidate;
	}
	
	public boolean isCallback() { return isCallback; }
	public boolean isValueType() { return isValueType; }

	/*********************
	 * private
	 *********************/
	
	void initNative() {
		long envHandle = mgr.getEnv().getHandle();
		ifaceHandle = BridgeNative.bindInterface(envHandle, this, id, attributes.length, operations.length, javaClass);
		try {
			Class<?> userStub = mgr.getStubClass(this, StubUtil.MODE_USER);
			BridgeNative.bindUserStub(envHandle, ifaceHandle, userStub);
		} catch(ClassNotFoundException e) {}
		try {
			Class <?> platformStub = mgr.getStubClass(this, StubUtil.MODE_PLATFORM);
			BridgeNative.bindPlatformStub(envHandle, ifaceHandle, platformStub);
		} catch(ClassNotFoundException e) {}
		try {
			Class<?> dictStub = mgr.getStubClass(this, StubUtil.MODE_DICT);
			BridgeNative.bindDictStub(envHandle, ifaceHandle, dictStub);
		} catch(ClassNotFoundException e) {}
		for(int i = 0; i < attributes.length; i++) {
			Attribute attr = attributes[i];
			BridgeNative.bindAttribute(envHandle, ifaceHandle, i, attr.type, attr.name);
		}
		for(int i = 0; i < operations.length; i++) {
			Operation op = operations[i];
			BridgeNative.bindOperation(envHandle, ifaceHandle, i, op.type, op.name, op.args.length, op.args);
		}
	}

	void dispose() {
		Env env = mgr.getEnv();
		if(env != null)
			BridgeNative.releaseInterface(env.getHandle(), ifaceHandle);		
	}
}
