package org.meshpoint.anode.util;

public class AndroidLog implements Log {

	@Override
	public int v(String tag, String msg) {
		return android.util.Log.v(tag, msg);
	}

	@Override
	public int v(String tag, String msg, Throwable tr) {
		return android.util.Log.v(tag, msg, tr);
	}

	@Override
	public int d(String tag, String msg) {
		return android.util.Log.d(tag, msg);
	}

	@Override
	public int d(String tag, String msg, Throwable tr) {
		return android.util.Log.d(tag, msg, tr);
	}

	@Override
	public int i(String tag, String msg) {
		return android.util.Log.i(tag, msg);
	}

	@Override
	public int i(String tag, String msg, Throwable tr) {
		return android.util.Log.i(tag, msg, tr);
	}

	@Override
	public int w(String tag, String msg) {
		return android.util.Log.w(tag, msg);
	}

	@Override
	public int w(String tag, String msg, Throwable tr) {
		return android.util.Log.w(tag, msg, tr);
	}

	@Override
	public int w(String tag, Throwable tr) {
		return android.util.Log.w(tag, tr);
	}

	@Override
	public int e(String tag, String msg) {
		return android.util.Log.e(tag, msg);
	}

	@Override
	public int e(String tag, String msg, Throwable tr) {
		return android.util.Log.e(tag, msg, tr);
	}

	@Override
	public int wtf(String tag, String msg) {
		return android.util.Log.wtf(tag, msg);
	}

	@Override
	public int wtf(String tag, Throwable tr) {
		return android.util.Log.wtf(tag, tr);
	}

	@Override
	public int wtf(String tag, String msg, Throwable tr) {
		return android.util.Log.wtf(tag, msg, tr);
	}

}
