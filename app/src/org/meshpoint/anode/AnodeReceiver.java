package org.meshpoint.anode;

import org.meshpoint.anode.Runtime.IllegalStateException;
import org.meshpoint.anode.Runtime.InitialisationException;
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
	public static final String CMD = "cmdline";
	public static final String INST = "instance";
	public static final String OPTS = "options";
	
	public AnodeReceiver() {
		super();
	}

	@Override
	public void onReceive(Context ctx, Intent intent) {
		/* get the system options */
		String options = intent.getStringExtra(OPTS);
		String[] opts = options == null ? null : options.split("\\s");

		try {
			Runtime.initRuntime(ctx, opts);
		} catch (InitialisationException e) {
			Log.v(TAG, "AnodeReceiver.onReceive::getRuntime: exception: " + e + "; cause: " + e.getCause());
			return;
		}

		/* unconditionally do any stop action */
		String action = intent.getAction();
		if(ACTION_STOPALL.equals(action)) {
			for(Isolate isolate : AnodeService.getAll())
				stopInstance(isolate);
			return;
		}
		if(ACTION_STOP.equals(action)) {
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
			return;
		}

		/* get the launch commandline */
		String args = intent.getStringExtra(CMD);
		
		/* if no cmdline was sent, then launch the activity for interactive behaviour */
		if(args == null || args.isEmpty()) {
			intent.setClassName(ctx, AnodeActivity.class.getName());
			intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			ctx.startActivity(intent);
			return;
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
