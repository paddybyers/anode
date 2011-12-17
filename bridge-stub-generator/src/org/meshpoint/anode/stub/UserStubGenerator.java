package org.meshpoint.anode.stub;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Modifier;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.IDLInterface.Attribute;
import org.meshpoint.anode.idl.IDLInterface.Operation;
import org.meshpoint.anode.idl.InterfaceManager;
import org.meshpoint.anode.idl.StubUtil;
import org.meshpoint.anode.idl.Types;

public class UserStubGenerator extends StubGenerator {
	
	/********************
	 * private state
	 ********************/
	
	private static final String STUB_BASE = "org.meshpoint.anode.js.JSInterface";
	
	/********************
	 * public API
	 ********************/
	
	public UserStubGenerator(InterfaceManager mgr, IDLInterface iface, File destination) {
		super(mgr, iface, destination);
	}
	
	public void generate() throws IOException, GeneratorException {
		if((iface.getModifiers() & Modifier.INTERFACE) == 0)
			throw new GeneratorException("UserStubGenerator: class must be an interface", null);
		String className = iface.getStubClassname();
		ClassWriter cw = new ClassWriter(className, StubUtil.MODE_USER);
		try {
			writePreamble(cw, className, STUB_BASE, iface.getName(), StubUtil.MODE_USER);
				/***************
				 * statics
				 ****************/
				cw.writeln("static int classId = org.meshpoint.anode.bridge.Env.getCurrent().getInterfaceManager().getByClass(" + iface.getName() + ".class).getId();");
				cw.writeln();

				/***************
				 * constructor
				 ****************/
				cw.writeln(className + "(long instHandle) { super(instHandle); }");
				cw.writeln();
	
				/***************
				 * finalizer
				 ****************/
				cw.writeln("public void finalize() { super.release(classId); }");
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
						String subExpr = "__invoke(classId, " + i + ", __args)";
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
						String subExpr = "__get(classId, " + i + ")";
						cw.writeln("return " + getObjectToArgExpression(attr.type, subExpr) + ";");
					cw.closeScope();
					cw.writeln();	
					/* setter */
					cw.openScope(modifiersStr + " void " + setterName(attr.name) + "(" + typeStr + " arg0) {");
						cw.writeln("__set(classId, " + i + ", " + getArgToObjectExpression(attr.type, getArgName(0)) + ");");
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
