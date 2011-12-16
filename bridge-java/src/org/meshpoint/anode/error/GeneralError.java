package org.meshpoint.anode.error;

/**
 * An exception type wrapping an arbitrary error code
 */
public class GeneralError extends Exception {
	private static final long serialVersionUID = 5692849251925713822L;
	public int errno;
	public GeneralError(int errno, String msg) {super(msg); this.errno = errno;}
}
