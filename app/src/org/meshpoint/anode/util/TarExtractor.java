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

public class TarExtractor {

	private File src, dest;

	TarExtractor(File src, File dest) {
		this.src = src;
		this.dest = dest;
	}

	public void extract() throws IOException {
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

			FileOutputStream fos = new FileOutputStream(new File(dest, entry.getName()));
			BufferedOutputStream dest = new BufferedOutputStream(fos);
			while((count = tis.read(buf)) != -1)
				dest.write(buf, 0, count);

			dest.flush();
			dest.close();
		}
		tis.close();		

		/* delete the tar file */
		tarFile.delete();
	}

}
