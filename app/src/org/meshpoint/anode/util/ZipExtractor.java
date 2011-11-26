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

			FileOutputStream fos = new FileOutputStream(entryFile);             
			while ((count = zis.read(buf, 0, 1024)) != -1)
				fos.write(buf, 0, count);

			fos.close(); 
			zis.closeEntry();
		}
		zis.close();
	}

}
