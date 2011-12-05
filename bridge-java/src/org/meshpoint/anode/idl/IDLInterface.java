package org.meshpoint.anode.idl;

import org.meshpoint.anode.bridge.BridgeNative;

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
	private long inboundBinding;
	private long outboundBinding;
	private int id = -1;
	private String name;
	private Class<? extends Object> javaClass;

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
		private long inboundBinding;
		private long outboundBinding;
	}

	/**
	 * A class encapsulating the metadata for an operation (method)
	 */
	public class Operation {
		public String name;
		public int type;
		public int modifiers;
		public int[] args;
		private long inboundBinding;
		private long outboundBinding;
	}

	/********************
	 * public API
	 ********************/
	
	public IDLInterface(InterfaceManager mgr, Class<?> javaClass) {
		name = javaClass.getCanonicalName();
		this.javaClass = javaClass;
		id = mgr.put(this);
	}

	public int getId() {
		return id;
	}

	public long getInboundHandle() {
		if(inboundBinding == 0)
			initInbound();
		return inboundBinding;
	}

	public long getOutboundHandle() {
		if(outboundBinding == 0)
			initOutbound();
		return outboundBinding;
	}

	public Class<? extends Object> getJavaClass() {
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

	/*********************
	 * private
	 *********************/
	
	public void initOutbound() {
		outboundBinding = BridgeNative.bindOutboundInterface(this);
		for(Attribute attr : attributes)
			attr.outboundBinding = BridgeNative.bindOutboundAttribute(attr, this);
		for(Operation op : operations)
			op.outboundBinding = BridgeNative.bindOutboundOperation(op, this);
	}

	public void initInbound() {
		inboundBinding = BridgeNative.bindInboundInterface(this);
		for(Attribute attr : attributes)
			attr.inboundBinding = BridgeNative.bindInboundAttribute(attr, this);
		for(Operation op : operations)
			op.inboundBinding = BridgeNative.bindInboundOperation(op, this);
	}
	
	public void dispose() {
		if(outboundBinding != 0) {
			for(Attribute attr : attributes)
				BridgeNative.releaseOutboundAttribute(attr.outboundBinding);
			for(Operation op : operations)
				BridgeNative.releaseOutboundAttribute(op.outboundBinding);
			BridgeNative.releaseOutboundInterface(outboundBinding);
		}
		if(inboundBinding != 0) {
			for(Attribute attr : attributes)
				BridgeNative.releaseInboundAttribute(attr.inboundBinding);
			for(Operation op : operations)
				BridgeNative.releaseInboundAttribute(op.inboundBinding);
			BridgeNative.releaseInboundInterface(inboundBinding);
		}
		
	}
}
