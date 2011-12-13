package org.w3c.dom;

public interface ByteArray extends PrimitiveArray {
	public int getLength();
	public void setLength(int length);
	public byte getElement(int index);
	public void setElement(int index, byte value);
}
