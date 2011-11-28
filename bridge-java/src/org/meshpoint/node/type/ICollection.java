package org.meshpoint.node.type;

import java.util.Collection;

public interface ICollection extends IValue {
	public IValue getProperty(String key);
	public void setProperty(String key, IValue value);
	public void deleteProperty(String key);
	public boolean containsProperty(String key);
	public Collection<String> properties();
}
