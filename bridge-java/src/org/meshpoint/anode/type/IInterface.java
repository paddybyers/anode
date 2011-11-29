package org.meshpoint.anode.type;

import org.meshpoint.anode.idl.IDLInterface;

/**
 * A value that implements an IDL interface
 * @author paddy
 *
 */
public interface IInterface extends IValue {
	public IDLInterface getDeclaredType();
}
