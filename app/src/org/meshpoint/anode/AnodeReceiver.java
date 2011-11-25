package org.meshpoint.anode;

import org.meshpoint.anode.Runtime.IllegalStateException;
import org.meshpoint.anode.Runtime.NodeException;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class AnodeReceiver extends BroadcastReceiver {
	
	private static String TAG = "anode::AnodeReceiver";
	public static final String ACTION_START = "org.meshpoint.anode.START";
	public static final String ACTION_STOP = "org.meshpoint.anode.STOP";
	public static final String ACTION_STOPALL = "org.meshpoint.anode.STOPALL";
	public static final String ACTION_INSTALL = "org.meshpoint.anode.INSTALL";
	public static final String ACTION_UNINSTALL = "org.meshpoint.anode.UNINSTALL";
	public static final String CMD = "cmdline";
	public static final String INST = "instance";
	public static final String OPTS = "options";
	public static final String MODULE = "module";
	public static final String PATH = "path";
	
	public AnodeReceiver() {
		super();
	}

	@Override
	public void onReceive(Context ctx, Intent intent) {
		/* get the system options */
		String action = intent.getAction();
		if(ACTION_STOPALL.equals(action)) {
			if(Runtime.isInitialised()) {
				for(Isolate isolate : AnodeService.getAll())
					stopInstance(isolate);
			}
			return;
		}
		if(ACTION_STOP.equals(action)) {
			if(Runtime.isInitialised()) {
				String instance = intent.getStringExtra(INST);
				if(instance == null) {
					instance = AnodeService.soleInstance();
					if(instance == null) {
						Log.v(TAG, "AnodeReceiver.onReceive::stop: no instance specified");
						return;
					}
				}
				Isolate isolate = AnodeService.getInstance(instance);
				if(isolate == null) {
					Log.v(TAG, "AnodeReceiver.onReceive::stop: instance " + instance + " not found");
					return;
				}
				stopInstance(isolate);
			}
			return;
		}
		
		if(ACTION_START.equals(action)) {
			/* get the launch commandline */
			String args = intent.getStringExtra(CMD);
		
			/* if no cmdline was sent, then launch the activity for interactive behaviour */
			if(args == null || args.isEmpty()) {
				intent.setClassName(ctx, AnodeActivity.class.getName());
				intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				ctx.startActivity(intent);
				return;
			}
		}

		/* otherwise, start service */
		intent.setClassName(ctx, AnodeService.class.getName());
		ctx.startService(intent);
	}

	private void stopInstance(Isolate isolate) {
		try {
			isolate.stop();
		} catch (IllegalStateException e) {
			Log.v(TAG, "AnodeReceiver.onReceive::stop: exception: " + e + "; cause: " + e.getCause());
		} catch (NodeException e) {
			Log.v(TAG, "AnodeReceiver.onReceive::stop: exception: " + e + "; cause: " + e.getCause());
		}
	}
}
