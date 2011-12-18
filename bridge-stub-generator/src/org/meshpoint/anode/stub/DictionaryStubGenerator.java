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
