/*
 * Copyright 2011-2012 Paddy Byers
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

package org.meshpoint.anode.stub.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

public class DirectoryClassLoader extends ClassLoader {
	
	private HashMap<String, Class<?>> loadedClasses = new HashMap<String, Class<?>>();
	private ArrayList<String> pathEntries = new ArrayList<String>();

	public DirectoryClassLoader(String[] classPath) {
		for(String pathElement : classPath) {
			if(pathElement.isEmpty()) continue;
			if(!(new File(pathElement).exists())) continue;
			if(pathElement.charAt(pathElement.length()-1) != '/')
				pathElement += '/';
			pathEntries.add(pathElement);
		}
	}
	
	public Class<?> loadClass(String className, boolean resolve) throws ClassNotFoundException {
		Class<?> result = null;
		try {
			result = findSystemClass(className);
			if(result != null)
				return result;
		} catch(ClassNotFoundException e) {}

		result = loadedClasses.get(className);
		if(result != null)
			return result;
		
		File classFile = null;
		String qualifiedPath = className.replace('.', '/') + ".class";
		for(String pathElement : pathEntries) {
			File candidate = new File(pathElement + qualifiedPath);
			if(candidate.exists()) {
				classFile = candidate;
				break;
			}
		}
		if(classFile == null)
			throw new ClassNotFoundException("DirectoryClassLoader: unable to find class (" + className + ")");

		Exception e = null;
		int offset = 0, read, len = (int)classFile.length();
		byte[] classData = new byte[len];
		try {
			FileInputStream fis = new FileInputStream(classFile);
			while(offset < len && (read = fis.read(classData, offset, len-offset)) != -1) {
				offset += read;
			}
		} catch(IOException ioe) {e = ioe;}
		if(offset < len)
			throw new ClassNotFoundException("DirectoryClassLoader: error reading class (" + className + ")", e);

		result = defineClass(className, classData, 0, len);
		loadedClasses.put(className, result);
		if(resolve)
			resolveClass(result);
		return result;
	}
}
