package org.meshpoint.anode.stub;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.lang.reflect.Modifier;
import java.util.HashSet;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.InterfaceManager;
import org.meshpoint.anode.idl.StubUtil;
import org.meshpoint.anode.idl.Types;
import org.meshpoint.anode.idl.IDLInterface.Operation;

public abstract class StubGenerator {

	/********************
	 * private state
	 ********************/
	
	protected InterfaceManager mgr;
	protected IDLInterface iface;
	protected File destination;
	private HashSet<String> memberNames = new HashSet<String>();
	
	/********************
	 * public API
	 ********************/
	
	public static class GeneratorException extends Exception {
		private static final long serialVersionUID = 5134576967433238034L;
		GeneratorException(String msg, Exception cause) {
			super(msg, cause);
		}
	}
	
	public StubGenerator(InterfaceManager mgr, IDLInterface iface, File destination) {
		this.mgr = mgr;
		this.iface = iface;
		this.destination = destination;
	}
	
	public abstract void generate() throws IOException, GeneratorException;

	/********************
	 * private API
	 ********************/
	
	protected void registerName(String memberName) throws GeneratorException {
		if(memberNames.contains(memberName))
			throw new GeneratorException("StubGenerator: overloaded operation or attribute name (" + memberName + ")", null);
		memberNames.add(memberName);
	}
	
	protected void writePreamble(ClassWriter lw, String className, String superclassName, String implementsName, int mode) {
		lw.writeln("/* This file has been automatically generated; do not edit */");
		lw.writeln();
		lw.writeln("package " + StubUtil.getStubPackage(mode) + ';');
		lw.writeln();
		StringBuffer header = new StringBuffer().append("public class ").append(className);
		if(superclassName != null) header.append(" extends ").append(superclassName);
		if(implementsName != null) header.append(" implements ").append(implementsName);
		lw.openScope(header.toString());
		lw.writeln();
	}
	
	protected static String getModifiers(int modifiers) {
		StringBuffer buf = new StringBuffer();
		if((modifiers & Modifier.PUBLIC) != 0)
			buf.append("public");
		if((modifiers & Modifier.STATIC) != 0) {
			if(buf.length() > 0) buf.append(' ');
			buf.append("static");
		}
		if((modifiers & Modifier.FINAL) != 0) {
			if(buf.length() > 0) buf.append(' ');
			buf.append("final");
		}
		return buf.toString();
	}

	protected String getType(int type) throws GeneratorException {
		/* array types */
		if((type & Types.TYPE_SEQUENCE) > 0)
			return getType(type & ~Types.TYPE_SEQUENCE) + "[]";
		
		if((type & Types.TYPE_ARRAY) > 0) {
			int componentType = type & ~Types.TYPE_ARRAY;
			if(componentType < Types.TYPE_STRING)
				return "org.w3c.dom." + getType(componentType|Types.TYPE_OBJECT) + "Array";
			return "org.w3c.dom.ObjectArray<" + getType(componentType) + ">";
		}
		
		if((type & Types.TYPE_MAP) > 0) {
			int componentType = type & ~Types.TYPE_MAP;
			return "java.util.HashMap<String, " + getType(componentType) + ">";
		}
		
		/* interface types */
		if((type & Types.TYPE_INTERFACE) > 0) {
			IDLInterface typeIface = mgr.getById(Types.type2classid(type));
			if(typeIface == null)
				throw new GeneratorException("Internal error: referenced class not found (id = " + Types.type2classid(type) + ")", null);
			return typeIface.getName();
		}
		
		/* others */
		String result;
		switch(type) {
		default:
			throw new GeneratorException("Illegal type encountered (type = " + type + ")", null);
		case Types.TYPE_UNDEFINED:
			result = "void";
			break;
		case Types.TYPE_BOOL:
			result = "boolean";
			break;
		case Types.TYPE_BYTE:
			result = "byte";
			break;
		case Types.TYPE_INT:
			result = "int";
			break;
		case Types.TYPE_LONG:
			result = "long";
			break;
		case Types.TYPE_DOUBLE:
			result = "double";
			break;
		case Types.TYPE_STRING:
			result = "String";
			break;
		case Types.TYPE_DATE:
			result = "java.util.Date";
			break;
		case Types.TYPE_OBJECT:
			result = "Object";
			break;
		case Types.TYPE_OBJECT|Types.TYPE_BOOL:
			result = "Boolean";
			break;
		case Types.TYPE_OBJECT|Types.TYPE_BYTE:
			result = "Byte";
			break;
		case Types.TYPE_OBJECT|Types.TYPE_INT:
			result = "Integer";
			break;
		case Types.TYPE_OBJECT|Types.TYPE_LONG:
			result = "Long";
			break;
		case Types.TYPE_OBJECT|Types.TYPE_DOUBLE:
			result = "Double";
			break;
		case Types.TYPE_OBJECT|Types.TYPE_STRING:
			result = "String";
			break;
		}
		return result;
	}
	
	protected static String getArgName(int argIdx) {
		return "arg" + argIdx;
	}

