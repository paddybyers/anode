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

package org.meshpoint.anode.stub;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Modifier;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.InterfaceManager;
import org.meshpoint.anode.idl.StubUtil;

public class DictionaryStubGenerator extends StubGenerator {

	/********************
	 * public API
	 ********************/
	
	public DictionaryStubGenerator(InterfaceManager mgr, IDLInterface iface, File destination) {
		super(mgr, iface, destination);
	}
	
	public void generate() throws IOException, GeneratorException {
		if((iface.getModifiers() & Modifier.INTERFACE) != 0)
			throw new GeneratorException("ValueStubGenerator: class must not be an interface", null);
		if(iface.getOperations().length > 0)
			throw new GeneratorException("ValueStubGenerator: class must not have any operations", null);
		String className = iface.getStubClassname();
		ClassWriter cw = new ClassWriter(className, StubUtil.MODE_DICT);
		try {
			writePreamble(cw, className, null, null, StubUtil.MODE_DICT);
				/*******************
				 * attribute methods
				 *******************/
				Attribute[] attributes = iface.getAttributes();
				emitArgsArray(cw, attributes.length, true);
	
				/* __import */
				cw.openScope("public static void __import(" + iface.getName() + " ob, Object[] vals)");
					for(int i = 0; i < attributes.length; i++) {
						Attribute attr = attributes[i];
						if((attr.modifiers & Modifier.STATIC) == 0) {
							registerName(attr.name);
							cw.writeln("ob." + attr.name + " = " + getObjectToArgExpression(attr.type, "vals[" + i + "]") + ";");
						}
					}
				cw.closeScope();
				cw.writeln();	

				/* __export */
				cw.openScope("public static Object[] __export(" + iface.getName() + " ob)");
					for(int i = 0; i < attributes.length; i++) {
						Attribute attr = attributes[i];
						if((attr.modifiers & Modifier.STATIC) == 0)
							cw.writeln("__args[" + i + "] = " + getArgToObjectExpression(attr.type, "ob." + attr.name) + ";");
					}
					cw.writeln("return __args;");
				cw.closeScope();
				cw.writeln();	
			cw.closeScope();
		} finally {
			cw.close();
		}
	}

}
