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

package org.meshpoint.anode.java;

import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.idl.Types;

public class Base {

	/*********************
	 * private state
	 *********************/
	long instHandle; /* (long)Persistent<Object>* */
	protected Env env;
	protected int type;

	/*********************
	 * private API
	 *********************/
	protected Base(short classId) {
		this(classId, Env.getCurrent());
	}

	protected Base(short classId, Env env) {
		this.env = env;
		env.bindInterface(classId);
		type = Types.classid2Type(classId);
	}

	protected Base(int type) {
		this.type = type;
	}

	public void finalize() {
		if(instHandle != 0)
			env.finalizeQueue.put(instHandle, type);
	}
	
	public Env getEnv() {
		return env;
	}
}
