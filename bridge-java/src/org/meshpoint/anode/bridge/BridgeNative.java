package org.meshpoint.anode.bridge;

import java.util.Collection;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.IDLInterface.Operation;
import org.meshpoint.anode.java.Base;
import org.meshpoint.anode.js.JSInterface;
import org.meshpoint.anode.js.JSObject;
import org.meshpoint.anode.type.IValue;

public class BridgeNative {

	/* intrinsically typed */
	native static IValue callAsFunction(long instHandle, IValue[] args);
	native static IValue callAsConstructor(long instHandle, IValue[] args);

	native static IValue callMethod(long instHandle, String methodName, IValue[] args);

	native static IValue getProperty(long instHandle, String key);
	native static void setProperty(long instHandle, String key, IValue value);
	native static void deleteProperty(long instHandle, String key);
	native static boolean containsProperty(long instHandle, String key);
	native static Collection<String> properties(long instHandle);

	native static IValue getIndexedProperty(long instHandle, int idx);
	native static void setIndexedProperty(long instHandle, int idx, IValue value);
	native static void deleteIndexedProperty(long instHandle, int idx);
	native static boolean containsIndex(long instHandle, int idx);
	native static int length(long instHandle);

	/* declaratively typed */
	native static IValue callAsInterface(long instHandle, int classIdx, int methodIdx, IValue[] args);
	
	/* instance handle management */
	native static void releaseObjectHandle(long instHandle);
	public native static void wrapJSObject(long instHandle, JSObject obj);
	public native static void wrapJSInterface(long instHandle, JSInterface obj, IDLInterface iface);
	public native static long wrapJavaInterface(Base obj, IDLInterface iface);
	
	/* interface handle management */
	public native static long bindInboundInterface(IDLInterface iface);
	public native static long bindOutboundInterface(IDLInterface iface);
	public native static long bindInboundAttribute(Attribute attr, IDLInterface iface);
	public native static long bindOutboundAttribute(Attribute attr, IDLInterface iface);
	public native static long bindInboundOperation(Operation attr, IDLInterface iface);
	public native static long bindOutboundOperation(Operation attr, IDLInterface iface);
	public native static long releaseOutboundInterface(long externalBinding);
	public native static long releaseInboundInterface(long nativeBinding);
	public native static long releaseOutboundAttribute(long externalBinding);
	public native static long releaseInboundAttribute(long nativeBinding);
	public native static long releaseOutboundOperation(long externalBinding);
	public native static long releaseInboundOperation(long nativeBinding);

	/* event thread management */
	public native static void requestEntry(long nodeIsolate);
	
	/* context initialisation:
	 * used in the Android implementation to pass an Android context
	 * object which is then passed to modules implemented in the bridge */
	public native static void setContext(Object opaqueContextObject);
}
