package org.meshpoint.anode.type;

import java.util.Collection;

public interface ICollection {
	public Object getProperty(String key);
	public void setProperty(String key, Object value);
	public void deleteProperty(String key);
	public boolean containsProperty(String key);
	public Collection<String> properties();
}
