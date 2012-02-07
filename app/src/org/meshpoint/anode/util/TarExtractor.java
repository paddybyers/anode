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

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.GZIPInputStream;

import org.xeustechnologies.jtar.TarEntry;
import org.xeustechnologies.jtar.TarInputStream;

public class TarExtractor implements ModuleUtils.Unpacker {

	public void unpack(File src, File dest) throws IOException {
		/* first extract the tar file */
		File tarFile = new File(src.getAbsolutePath() + ".tar");
		byte[] buf = new byte[1024];
		GZIPInputStream zis = null;
		zis = new GZIPInputStream(new FileInputStream(src));
		FileOutputStream tarfos = new FileOutputStream(tarFile);             
		int count;
		while ((count = zis.read(buf, 0, 1024)) != -1)
			tarfos.write(buf, 0, count);
		tarfos.close(); 
		zis.close();

		/* now unpack the tar */
		TarInputStream tis = new TarInputStream(new BufferedInputStream(new FileInputStream(tarFile)));
		TarEntry entry;
		while((entry = tis.getNextEntry()) != null) {
			File entryFile = new File(dest, entry.getName());
			File parentDir = new File(entryFile.getParent());
			if(!parentDir.isDirectory() && !parentDir.mkdirs())
				throw new IOException("TarExtractor.unpack(): unable to create directory");

			FileOutputStream fos = new FileOutputStream(entryFile);
			BufferedOutputStream bos = new BufferedOutputStream(fos);
			while((count = tis.read(buf)) != -1)
				bos.write(buf, 0, count);

			bos.flush();
			bos.close();
		}
		tis.close();		

		/* delete the tar file */
		tarFile.delete();
	}

}
