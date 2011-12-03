package org.meshpoint.anode.stub;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.lang.reflect.Modifier;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.IDLInterface.Operation;
import org.meshpoint.anode.idl.InterfaceManager;

public class ExportStubGenerator extends StubGenerator {

	/********************
	 * public API
	 ********************/
	
	public ExportStubGenerator(InterfaceManager mgr, IDLInterface iface, File destination) {
		super(mgr, iface, destination);
	}
	
	public void generate() throws IOException, GeneratorException {
		if((iface.getModifiers() & Modifier.INTERFACE) != 0)
			throw new GeneratorException("ExportStubGenerator: class must not be an interface", null);
		String ifaceName = iface.getName();
		String className = hashName(ifaceName);
		PrintStream ps = openStream(className);
		try {
			/***************
			 * preamble
			 ****************/
			ps.println("/* This file has been automatically generated; do not edit */");
			ps.println();
			ps.println("package " + STUB_PACKAGE + ';');
			ps.println();
			ps.println("public final class " + className + " {");
			ps.println();

			/*******************
			 * invoke method
			 *******************/
			Operation[] operations = iface.getOperations();
			if(operations.length > 0) {
				emitMaxargsArray(ps, iface, true);
				ps.println("\tstatic Object __invoke(" + ifaceName + " inst, int opIdx, Object[] args) {");
				if(operations.length == 1) {
					/* no switch statement */
					Operation op = operations[0];
					registerName(op.name);
					ps.println("\t\treturn " + getInvokeCaseBodyExpression(op, "\t\t") + ";");
				} else {
					/* switch needed */
					ps.println("\t\tObject result = null;");
					ps.println("\t\tswitch(opIdx) {");
					for(int i = 0; i < operations.length; i++) {
						Operation op = operations[i];
						registerName(op.name);
						ps.println("\t\tcase " + i + ": /* " + op.name + "*/");
						ps.println("\t\t\tresult = " + getInvokeCaseBodyExpression(op, "\t\t\t") + ";");
						ps.println("\t\t\tbreak;");
					}
					ps.println("\t\tdefault:");
					ps.println("\t\t}");
					ps.println("\t\treturn result;");
				}
				ps.println("\t}");
				ps.println();
			}

			/*******************
			 * attribute methods
			 *******************/
			Attribute[] attributes = iface.getAttributes();
			if(attributes.length > 0) {
				/* __get */
				ps.println("\tstatic Object __get(" + ifaceName + " inst, int attrIdx) {");
				if(attributes.length == 1) {
					/* no switch statement */
					Attribute attr = attributes[0];
					registerName(attr.name);
					ps.println("\t\treturn " + getArgToObjectExpression(attr.type, getAttrAccessExpression(ifaceName, attr)) + ";");
				} else {
					/* switch needed */
					ps.println("\t\tObject result = null;");
					ps.println("\t\tswitch(attrIdx) {");
					for(int i = 0; i < attributes.length; i++) {
						Attribute attr = attributes[i];
						registerName(attr.name);
						ps.println("\t\tcase " + i + ": /* " + attr.name + "*/");
						ps.println("\t\t\tresult = " + getArgToObjectExpression(attr.type, getAttrAccessExpression(ifaceName, attr)) + ";");
						ps.println("\t\t\tbreak;");
					}
					ps.println("\t\tdefault:");
					ps.println("\t\t}");
					ps.println("\t\treturn result;");
				}
				ps.println("\t}");
				ps.println();
				/* __set */
				ps.println("\tstatic void __set(" + ifaceName + " inst, int attrIdx, Object val) {");
				if(attributes.length == 1) {
					/* no switch statement */
					Attribute attr = attributes[0];
					ps.println("\t\t" + getAttrAccessExpression(ifaceName, attr) + " = " + getObjectToArgExpression(attr.type, "val") + ";");
				} else {
					/* switch needed */
					ps.println("\t\tswitch(attrIdx) {");
					for(int i = 0; i < attributes.length; i++) {
						Attribute attr = attributes[i];
						ps.println("\t\tcase " + i + ": /* " + attr.name + "*/");
						ps.println("\t\t\t" + getAttrAccessExpression(ifaceName, attr) + " = " + getObjectToArgExpression(attr.type, "val") + ";");
						ps.println("\t\t\tbreak;");
					}
					ps.println("\t\tdefault:");
					ps.println("\t\t}");
				}
				ps.println("\t}");
				ps.println();
			}

			/***************
			 * postamble
			 ***************/
			ps.println("}");
		} finally {
			closeStream(ps);
		}
	}

	/***************
	 * helpers
	 ***************/
	private String getInvokeCaseBodyExpression(Operation op, String indentPrefix) throws GeneratorException {
		StringBuffer subExpr = new StringBuffer("inst." + op.name + "(");
		if(op.args.length > 0) {
			subExpr.append('\n');
			for(int i = 0; i < op.args.length; i++) {
				subExpr.append(indentPrefix);
				subExpr.append('\t');
				subExpr.append(getObjectToArgExpression(op.args[i], "args[" + i + "]"));
				if(i + 1 < op.args.length) subExpr.append(',');
				subExpr.append('\n');
			}
			subExpr.append(indentPrefix);			
		}
		subExpr.append(')');
		return getArgToObjectExpression(op.type, subExpr.toString());
	}
	
	private static String getAttrAccessExpression(String ifaceName, Attribute attr) {
		return (((attr.modifiers & Modifier.STATIC) > 0) ? ifaceName : "inst") + '.' + attr.name;
	}
}
