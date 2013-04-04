package org.meshpoint.anode.bridge;

import android.content.Context;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import dalvik.system.DexClassLoader;

public class ModuleClassLoader extends ClassLoader {
	private static String packageName = "org.webinos.modules";

	private ClassLoader defaultClassLoader;
	private DexClassLoader internalClassLoader;

	public ModuleClassLoader(Object envContext) {
		if (!(envContext instanceof Context)) {
			throw new IllegalArgumentException();
		}
		Context androidContext = (Context) envContext;
		defaultClassLoader = androidContext.getClassLoader();
		PackageManager pkgMgr = androidContext.getPackageManager();

		if (pkgMgr != null
				&& pkgMgr.checkSignatures(androidContext.getPackageName(), packageName) >= 0) {
			String dexPath = null;
			try {
				dexPath = pkgMgr.getApplicationInfo(packageName, 0).sourceDir;
			} catch (NameNotFoundException e) {}
			if (dexPath != null) {
				String dexOutputDir = androidContext.getDir("dex", 0).getAbsolutePath();
				internalClassLoader = new DexClassLoader(dexPath, dexOutputDir, null,
						androidContext.getClassLoader());
			}
		}
	}

	@Override
	public Class<?> findClass(String className)  throws ClassNotFoundException {
		Class<?> clazz;
		try {
			if((clazz = defaultClassLoader.loadClass(className)) != null)
				return clazz;
		} catch(ClassNotFoundException cnfe) {}

		if (internalClassLoader != null && (clazz = internalClassLoader.loadClass(className)) != null)
			return clazz;

		throw new ClassNotFoundException(className);
	}
}
