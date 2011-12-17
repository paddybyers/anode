package org.meshpoint.anode.idl;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.IDLInterface.Operation;

public class InterfaceManager {

	private Env env;
	private ClassLoader classLoader;
	private ArrayList<IDLInterface> interfaces;
	private HashMap<String, IDLInterface> nameMap;
	private HashMap<Class<?>, IDLInterface> classMap;
	
	/******************
	 * public API
	 ******************/

	public InterfaceManager(Env env, ClassLoader classLoader) {
		this.env = env;
		if(classLoader == null)
			classLoader = this.getClass().getClassLoader();
		this.classLoader = classLoader;
		interfaces = new ArrayList<IDLInterface>();
		nameMap = new HashMap<String, IDLInterface>();
		classMap = new HashMap<Class<?>, IDLInterface>();
	}
	
	Env getEnv() {
		return env;
	}

	ClassLoader getClassLoader() {
		return classLoader;
	}

	public synchronized IDLInterface getById(int id) {
		return interfaces.get(classId2Idx(id));
	}
	
	public synchronized IDLInterface getByName(String name) {
		IDLInterface result = nameMap.get(name);
		if(result != null)
			return result;
		
		try {
			Class<?> javaClass = classLoader.loadClass(name);
			result = getByClass(javaClass);
		} catch (ClassNotFoundException e) {}
		return result;
	}
	
	public synchronized IDLInterface getByClass(Class<?> javaClass) {
		IDLInterface result = classMap.get(javaClass);
		if(result == null)
			result = loadClass(javaClass);
		return result;
	}

	public Class<?> getStubClass(IDLInterface iface, int mode) throws ClassNotFoundException {
		String stubClassName = StubUtil.getStubPackage(mode) + '.' + iface.getStubClassname();
		return getClassLoader().loadClass(stubClassName);
	}
	
	/****************
	 * class loading
	 ****************/
	public IDLInterface loadClass(Class<?> javaClass) {
		IDLInterface result;
		synchronized(this) {
			result = classMap.get(javaClass);
			if(result != null) return result;
		}
		result = defineClass(javaClass);
		return result;
	}

	private IDLInterface defineClass(Class<?> javaClass) {
		String canonicalName = javaClass.getCanonicalName().intern();
		/* terminate when at Object or some other framework class */
		if(canonicalName == "java.lang.Object" || canonicalName.startsWith("org.meshpoint.anode"))
			return null;

		/* add to manager, and resolve parent */
		IDLInterface result = new IDLInterface(this, javaClass);
		result.modifiers = javaClass.getModifiers();
		Class<?> parentClass = javaClass.getSuperclass();
		if(parentClass != null)
			result.parent = loadClass(parentClass);

		/* resolve fields */
		ArrayList<Attribute> attributeList = new ArrayList<Attribute>();
		for(Field f : javaClass.getDeclaredFields()) {
			int modifiers = f.getModifiers();
			if(!f.isSynthetic() && ((modifiers & Modifier.PUBLIC)) != 0) {
				int type = Types.fromJavaType(this, f.getGenericType());
				if(type != Types.TYPE_INVALID) {
					Attribute attr = result.new Attribute();
					attr.name = f.getName();
					attr.type = type;
					attr.modifiers = modifiers;
					attributeList.add(attr);
				}
			}
		}
		result.attributes = attributeList.toArray(new Attribute[attributeList.size()]);
		Arrays.sort(result.attributes);
		
		/* resolve methods */
		ArrayList<Operation> operationList = new ArrayList<Operation>();
		for(Method m : javaClass.getDeclaredMethods()) {
			int modifiers = m.getModifiers();
			if(!m.isSynthetic() && ((modifiers & Modifier.PUBLIC)) != 0) {
				int type = Types.fromJavaType(this, m.getGenericReturnType());
				if(type != Types.TYPE_INVALID) {
					Operation op = result.new Operation();
					op.name = m.getName();
					op.type = type;
					op.modifiers = modifiers;
					Type[] argTypes = m.getGenericParameterTypes();
					int argCount = argTypes.length;
					op.args = new int[argCount];
					for(int i = 0; i < argCount; i++) {
						type = Types.fromJavaType(this, argTypes[i]);
						if(type == Types.TYPE_INVALID) {
							op = null;
							break;
						}
						op.args[i] = type;
					}
					if(op != null)
						operationList.add(op);
				}
			}
		}
		result.operations = operationList.toArray(new Operation[operationList.size()]);
		Arrays.sort(result.operations);

		/* init the native part */
		if(getEnv() != null) result.initNative();
		return result;
	}
	
	/**********************
	 * private
	 **********************/
	
	private static int classId2Idx(int classId) {
		return classId >> 1;
	}

	private static int idx2ClassId(int idx, boolean isDict) {
		return (idx << 1) + (isDict ? 1 : 0);
	}

	synchronized int put(IDLInterface iface) {
		int result = (int)iface.getId();
		if(result == -1) {
			result = idx2ClassId(interfaces.size(), iface.isValueType());
			interfaces.add(iface);
			nameMap.put(iface.getName(), iface);
			classMap.put(iface.getJavaClass(), iface);
		}
		return result;
	}
	
	public void dispose() {
		for(IDLInterface iface : interfaces) {
			iface.dispose();
		}
	}
}
