package org.meshpoint.anode.java;

import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.idl.Types;

public class Base {

	/*********************
	 * private state
	 *********************/
	long instHandle; /* (long)Persistent<Object>* */
	protected Env env = Env.getCurrent();
	protected int type;

	/*********************
	 * private API
	 *********************/
	protected Base(short classId) {
		env.bindInterface(classId);
		type = Types.classid2Type(classId);
	}

	protected Base(int type) {
		this.type = type;
	}

	public void finalize() {
		env.finalizeQueue.put(instHandle, type);
	}
	
}
