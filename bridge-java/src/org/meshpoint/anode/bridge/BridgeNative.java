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

package org.meshpoint.anode.bridge;

import java.util.Collection;

import org.meshpoint.anode.idl.IDLInterface;

public class BridgeNative {
	
	private static final String LIBRARY_PATH = System.getenv("NODE_ROOT");
	private static final String BRIDGE_LIBRARY = "bridge.node";

	static {
		try {
			System.load(LIBRARY_PATH + '/' + BRIDGE_LIBRARY);
		} catch(UnsatisfiedLinkError e) {
			System.err.println("Unable to load bridge native library: " + LIBRARY_PATH);
		}
	}

	/* IFunction */
	native static Object callAsFunction(long envHandle, long instHandle, Object target, Object[] args);
	native static Object callAsConstructor(long envHandle, long instHandle, Object[] args);

	/* ICollection */
	native static Object getProperty(long envHandle, long instHandle, String key);
	native static void setProperty(long envHandle, long instHandle, String key, Object value);
	native static void deleteProperty(long envHandle, long instHandle, String key);
	native static boolean containsProperty(long envHandle, long instHandle, String key);
	native static Collection<String> properties(long envHandle, long instHandle);

	/* IIndexedCollection */
	public native static Object getIndexedProperty(long envHandle, long instHandle, int elementType, int idx);
	public native static void setIndexedProperty(long envHandle, long instHandle, int elementType, int idx, Object value);
	public native static void deleteIndexedProperty(long envHandle, long instHandle, int idx);
	public native static boolean containsIndex(long envHandle, long instHandle, int idx);
	public native static int getLength(long envHandle, long instHandle);
	public native static void setLength(long envHandle, long instHandle, int length);

	/* JSInterface */
	public native static Object invokeJSInterface(long envHandle, long instHandle, int classId, int opIdx, Object[] args);
	public native static Object getJSInterface(long envHandle, long instHandle, int classId, int attrIdx);
	public native static void setJSInterface(long envHandle, long instHandle, int classId, int attrIdx, Object val);
	
	/* instance handle management */
	native static void releaseObjectHandle(long envHandle, long instHandle, int type);
	
	/* interface handle management */
	public native static long bindInterface(long envHandle, long parentHandle, IDLInterface iface, int classId, int attrCount, int opCount, Class<?> declaredClass);
	public native static void bindAttribute(long envHandle, long ifaceHandle, int attrIdx, int type, String name);
	public native static void bindOperation(long envHandle, long ifaceHandle, int opIdx, int type, String name, int argCount, int[] argTypes);
	public native static void releaseInterface(long envHandle, long ifaceHandle);
	public native static void bindUserStub(long envHandle, long interfaceHandle, Class<?> userStub);
	public native static void bindPlatformStub(long envHandle, long interfaceHandle, Class<?> platformStub);
	public native static void bindDictStub(long envHandle, long interfaceHandle, Class<?> dictStub);

	/* event thread management */
	public native static void requestEntry(long envHandle);
	
	/* context initialisation:
	 * used in the Android implementation to pass an Android context
	 * object which is then passed to modules implemented in the bridge */
	public native static void setContext(Object opaqueContextObject);
}
