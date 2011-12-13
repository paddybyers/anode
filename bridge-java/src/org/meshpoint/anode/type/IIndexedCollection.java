package org.meshpoint.anode.type;

public interface IIndexedCollection<T> {
	public T getIndexedProperty(int idx);
	public void setIndexedProperty(int idx, T value);
	public void deleteIndexedProperty(int idx);
	public boolean containsIndex(int idx);
	public int length();
}
