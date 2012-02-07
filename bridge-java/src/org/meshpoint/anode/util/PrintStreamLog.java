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
		if(tag != null && tag.length() != 0)
			ps.print(tag + ": ");
		if(msg != null && msg.length() != 0)
			ps.print(msg);
		ps.println();
		if(tr != null) {
			tr.printStackTrace(ps);
		}
	}
}
