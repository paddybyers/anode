package org.meshpoint.node.idl;

import org.meshpoint.node.bridge.Env;
import org.meshpoint.node.bridge.NativeBinding;

/**
 * An interface defined in IDL for use within one or more modules
 * @author paddy
 *
 */
public class IDLInterface {
	/********************
	 * private state
	 ********************/

	private long inboundBinding;
	private long outboundBinding;
	private Env env;
	private int id = -1;
	private String name;
	private Class<? extends Object> javaClass;

	/********************
	 * interface metadata
	 ********************/

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
	
	public IDLInterface(Env env, Class<? extends Object> javaClass) {
		this.env = env;
		String ifaceName = javaClass.getCanonicalName();
		if(!ifaceName.startsWith(Types.INTERFACE_TYPE_PREFIX))
			throw new IllegalArgumentException("InterfaceImpl(): interface " + ifaceName + " is not in expected package");
		this.name = ifaceName.substring(Types.INTERFACE_TYPE_PREFIX.length());
		this.javaClass = javaClass;
		id = env.getInterfaceManager().put(this);
	}

	public int getId() {
		return id;
	}

	public Class<? extends Object> getJavaClass() {
		return javaClass;
	}

	public String getName() {
		return name;
	}

	public Env getEnv() {
		return env;
	}

	public Attribute[] getAttributes() {
		return attributes;
	}

	public Operation[] getOperations() {
		return operations;
	}

	/*********************
	 * private
	 *********************/
	
	public void initOutbound() {
		outboundBinding = NativeBinding.bindOutboundInterface(this);
		for(Attribute attr : attributes)
			attr.outboundBinding = NativeBinding.bindOutboundAttribute(attr, this);
		for(Operation op : operations)
			op.outboundBinding = NativeBinding.bindOutboundOperation(op, this);
	}

	public void initForImport() {
		inboundBinding = NativeBinding.bindInboundInterface(this);
		for(Attribute attr : attributes)
			attr.inboundBinding = NativeBinding.bindInboundAttribute(attr, this);
		for(Operation op : operations)
			op.inboundBinding = NativeBinding.bindInboundOperation(op, this);
	}
	
	public void dispose() {
		if(outboundBinding != 0) {
			for(Attribute attr : attributes)
				NativeBinding.releaseOutboundAttribute(attr.outboundBinding);
			for(Operation op : operations)
				NativeBinding.releaseOutboundAttribute(op.outboundBinding);
			NativeBinding.releaseOutboundInterface(outboundBinding);
		}
		if(inboundBinding != 0) {
			for(Attribute attr : attributes)
				NativeBinding.releaseInboundAttribute(attr.inboundBinding);
			for(Operation op : operations)
				NativeBinding.releaseInboundAttribute(op.inboundBinding);
			NativeBinding.releaseInboundInterface(inboundBinding);
		}
		
	}
}
