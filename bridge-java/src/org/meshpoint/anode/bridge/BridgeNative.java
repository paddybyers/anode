package org.meshpoint.anode.bridge;

import java.util.Collection;

import org.meshpoint.anode.idl.IDLInterface;

public class BridgeNative {

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
	public native static Object invokeJSInterface(long envHandle, long instHandle, long interfaceHandle, int opIdx, Object[] args);
	public native static Object getJSInterface(long envHandle, long instHandle, long interfaceHandle, int attrIdx);
	public native static void setJSInterface(long envHandle, long instHandle, long interfaceHandle, int attrIdx, Object val);
	
	/* instance handle management */
	native static void releaseObjectHandle(long envHandle, long instHandle, int type);
	
	/* interface handle management */
	public native static long bindInterface(long envHandle, IDLInterface iface, int classId, int attrCount, int opCount, Class<?> declaredClass, Class<?> userStub, Class<?> platformStub, Class<?> dictStub);
	public native static void bindAttribute(long envHandle, long ifaceHandle, int attrIdx, int type, String name);
	public native static void bindOperation(long envHandle, long ifaceHandle, int opIdx, int type, String name, int argCount, int[] argTypes);
	public native static void releaseInterface(long envHandle, long ifaceHandle);

	/* event thread management */
	public native static void requestEntry(long envHandle);
	
	/* context initialisation:
	 * used in the Android implementation to pass an Android context
	 * object which is then passed to modules implemented in the bridge */
	public native static void setContext(Object opaqueContextObject);
}
