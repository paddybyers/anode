package org.meshpoint.node.type;

import org.meshpoint.node.idl.IDLInterface;

/**
 * A value that implements an IDL interface
 * @author paddy
 *
 */
public interface IInterface extends IValue {
	public IDLInterface getDeclaredType();
}
