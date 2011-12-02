package org.meshpoint.anode.stub;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.lang.reflect.Modifier;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.IDLInterface.Operation;
import org.meshpoint.anode.idl.InterfaceManager;
import org.meshpoint.anode.idl.Types;

public class ImportStubGenerator extends StubGenerator {
	
	/********************
	 * private state
	 ********************/
	
	private static final String STUB_BASE = "org.meshpoint.anode.js.JSInterface";
	
	/********************
	 * public API
	 ********************/
	
	public ImportStubGenerator(InterfaceManager mgr, IDLInterface iface, File destination) {
		super(mgr, iface, destination);
	}
	
	public void generate() throws IOException, GeneratorException {
		if((iface.getModifiers() & Modifier.INTERFACE) == 0)
			throw new GeneratorException("ImportStubGenerator: class must be an interface", null);
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
			ps.println("public final class " + className + " extends " + STUB_BASE + " implements " + ifaceName + " {");
			ps.println();

			/***************
			 * constructor
			 ****************/
			ps.println('\t' + className + "(long instHandle, org.meshpoint.anode.idl.IDLInterface iface) { super(instHandle, iface); }");
			ps.println();

			/*******************
			 * operation methods
			 *******************/
			emitArgsArray(ps, iface, false);
			Operation[] operations = iface.getOperations();
			for(int i = 0; i < operations.length; i++) {
				Operation op = operations[i];
				registerName(op.name);
				ps.print("\t" + getModifiers(op.modifiers) + " " + getType(op.type) + " " + op.name + "(");
				for(int argIdx = 0; argIdx < op.args.length; argIdx++) {
					/* argument specifier for function signature */
					if(argIdx > 0) ps.print(", ");
					ps.print(getType(op.args[argIdx]) + " " + getArgName(argIdx));
				}
				ps.println(") {");
				for(int argIdx = 0; argIdx < op.args.length; argIdx++) {
					/* argument bashing */
					ps.println("\t\t__args[" + argIdx + "] = " + getArgToObjectExpression(op.args[argIdx], getArgName(argIdx)) + ";");
				}
				String subExpr = "__invoke(" + i + ", __args)";
				if(op.type == Types.TYPE_UNDEFINED) {
					ps.println("\t\t" + subExpr + ";");
				} else {
					ps.println("\t\treturn " + getObjectToArgExpression(op.type, subExpr) + ";");
				}
				ps.println("\t}");
				ps.println();
			}

			/*******************
			 * attribute methods
			 *******************/
			Attribute[] attributes = iface.getAttributes();
			for(int i = 0; i < attributes.length; i++) {
				Attribute attr = attributes[i];
				registerName(attr.name);
				String typeStr = getType(attr.type);
				String modifiersStr = getModifiers(attr.modifiers);
				/* getter */
				ps.println("\t" + modifiersStr + " " + typeStr + " " + getterName(attr.name) + "() {");
				String subExpr = "__get(" + i + ")";
				ps.println("\t\treturn " + getObjectToArgExpression(attr.type, subExpr) + ";");
				ps.println("\t}");
				ps.println();
				/* setter */
				ps.println("\t" + modifiersStr + " void " + setterName(attr.name) + "(" + typeStr + " arg0) {");
				ps.println("\t\t__set(" + i + ", " + getArgToObjectExpression(attr.type, getArgName(0)) + ");");
				ps.println("\t}");
				ps.println();
			}
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
