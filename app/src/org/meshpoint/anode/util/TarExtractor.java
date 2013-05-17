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

import org.kamranzafar.jtar.TarEntry;
import org.kamranzafar.jtar.TarInputStream;

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
		TarInputStream tis = new TarInputStream(new BufferedInputStream(
			new FileInputStream(tarFile)));
		TarEntry entry;
		while ((entry = tis.getNextEntry()) != null) {
			File entryFile = new File(dest, entry.getName());
			File parentDir = new File(entryFile.getParent());

			if (!parentDir.isDirectory() && !parentDir.mkdirs()) {
				tis.close();
				throw new IOException(
					"TarExtractor.unpack(): unable to create directory");
			}

			if (entry.getName().equals("././@LongLink")) {
				// GNU tar format not supported by jtar
				// attempt to recover:
				// LongLink entry provides full file name for following entry
				// -> read file name from body and skip ahead
				String filename = "";
				while ((count = tis.read(buf)) != -1) {
					filename += new String(buf, 0, count - 1);
				}
				entry = tis.getNextEntry();
				entryFile = new File(dest, filename);
				android.util.Log.v("TarExtractor",
					String.format("Encountered long filename." +
						" Moving on to: %s:%s -> %s",
						entry.getName(), entry.isDirectory(), entryFile));
			}
			if (entry.isDirectory()) {
				if (entryFile.isDirectory()) {
					// directory already exists
					android.util.Log.v("TarExtractor",
						"directory for " + entry.getName() + " already exists");
				} else if (entryFile.mkdir()) {
					// directory created (side effect)
					android.util.Log.v("TarExtractor",
						"directory created for " + entry.getName());
				} else {
					// fail
					android.util.Log.w("TarExtractor",
						"failed to create directory for " + entry.getName());
				}
			} else if (!entryFile.isDirectory()) {
				FileOutputStream fos = new FileOutputStream(entryFile);
				BufferedOutputStream bos = new BufferedOutputStream(fos);
				while ((count = tis.read(buf)) != -1) {
					bos.write(buf, 0, count);
				}

				bos.flush();
				bos.close();
			} else {
				android.util.Log.v("TarExtractor",
					String.format("What? %s:%s:%s", entryFile,
						entryFile.isDirectory(), entry.isDirectory())
					);
			}
		}
		tis.close();

		/* delete the tar file */
		tarFile.delete();
	}
}
