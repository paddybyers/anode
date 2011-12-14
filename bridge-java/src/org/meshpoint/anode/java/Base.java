package org.meshpoint.anode.java;

import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.type.IInterface;

public class Base implements IInterface {

	/*********************
	 * private state
	 *********************/
	long instHandle; /* (long)Persistent<Object>* */
	protected Env env = Env.getCurrent();
	private int classId;

	/*********************
	 * private API
	 *********************/
	protected Base(IDLInterface iface) {
		classId = iface.getId();
	}

	public void finalize() {
		env.finalizeQueue.put(instHandle, classId);
	}
	
	/*********************
	 * public API
	 *********************/

	@Override
	public IDLInterface getDeclaredType() {
		return env.getInterfaceManager().getById(classId);
	}

}
