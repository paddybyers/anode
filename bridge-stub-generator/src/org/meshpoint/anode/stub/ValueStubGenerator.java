package org.meshpoint.anode.stub;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.lang.reflect.Modifier;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.InterfaceManager;

public class ValueStubGenerator extends StubGenerator {

	/********************
	 * private state
	 ********************/
	
	private static final String STUB_IFACE = "org.meshpoint.anode.java.ValueType";
	
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
		String ifaceName = iface.getName();
		String className = hashName(ifaceName);
		String classFilename = className + ".java";
		String packagePath = STUB_PACKAGE.replace('.', '/');
		File packageDir = new File(destination.toString() + '/' + packagePath);
		packageDir.mkdirs();
		if(!packageDir.exists())
			throw new IOException("Unable to create package directory (" + packageDir.toString() + ")");
		
		File classFile = new File(packageDir, classFilename);
		FileOutputStream fos = new FileOutputStream(classFile);
		PrintStream ps = new PrintStream(fos);
		try {
			/***************
			 * preamble
			 ****************/
			ps.println("/* This file has been automatically generated; do not edit */");
			ps.println();
			ps.println("package " + STUB_PACKAGE + ';');
			ps.println();
			ps.println("public final class " + className + " extends " + ifaceName + " implements " + STUB_IFACE + " {");
			ps.println();

			/*******************
			 * attribute methods
			 *******************/
			Attribute[] attributes = iface.getAttributes();
			emitArgsArray(ps, attributes.length, true);

			/* __import */
			ps.println("\tpublic void __import(Object[] vals) {");
			for(int i = 0; i < attributes.length; i++) {
				Attribute attr = attributes[i];
				registerName(attr.name);
				ps.println("\t\t" + attr.name + " = " + getObjectToArgExpression(attr.type, "vals[" + i + "]") + ";");
			}
			ps.println("\t}");
			ps.println("");

			/* __export */
			ps.println("\tpublic Object[] __export() {");
			for(int i = 0; i < attributes.length; i++) {
				Attribute attr = attributes[i];
				ps.println("\t\t__args[" + i + "] = " + getArgToObjectExpression(attr.type, attr.name) + ";");
			}
			ps.println("\t\treturn __args;");
			ps.println("\t}");
			ps.println("");

			/***************
			 * postamble
			 ***************/
			ps.println("}");
		} finally {
			fos.flush();
			fos.close();
		}
	}

}