	protected static String getterName(String attrName) {
		return "get_" + StubUtil.uclName(attrName);
	}

	protected static String setterName(String attrName) {
		return "set_" + StubUtil.uclName(attrName);
	}

	protected static String getArgToObjectExpression(int type, String subExpr) throws GeneratorException {
		String result = subExpr;

		/* void */
		if(type == Types.TYPE_UNDEFINED)
			return result;

		/* array + interface types */
		if((type & (Types.TYPE_SEQUENCE|Types.TYPE_ARRAY|Types.TYPE_MAP|Types.TYPE_INTERFACE)) > 0)
			return result;
		
		/* others */
		switch(type) {
		default:
			throw new GeneratorException("Illegal type encountered (type = " + type + ")", null);
		case Types.TYPE_BOOL:
			result = "org.meshpoint.anode.js.JSValue.asJSBoolean(" + result + ")";
			break;
		case Types.TYPE_INT:
			result = "org.meshpoint.anode.js.JSValue.asJSNumber((long)" + result + ")";
			break;
		case Types.TYPE_LONG:
		case Types.TYPE_DOUBLE:
			result = "org.meshpoint.anode.js.JSValue.asJSNumber(" + result + ")";
			break;
		case Types.TYPE_STRING:
		case Types.TYPE_DATE:
		case Types.TYPE_OBJECT:
		case Types.TYPE_OBJECT|Types.TYPE_BOOL:
		case Types.TYPE_OBJECT|Types.TYPE_BYTE:
		case Types.TYPE_OBJECT|Types.TYPE_INT:
		case Types.TYPE_OBJECT|Types.TYPE_LONG:
		case Types.TYPE_OBJECT|Types.TYPE_DOUBLE:
		case Types.TYPE_OBJECT|Types.TYPE_STRING:
		}
		return result;
	}
	
	protected String getCastExpression(int type) throws GeneratorException {
		return "(" + getType(type) + ")";
	}
	
	protected String getObjectToArgExpression(int type, String subExpr) throws GeneratorException {
		String result;
		String jsCastExpr = "((org.meshpoint.anode.js.JSValue)" + subExpr + ')';
		switch(type) {
		default:
			result = getCastExpression(type) + subExpr;
			break;
		case Types.TYPE_BOOL:
			result = jsCastExpr + ".getBooleanValue()";
			break;
		case Types.TYPE_INT:
			result = "(int)" + jsCastExpr + ".longValue";
			break;
		case Types.TYPE_LONG:
			result = jsCastExpr + ".longValue";
			break;
		case Types.TYPE_DOUBLE:
			result = jsCastExpr + ".dblValue";
			break;
		case Types.TYPE_OBJECT:
			result = subExpr;
		}
		return result;
	}

	protected void emitMaxargsArray(ClassWriter lw, IDLInterface iface, boolean includeGetter) {
		Operation[] operations = iface.getOperations();
		int maxArgCount = 0;
		for(Operation op : operations) {
			int thisArgCount = op.args.length;
			if(thisArgCount > maxArgCount)
				maxArgCount = thisArgCount;
		}
		emitArgsArray(lw, maxArgCount, includeGetter);
	}

	protected void emitArgsArray(ClassWriter lw, int len, boolean includeGetter) {
		lw.writeln("private static Object[] __args = new Object[" + len + "];");
		lw.writeln();
		if(includeGetter) {
			lw.writeln("public static Object[] __getArgs() { return __args; }");
			lw.writeln();
		}
	}

	protected class ClassWriter {
		/**
		 * Create a ClassWriter instance for the specified stub class
		 * @param className class name, without package component
		 * @param mode stub mode
		 * @throws IOException
		 */
		public ClassWriter(String className, int mode) throws IOException {
			String stubPackage = StubUtil.getStubPackage(mode).replace('.', '/');
			File packageDir = new File(destination.toString() + '/' + stubPackage);
			packageDir.mkdirs();
			if(!packageDir.exists())
				throw new IOException("Unable to create package directory (" + packageDir.toString() + ")");
			
			String classFilename = className + ".java";
			File classFile = new File(packageDir, classFilename);
			FileOutputStream fos = new FileOutputStream(classFile);
			this.ps = new PrintStream(fos);
		}
		public void writeln() { ps.println(); }
		public void writeln(String s) { ps.append(indentChars, 0, indent); ps.println(s); }
		public void writeln(String s, int indentDelta) { indent += indentDelta; writeln(s); indent -= indentDelta; }
		public void openScope(String s) { writelnStart(s); openScope(); }
		public void closeScope() { --indent; writeln("}"); }
		public void close() { ps.flush(); ps.close(); }
		private void writelnStart(String s) { ps.append(indentChars, 0, indent); ps.print(s); }
		private void writelnEnd(String s) { ps.println(s); }
		private void openScope() { writelnEnd(" {"); indent++; }
		private PrintStream ps;
		private int indent;
		private CharSequence indentChars = "\t\t\t\t\t";
	}
}
