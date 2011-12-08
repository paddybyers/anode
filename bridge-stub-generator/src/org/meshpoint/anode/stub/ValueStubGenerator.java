package org.meshpoint.anode.stub;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Modifier;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.InterfaceManager;
import org.meshpoint.anode.idl.StubUtil;

public class ValueStubGenerator extends StubGenerator {

	/********************
	 * private state
	 ********************/
	
	private static final String STUB_IFACE = "org.meshpoint.anode.java.ValueStub";
	
	/********************
	 * public API
	 ********************/
	
	public ValueStubGenerator(InterfaceManager mgr, IDLInterface iface, File destination) {
		super(mgr, iface, destination);
	}
	
	public void generate() throws IOException, GeneratorException {
		if((iface.getModifiers() & Modifier.INTERFACE) != 0)
			throw new GeneratorException("ValueStubGenerator: class must not be an interface", null);
		if(iface.getOperations().length > 0)
			throw new GeneratorException("ValueStubGenerator: class must not have any operations", null);
		String className = iface.getStubClassname();
		ClassWriter cw = new ClassWriter(className, StubUtil.MODE_VALUE);
		try {
			writePreamble(cw, className, iface.getName(), STUB_IFACE, StubUtil.MODE_VALUE);
				/*******************
				 * attribute methods
				 *******************/
				Attribute[] attributes = iface.getAttributes();
				emitArgsArray(cw, attributes.length, true);
	
				/* __import */
				cw.openScope("public void __import(Object[] vals)");
					for(int i = 0; i < attributes.length; i++) {
						Attribute attr = attributes[i];
						registerName(attr.name);
						cw.writeln(attr.name + " = " + getObjectToArgExpression(attr.type, "vals[" + i + "]") + ";");
					}
				cw.closeScope();
				cw.writeln();	

				/* __export */
				cw.openScope("public Object[] __export()");
					for(int i = 0; i < attributes.length; i++) {
						Attribute attr = attributes[i];
						cw.writeln("__args[" + i + "] = " + getArgToObjectExpression(attr.type, attr.name) + ";");
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
