package org.meshpoint.anode.java;

public abstract class Array extends Base {
	boolean isReadOnly;
	boolean isFixedLength;
	
	protected Array(int type, boolean isFixedLength) {
		super(type);
		this.isFixedLength = isFixedLength;
	}
}
