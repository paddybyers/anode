package org.meshpoint.anode.util;

import java.io.PrintStream;

public class PrintStreamLog implements Log {
	
	public PrintStreamLog(PrintStream ps) {
		this.ps = ps;
	}

	@Override
	public int v(String tag, String msg) {
		print(severities[VERBOSE], tag, msg, null);
		return 0;
	}

	@Override
	public int v(String tag, String msg, Throwable tr) {
		print(severities[VERBOSE], tag, msg, tr);
		return 0;
	}

	@Override
	public int d(String tag, String msg) {
		print(severities[DEBUG], tag, msg, null);
		return 0;
	}

	@Override
	public int d(String tag, String msg, Throwable tr) {
		print(severities[DEBUG], tag, msg, tr);
		return 0;
	}

	@Override
	public int i(String tag, String msg) {
		print(severities[INFO], tag, msg, null);
		return 0;
	}

	@Override
	public int i(String tag, String msg, Throwable tr) {
		print(severities[INFO], tag, msg, tr);
		return 0;
	}

	@Override
	public int w(String tag, String msg) {
		print(severities[WARN], tag, msg, null);
		return 0;
	}

	@Override
	public int w(String tag, String msg, Throwable tr) {
		print(severities[WARN], tag, msg, tr);
		return 0;
	}

	@Override
	public int w(String tag, Throwable tr) {
		print(severities[WARN], tag, null, tr);
		return 0;
	}

	@Override
	public int e(String tag, String msg) {
		print(severities[ERROR], tag, msg, null);
		return 0;
	}

	@Override
	public int e(String tag, String msg, Throwable tr) {
		print(severities[ERROR], tag, msg, tr);
		return 0;
	}

	@Override
	public int wtf(String tag, String msg) {
		print(severities[ASSERT], tag, msg, null);
		return 0;
	}

	@Override
	public int wtf(String tag, Throwable tr) {
		print(severities[ASSERT], tag, null, tr);
		return 0;
	}

	@Override
	public int wtf(String tag, String msg, Throwable tr) {
		print(severities[ASSERT], tag, msg, tr);
		return 0;
	}

	private PrintStream ps;
	private String[] severities = new String[]{"", "", "VERBOSE", "DEBUG", "INFO", "WARN", "ERROR", "ASSERT"};
	
	private void print(String severity, String tag, String msg, Throwable tr) {
		ps.print("(" + severity + "): ");
		if(tag != null && !tag.isEmpty())
			ps.print(tag + ": ");
		if(msg != null && !msg.isEmpty())
			ps.print(msg);
		ps.println();
		if(tr != null) {
			tr.printStackTrace(ps);
		}
	}
}
