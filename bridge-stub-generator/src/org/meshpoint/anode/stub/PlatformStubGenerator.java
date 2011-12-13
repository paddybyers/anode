package org.meshpoint.anode.stub;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Modifier;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.IDLInterface.Operation;
import org.meshpoint.anode.idl.InterfaceManager;
import org.meshpoint.anode.idl.StubUtil;

public class PlatformStubGenerator extends StubGenerator {

	/********************
	 * public API
	 ********************/
	
	public PlatformStubGenerator(InterfaceManager mgr, IDLInterface iface, File destination) {
		super(mgr, iface, destination);
	}
	
	public void generate() throws IOException, GeneratorException {
		if((iface.getModifiers() & Modifier.INTERFACE) != 0)
			throw new GeneratorException("ExportStubGenerator: class must not be an interface", null);
		String ifaceName = iface.getName();
		String className = iface.getStubClassname();
		ClassWriter cw = new ClassWriter(className, StubUtil.MODE_PLATFORM);
		try {
			writePreamble(cw, className, null, null, StubUtil.MODE_PLATFORM);

			/*******************
			 * invoke method
			 *******************/
			Operation[] operations = iface.getOperations();
			if(operations.length > 0) {
				emitMaxargsArray(cw, iface, true);
				cw.openScope("static Object __invoke(" + ifaceName + " inst, int opIdx, Object[] args)");
				if(operations.length == 1) {
					/* no switch statement */
					Operation op = operations[0];
					registerName(op.name);
					cw.writeln("return " + getInvokeCaseBodyExpression(op, "\t\t") + ";");
				} else {
					/* switch needed */
					cw.writeln("Object result = null;");
					cw.openScope("switch(opIdx)");
						for(int i = 0; i < operations.length; i++) {
							Operation op = operations[i];
							registerName(op.name);
							cw.writeln("case " + i + ": /* " + op.name + "*/", -1);
							cw.writeln("result = " + getInvokeCaseBodyExpression(op, "\t\t\t") + ";");
							cw.writeln("break;");
						}
						cw.writeln("default:", -1);
					cw.closeScope();
					cw.writeln("return result;");
				}
				cw.closeScope();
				cw.writeln();	
			}

			/*******************
			 * attribute methods
			 *******************/
			Attribute[] attributes = iface.getAttributes();
			if(attributes.length > 0) {
				/* __get */
				cw.openScope("static Object __get(" + ifaceName + " inst, int attrIdx)");
				if(attributes.length == 1) {
					/* no switch statement */
					Attribute attr = attributes[0];
					registerName(attr.name);
					cw.writeln("return " + getArgToObjectExpression(attr.type, getAttrAccessExpression(ifaceName, attr)) + ";");
				} else {
					/* switch needed */
					cw.writeln("Object result = null;");
					cw.openScope("switch(attrIdx)");
						for(int i = 0; i < attributes.length; i++) {
							Attribute attr = attributes[i];
							registerName(attr.name);
							cw.writeln("case " + i + ": /* " + attr.name + "*/", -1);
							cw.writeln("result = " + getArgToObjectExpression(attr.type, getAttrAccessExpression(ifaceName, attr)) + ";");
							cw.writeln("break;");
						}
						cw.writeln("default:", -1);
					cw.closeScope();
					cw.writeln("return result;");
				}
				cw.closeScope();
				cw.writeln();	

				/* __set */
				cw.openScope("static void __set(" + ifaceName + " inst, int attrIdx, Object val)");
				if(attributes.length == 1) {
					/* no switch statement */
					Attribute attr = attributes[0];
					cw.writeln(getAttrAccessExpression(ifaceName, attr) + " = " + getObjectToArgExpression(attr.type, "val") + ";");
				} else {
					/* switch needed */
					cw.openScope("switch(attrIdx)");
						for(int i = 0; i < attributes.length; i++) {
							Attribute attr = attributes[i];
							cw.writeln("case " + i + ": /* " + attr.name + "*/", -1);
							cw.writeln(getAttrAccessExpression(ifaceName, attr) + " = " + getObjectToArgExpression(attr.type, "val") + ";");
							cw.writeln("break;");
						}
						cw.writeln("default:", -1);
					cw.closeScope();
				}
				cw.closeScope();
				cw.writeln();	
			}
			cw.closeScope();
		} finally {
			cw.close();
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
