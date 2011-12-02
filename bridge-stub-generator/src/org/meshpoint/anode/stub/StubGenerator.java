package org.meshpoint.anode.stub;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.lang.reflect.Modifier;
import java.util.HashSet;

import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.InterfaceManager;
import org.meshpoint.anode.idl.Types;
import org.meshpoint.anode.idl.IDLInterface.Operation;

public abstract class StubGenerator {

	/********************
	 * private state
	 ********************/
	
	protected InterfaceManager mgr;
	protected IDLInterface iface;
	protected File destination;
	private static final int MAX_NAME_LENGTH = 80;
	private HashSet<String> memberNames = new HashSet<String>();
	
	/********************
	 * public API
	 ********************/
	
	public static final String STUB_PACKAGE = "org.meshpoint.anode.stub.gen";
	
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
	
	protected static String uclName(String attrName) {
		return Character.toUpperCase(attrName.charAt(0)) + attrName.substring(1);
	}

	protected String hashName(String idlName) {
		String[] parts = idlName.split("\\.");
		StringBuffer candidate = new StringBuffer();
		for(String part: parts) {
			if(candidate.length() == 0) {
				candidate.append(uclName(part));
			} else {
				candidate.append('_');
				candidate.append(part);
			}
		}
		if(candidate.length() > MAX_NAME_LENGTH) {
			/* hash the remainder of the name */
			/* FIXME: implement this */
		}
		return candidate.toString();
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
		if((type & Types.TYPE_ARRAY) > 0)
			return getType(type & ~Types.TYPE_ARRAY) + "[]";
		
		/* interface types */
		if((type & Types.TYPE_INTERFACE) > 0) {
			IDLInterface typeIface = mgr.getById(type >> 16);
			if(typeIface == null)
				throw new GeneratorException("Internal error: referenced class not found (id = " + (type >> 16) + ")", null);
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
		return "get_" + uclName(attrName);
	}

	protected static String setterName(String attrName) {
		return "set_" + uclName(attrName);
	}

	protected static String getArgToObjectExpression(int type, String subExpr) throws GeneratorException {
		String result = subExpr;

		/* array + interface types */
		if((type & (Types.TYPE_ARRAY|Types.TYPE_INTERFACE)) > 0)
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

	protected void emitMaxargsArray(PrintStream ps, IDLInterface iface, boolean includeGetter) {
		Operation[] operations = iface.getOperations();
		int maxArgCount = 0;
		for(Operation op : operations) {
			int thisArgCount = op.args.length;
			if(thisArgCount > maxArgCount)
				maxArgCount = thisArgCount;
		}
		if(maxArgCount > 0) {
			emitArgsArray(ps, maxArgCount, includeGetter);
		}
	}

	protected void emitArgsArray(PrintStream ps, int len, boolean includeGetter) {
		if(len > 0) {
			ps.println("\tprivate static Object[] __args = new Object[" + len + "];");
			ps.println();
			if(includeGetter) {
				ps.println("\tpublic static Object[] __getArgs() { return __args; }");
				ps.println();
			}
		}
	}
}
