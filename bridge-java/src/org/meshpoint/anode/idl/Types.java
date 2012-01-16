package org.meshpoint.anode.idl;

import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.List;

import org.w3c.dom.Array;
import org.w3c.dom.ByteArray;
import org.w3c.dom.DoubleArray;
import org.w3c.dom.IntegerArray;
import org.w3c.dom.LongArray;
import org.w3c.dom.ObjectArray;

/**
 * A single integer is used to represent a type of any argument
 * or return value of an operation.
 * For interface types, and array<interface> types, the type
 * includes the class id as (TYPE_INTERFACE | (classid << 16))
 * or (TYPE_INTERFACE | [TYPE_ARRAY|TYPE_SEQUENCE|TYPE_MAP] | (classid << 16))
 * @author paddy
 *
 */
public class Types {
	
	public static Object jsNull;
	public static Object jsUndefined;
	
	public static class TypeError extends RuntimeException {
		private static final long serialVersionUID = 4694411268187181961L;
	}

	/*
	 * basic types
	 */
	public static final int TYPE_INVALID   = -1;
	public static final int TYPE_NONE      = 0;
	public static final int TYPE_UNDEFINED = 1;
	public static final int TYPE_NULL      = 2;
	public static final int TYPE_BOOL      = 3;
	public static final int TYPE_BYTE      = 4;
	public static final int TYPE_SHORT     = 5;
	public static final int TYPE_INT       = 6;
	public static final int TYPE_LONG      = 7;
	public static final int TYPE_DOUBLE    = 8;
	public static final int TYPE_STRING    = 9;
	public static final int TYPE_FUNCTION  = 10;
	public static final int TYPE_DATE      = 11;

	public static final int TYPE_OBJECT    = 16;
	public static final int TYPE_SEQUENCE  = 32;
	public static final int TYPE_ARRAY     = 64;
	public static final int TYPE_INTERFACE = 128;
	public static final int TYPE_MAP       = 256;
	
	public static boolean isInterface(int type) {
		return (type & TYPE_INTERFACE) != 0;
	}
	
	public static boolean isSequence(int type) {
		return (type & TYPE_SEQUENCE) != 0;
	}

	public static boolean isArray(int type) {
		return (type & TYPE_ARRAY) != 0;
	}
	
	public static short getClassId(int type) {
		return type2classid(type);
	}
	
	public static int classid2Type(short classid) { return TYPE_INTERFACE | (((int)classid) << 16); }
	public static short type2classid(int type) { return (short)(type >> 16); }
	
	public static int fromJavaType(InterfaceManager interfaceManager, Type javaType) {
		/* parameterised types are only supported for ObjectArray<T> */
		if(javaType instanceof ParameterizedType) {
			ParameterizedType paramType = (ParameterizedType)javaType;
			if(paramType.getRawType() == ObjectArray.class) {
				Type[] typeArgs = paramType.getActualTypeArguments();
				if(typeArgs.length == 1)
					return TYPE_ARRAY | fromJavaType(interfaceManager, typeArgs[0]);
			}
			else if(paramType.getRawType() == HashMap.class) {
				Type[] typeArgs = paramType.getActualTypeArguments();
				if(typeArgs.length == 2) {
					if(typeArgs[0] == String.class)
						return TYPE_MAP | fromJavaType(interfaceManager, typeArgs[1]);
				}
			}
			return TYPE_INVALID;
		}
		/* handle sequence types; mutidimensional types are not supported */
		Class<?> javaClass = (Class<?>)javaType;
		if(javaClass.isArray()) {
			Class<?> componentClass = javaClass.getComponentType();
			if(componentClass.isArray()) throw new IllegalArgumentException("Types.fromJavaType: mutidimensional arrays are not supported");
			return TYPE_SEQUENCE | fromJavaType(interfaceManager, componentClass);
		}
		/* handle array types; mutidimensional types are not supported */
		if(Array.class.isAssignableFrom(javaClass)) {
			int componentType = getArrayComponentType(javaClass);
			if(componentType != TYPE_INVALID)
				componentType |= TYPE_ARRAY;
			return componentType;
		}
		/* handle dictionary types */
		if(Dictionary.class.isAssignableFrom(javaClass)) {
			while(javaClass != Object.class) {
				IDLInterface iface = interfaceManager.getByClass(javaClass);
				if(iface != null) return classid2Type(iface.getId());
				javaClass = javaClass.getSuperclass();
			}
			return TYPE_INVALID;
		}
		/* handle basic types */
		int baseClassIndex = classMap.indexOf(javaClass);
		if(baseClassIndex != -1)
			return baseClassIndex;
		/* handle interface types */
		String canonicalName = javaClass.getCanonicalName();
		IDLInterface iface = interfaceManager.getByName(canonicalName);
		if(iface != null)
			return classid2Type(iface.getId());

		return TYPE_INVALID;
	}
	
	public static int getInterfaceType(InterfaceManager interfaceManager, IDLInterface obj) {
		int result;
		Class<?> class_ = obj.getClass();
		while(class_ != null) {
			result = fromJavaType(interfaceManager, class_);
			if(result != TYPE_INVALID) return result;
			for(Class<?> interface_ : class_.getInterfaces()) {
				result = fromJavaType(interfaceManager, interface_);
				if(result != TYPE_INVALID) return result;
			}
			class_ = class_.getSuperclass();
		}
		return TYPE_INVALID;
	}
	
	public static int getArrayComponentType(Class <?> javaClass) {
		int result = TYPE_INVALID;
		if(ByteArray.class.isAssignableFrom(javaClass))
			result = TYPE_BYTE;
		else if(IntegerArray.class.isAssignableFrom(javaClass))
			result = TYPE_INT;
		else if(LongArray.class.isAssignableFrom(javaClass))
			result = TYPE_LONG;
		else if(DoubleArray.class.isAssignableFrom(javaClass))
			result = TYPE_DOUBLE;
		return result;
	}
	
	public static IDLInterface baseInterface(InterfaceManager interfaceManager, int type) {
		if((type & (TYPE_SEQUENCE | TYPE_MAP | TYPE_ARRAY)) != 0)
			return baseInterface(interfaceManager, type & ~(TYPE_SEQUENCE | TYPE_MAP | TYPE_ARRAY));
		if(isInterface(type))
			return interfaceManager.getById(getClassId(type));
		return null;
	}

	public class Function {}

	/******************
	 * private state
	 ******************/
	private static List<Class<?>> classMap = new ArrayList<Class<?>>(Arrays.asList(new Class<?>[] {
		null,
		Void.TYPE,
		Void.TYPE,
		Boolean.TYPE,
		Byte.TYPE,
		Short.TYPE,
		Integer.TYPE,
		Long.TYPE,
		Double.TYPE,
		String.class,
		Function.class,
		Date.class,
		null,
		null,
		null,
		null,
		Object.class,
		null,
		null,
		Boolean.class,
		Byte.class,
		Short.class,
		Integer.class,
		Long.class,
		Double.class,
	}));
}
