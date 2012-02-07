/*
 * Copyright 2011-2012 Paddy Byers
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

package org.meshpoint.anode.idl;

public class StubUtil {
	
	public static final int MODE_USER = 0;
	public static final int MODE_PLATFORM = 1;
	public static final int MODE_DICT  = 2;
	
	public static final String[] modes = new String[]{"user", "platform", "dict"};

	private static final String STUB_PACKAGE = "org.meshpoint.anode.stub.gen.";
	
	public static String getStubPackage(int mode) {
		return STUB_PACKAGE + modes[mode];
	}

	public static String uclName(String attrName) {
		return Character.toUpperCase(attrName.charAt(0)) + attrName.substring(1);
	}

}
