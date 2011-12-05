package org.meshpoint.anode.idl;

public class StubUtil {
	
	public static final int MODE_IMPORT = 0;
	public static final int MODE_EXPORT = 1;
	public static final int MODE_VALUE  = 2;
	
	public static final String[] modes = new String[]{"import", "export", "value"};

	private static final String STUB_PACKAGE = "org.meshpoint.anode.stub.gen.";
	
	public static String getStubPackage(int mode) {
		return STUB_PACKAGE + modes[mode];
	}

	public static String uclName(String attrName) {
		return Character.toUpperCase(attrName.charAt(0)) + attrName.substring(1);
	}

}
