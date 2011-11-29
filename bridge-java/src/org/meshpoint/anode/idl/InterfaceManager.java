package org.meshpoint.anode.idl;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.HashMap;

import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.IDLInterface.Operation;

public class InterfaceManager {

	private Env env;
	private ClassLoader classLoader;
	private ArrayList<IDLInterface> interfaces;
	private HashMap<String, IDLInterface> nameMap;
	private HashMap<Class<? extends Object>, IDLInterface> classMap;
	
	/******************
	 * public API
	 ******************/
	public InterfaceManager(Env env) {
		this.env = env;
		classLoader = this.getClass().getClassLoader(); /* we might support distinct loaders in future */
		interfaces = new ArrayList<IDLInterface>();
		nameMap = new HashMap<String, IDLInterface>();
		classMap = new HashMap<Class<? extends Object>, IDLInterface>();
	}
	
	Env getEnv() {
		return env;
	}

	ClassLoader getClassLoader() {
		return classLoader;
	}

	public synchronized int put(IDLInterface iface) {
		int result = (int)iface.getId();
		if(result == -1) {
			result = interfaces.size();
			interfaces.add(iface);
			nameMap.put(iface.getName(), iface);
			classMap.put(iface.getJavaClass(), iface);
		}
		return result;
	}
	
	public synchronized IDLInterface getById(int id) {
		return interfaces.get(id);
	}
	
	public synchronized IDLInterface getByName(String name) {
		return nameMap.get(name);
	}
	
	/****************
	 * class loading
	 ****************/
	public IDLInterface loadClass(Class<? extends Object> javaClass) {
		IDLInterface result;
		synchronized(this) {
			result = classMap.get(javaClass);
			if(result != null) return result;
		}
		result = defineClass(javaClass);
		return result;
	}
	
	private IDLInterface defineClass(Class<? extends Object> javaClass) {
		String canonicalName = javaClass.getCanonicalName().intern();
		if(canonicalName == "java.lang.Object" || canonicalName.startsWith(Types.INTERFACE_TYPE_PREFIX))
			return null;

		/* add to manager, and resolve parent */
		IDLInterface result = new IDLInterface(env, javaClass);
		result.parent = loadClass(javaClass.getSuperclass());

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
	public void dispose() {
		for(IDLInterface iface : interfaces) {
			iface.dispose();
		}
	}
}
