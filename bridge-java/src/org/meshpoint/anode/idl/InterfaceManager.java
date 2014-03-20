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

	private static InterfaceManager mgr = new InterfaceManager(null, true);
	private ClassLoader classLoader;
	private boolean initClasses;
	private ArrayList<IDLInterface> interfaces;
	private HashMap<String, IDLInterface> nameMap;
	private HashMap<Class<?>, IDLInterface> classMap;
	
	/******************
	 * public API
	 ******************/
	
	public static InterfaceManager getInstance() { return mgr; }

	public InterfaceManager(ClassLoader classLoader, boolean initClasses) {
		this.classLoader = classLoader;
		this.initClasses = initClasses;
		interfaces = new ArrayList<IDLInterface>();
		nameMap = new HashMap<String, IDLInterface>();
		classMap = new HashMap<Class<?>, IDLInterface>();
	}
	
	ClassLoader getClassLoader() {
		return (classLoader != null) ? classLoader : Env.getCurrent().getClassLoader();
	}

	public synchronized IDLInterface getById(short id) {
		return interfaces.get(classId2Idx(id));
	}
	
	public synchronized IDLInterface getByName(String name) {
		IDLInterface result = nameMap.get(name);
		if(result != null)
			return result;
		
		try {
			Class<?> javaClass;
			if(initClasses)
				javaClass = Class.forName(name, true, getClassLoader());
			else
				javaClass = getClassLoader().loadClass(name);
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
		if(canonicalName.startsWith("java.lang") || canonicalName.startsWith("org.meshpoint.anode."))
			return null;

		/* add to manager, and resolve parent */
		IDLInterface result = new IDLInterface(this, javaClass);
		result.modifiers = javaClass.getModifiers();
		if(javaClass.isInterface()) {
			Class<?>[] parentInterfaces = javaClass.getInterfaces();
			if(parentInterfaces.length == 1)
				result.parent = loadClass(parentInterfaces[0]);
		} else {
			Class<?> parentClass = javaClass.getSuperclass();
			if(parentClass != null)
				result.parent = loadClass(parentClass);
		}

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
		
		return result;
	}
	
	/**********************
	 * private
	 **********************/

	public static int classId2Idx(short classId) {
		return classId >> 1;
	}

	private static short idx2ClassId(int idx, boolean isDict) {
		return (short)((idx << 1) + (isDict ? 1 : 0));
	}

	synchronized short put(IDLInterface iface) {
		short result = iface.getId();
		if(result == -1) {
			result = idx2ClassId(interfaces.size(), iface.isValueType());
			interfaces.add(iface);
			nameMap.put(iface.getName(), iface);
			classMap.put(iface.getJavaClass(), iface);
		}
		return result;
	}
}
