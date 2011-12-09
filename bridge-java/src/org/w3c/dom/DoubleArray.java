package org.w3c.dom;

public interface DoubleArray extends PrimitiveArray {
	public int getLength();
	public void setLength(int length);
	public double getElement(int index);
	public void setElement(int index, double value);
}
