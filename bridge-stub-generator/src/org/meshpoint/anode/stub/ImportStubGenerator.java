package org.meshpoint.anode.stub;

import java.io.File;
import java.io.IOException;
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
		String className = hashName(iface.getName());
		ClassWriter cw = new ClassWriter(className);
		try {
			writePreamble(cw, className, STUB_BASE, iface.getName());
				/***************
				 * constructor
				 ****************/
				cw.writeln(className + "(long instHandle, org.meshpoint.anode.idl.IDLInterface iface) { super(instHandle, iface); }");
				cw.writeln();
	
				/*******************
				 * operation methods
				 *******************/
				emitMaxargsArray(cw, iface, false);
				Operation[] operations = iface.getOperations();
				for(int i = 0; i < operations.length; i++) {
					Operation op = operations[i];
					registerName(op.name);
					cw.openScope(getModifiers(op.modifiers) + " " + getType(op.type) + " " + op.name + "(" + getArgListExpression(op) + ")");
						for(int argIdx = 0; argIdx < op.args.length; argIdx++) {
							/* argument bashing */
							cw.writeln("__args[" + argIdx + "] = " + getArgToObjectExpression(op.args[argIdx], getArgName(argIdx)) + ";");
						}
						String subExpr = "__invoke(" + i + ", __args)";
						if(op.type == Types.TYPE_UNDEFINED) {
							cw.writeln(subExpr + ";");
						} else {
							cw.writeln("return " + getObjectToArgExpression(op.type, subExpr) + ";");
						}
					cw.closeScope();
					cw.writeln();	
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
					cw.openScope(modifiersStr + " " + typeStr + " " + getterName(attr.name) + "()");
						String subExpr = "__get(" + i + ")";
						cw.writeln("return " + getObjectToArgExpression(attr.type, subExpr) + ";");
					cw.closeScope();
					cw.writeln();	
					/* setter */
					cw.openScope(modifiersStr + " void " + setterName(attr.name) + "(" + typeStr + " arg0) {");
						cw.writeln("__set(" + i + ", " + getArgToObjectExpression(attr.type, getArgName(0)) + ");");
					cw.closeScope();
					cw.writeln();	
				}
			cw.closeScope();
		} finally {
			cw.close();
		}
	}

	private String getArgListExpression(Operation op) throws GeneratorException {
		StringBuffer buf = new StringBuffer();
		for(int argIdx = 0; argIdx < op.args.length; argIdx++) {
			if(argIdx > 0) buf.append(", ");
			buf.append(getType(op.args[argIdx]) + " " + getArgName(argIdx));
		}
		return buf.toString();
	}
}
