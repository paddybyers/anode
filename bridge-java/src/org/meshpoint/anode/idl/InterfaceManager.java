package org.meshpoint.anode.idl;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.HashMap;

import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.IDLInterface.Operation;

public class InterfaceManager {

	private ClassLoader classLoader;
	private ArrayList<IDLInterface> interfaces;
	private HashMap<String, IDLInterface> nameMap;
	private HashMap<Class<? extends Object>, IDLInterface> classMap;
	
	/******************
	 * public API
	 ******************/
	public InterfaceManager(ClassLoader classLoader) {
		if(classLoader == null)
			classLoader = this.getClass().getClassLoader();
		this.classLoader = classLoader;
		interfaces = new ArrayList<IDLInterface>();
		nameMap = new HashMap<String, IDLInterface>();
		classMap = new HashMap<Class<? extends Object>, IDLInterface>();
	}
	
	ClassLoader getClassLoader() {
		return classLoader;
	}

	public synchronized IDLInterface getById(int id) {
		return interfaces.get(id);
	}
	
	public synchronized IDLInterface getByName(String name) {
		IDLInterface result = nameMap.get(name);
		if(result != null)
			return result;
		
		try {
			Class<?> javaClass = classLoader.loadClass(name);
			result = loadClass(javaClass);
		} catch (ClassNotFoundException e) {}
		return result;
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
		if(canonicalName == "java.lang.Object")
			return null;

		/* add to manager, and resolve parent */
		IDLInterface result = new IDLInterface(this, javaClass);
		Class<?> parentClass = javaClass.getSuperclass();
		if(parentClass != null)
			result.parent = loadClass(parentClass);

		/* resolve fields */
		ArrayList<Attribute> attributeList = new ArrayList<Attribute>();
		for(Field f : javaClass.getDeclaredFields()) {
			int modifiers = f.getModifiers();
			if(!f.isSynthetic() && (modifiers & Modifier.PRIVATE) == 0) {
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
		
		/* resolve methods */
		ArrayList<Operation> operationList = new ArrayList<Operation>();
		for(Method m : javaClass.getDeclaredMethods()) {
			int modifiers = m.getModifiers();
			if(!m.isSynthetic() && (modifiers & Modifier.PRIVATE) == 0) {
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

		return result;
	}
	
	/**********************
	 * private
	 **********************/
	synchronized int put(IDLInterface iface) {
		int result = (int)iface.getId();
		if(result == -1) {
			result = interfaces.size();
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
