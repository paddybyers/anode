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

package org.meshpoint.anode.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class ZipExtractor implements ModuleUtils.Unpacker {

	public void unpack(File src, File dest) throws IOException {
		int count;
		byte[] buf = new byte[1024];
		ZipInputStream zis = null;
		ZipEntry zipentry;
		zis = new ZipInputStream(new FileInputStream(src));

		while ((zipentry = zis.getNextEntry()) != null) {
			String entryName = zipentry.getName();
			File entryFile = new File(dest, entryName);
			File parentDir = new File(entryFile.getParent());
			if(!parentDir.isDirectory() && !parentDir.mkdirs())
				throw new IOException("ZipExtractor.unpack(): unable to create directory");

			if(zipentry.isDirectory()) {
				if(!entryFile.mkdir())
					throw new IOException("ZipExtractor.unpack(): unable to create directory entry");					
			} else {
				FileOutputStream fos = new FileOutputStream(entryFile);             
				while ((count = zis.read(buf, 0, 1024)) != -1)
					fos.write(buf, 0, count);
	
				fos.close(); 
			}
			zis.closeEntry();
		}
		zis.close();
	}

}
