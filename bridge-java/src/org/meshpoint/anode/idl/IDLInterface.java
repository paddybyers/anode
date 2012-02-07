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

/**
 * An interface defined in IDL for use within one or more modules
 * @author paddy
 *
 */
public class IDLInterface {
	/********************
	 * private state
	 ********************/

	private static final int MAX_NAME_LENGTH = 80;
	private short id = -1;
	private String name;
	private Class<?> javaClass;
	private boolean isCallback;
	private boolean isValueType;

	/********************
	 * interface metadata
	 ********************/

	int modifiers;
	IDLInterface parent;
	Attribute[] attributes;
	Operation[] operations;

	/**
	 * A class encapsulating the metadata for an attribute
	 */
	public class Attribute implements Comparable<Attribute> {
		public String name;
		public int type;
		public int modifiers;
		@Override
		public int compareTo(Attribute attr) { return name.compareTo(attr.name); }
	}

	/**
	 * A class encapsulating the metadata for an operation (method)
	 */
	public class Operation implements Comparable<Operation> {
		public String name;
		public int type;
		public int modifiers;
		public int[] args;
		@Override
		public int compareTo(Operation op) { return name.compareTo(op.name); }
	}

	/********************
	 * public API
	 ********************/
	
	public IDLInterface(InterfaceManager mgr, Class<?> javaClass) {
		name = javaClass.getCanonicalName();
		this.javaClass = javaClass;
		if(Callback.class.isAssignableFrom(javaClass))
			isCallback = true;
		if(Dictionary.class.isAssignableFrom(javaClass))
			isValueType = true;
		id = mgr.put(this);
	}

	public short getId() {
		return id;
	}

	public Class<?> getJavaClass() {
		return javaClass;
	}

	public IDLInterface getParent() {
		return parent;
	}
	
	public String getName() {
		return name;
	}
	
	public int getModifiers() {
		return modifiers;
	}

	public Attribute[] getAttributes() {
		return attributes;
	}

	public Operation[] getOperations() {
		return operations;
	}

	public String getStubClassname() {
		String candidate = StubUtil.uclName(name.replace('.', '_'));
		if(candidate.length() > MAX_NAME_LENGTH) {
			/* hash the remainder of the name */
			/* FIXME: implement this */
		}
		return candidate;
	}
	
	public boolean isCallback() { return isCallback; }
	public boolean isValueType() { return isValueType; }
}
