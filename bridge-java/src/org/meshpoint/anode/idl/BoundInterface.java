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

package org.meshpoint.anode.idl;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.IDLInterface.Operation;

public class BoundInterface {
	private long ifaceHandle;
	private Env env;
	private IDLInterface iface;
	
	public BoundInterface(Env env, short classId) {
		this.env = env;
		iface = env.getInterfaceManager().getById(classId);
	}
	
	public Env getEnv() { return env; }
	
	public int getType() { return Types.classid2Type(iface.getId()); }
	
	public void bind() {
		/* create native binding for this interface */
		long envHandle = env.getHandle();
		IDLInterface parent = iface.getParent();
		long parentHandle = (parent == null) ? 0 : env.bindInterface(parent.getId()).ifaceHandle;
		ifaceHandle = BridgeNative.bindInterface(envHandle, parentHandle, iface, iface.getId(), iface.getAttributes().length, iface.getOperations().length, iface.getJavaClass());
		InterfaceManager mgr = env.getInterfaceManager();

		/* bind stubs */
		try {
			Class<?> userStub = mgr.getStubClass(iface, StubUtil.MODE_USER);
			BridgeNative.bindUserStub(envHandle, ifaceHandle, userStub);
		} catch(ClassNotFoundException e) {}
		try {
			Class <?> platformStub = mgr.getStubClass(iface, StubUtil.MODE_PLATFORM);
			BridgeNative.bindPlatformStub(envHandle, ifaceHandle, platformStub);
		} catch(ClassNotFoundException e) {}
		try {
			Class<?> dictStub = mgr.getStubClass(iface, StubUtil.MODE_DICT);
			BridgeNative.bindDictStub(envHandle, ifaceHandle, dictStub);
		} catch(ClassNotFoundException e) {}

		/* bind attributes, including recursively binding attribute types */
		Attribute[] attributes = iface.getAttributes();
		for(int i = 0; i < attributes.length; i++) {
			Attribute attr = attributes[i];
			IDLInterface attrType = Types.baseInterface(mgr, attr.type);
			if(attrType != null)
				env.bindInterface(attrType.getId());
			BridgeNative.bindAttribute(envHandle, ifaceHandle, i, attr.type, attr.name);
		}

		/* bind operations, including recursively binding operation and arg types */
		Operation[] operations = iface.getOperations();
		for(int i = 0; i < operations.length; i++) {
			Operation op = operations[i];
			IDLInterface opType = Types.baseInterface(mgr, op.type);
			if(opType != null)
				env.bindInterface(opType.getId());
			for(int j = 0; j < op.args.length; j++) {
				IDLInterface argType = Types.baseInterface(mgr, op.args[j]);
				if(argType != null)
					env.bindInterface(argType.getId());
			}
			BridgeNative.bindOperation(envHandle, ifaceHandle, i, op.type, op.name, op.args.length, op.args);
		}
	}

	public void dispose() {
		BridgeNative.releaseInterface(env.getHandle(), ifaceHandle);
	}
}
