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
	public static final String CMD = "cmdline";
	
	public AnodeReceiver() {
		super();
		Log.v(TAG, "*********** AnodeReceiver.init() ***************");
	}

	@Override
	public void onReceive(Context ctx, Intent intent) {
		Runtime runtime;
		String action = intent.getAction();

		try {
			runtime = Runtime.getRuntime(ctx);
		} catch (InitialisationException e) {
			Log.v(TAG, "AnodeReceiver.onReceive::getRuntime: exception: " + e + "; cause: " + e.getCause());
			return;
		}

		/* unconditionally do a stop action */
		if(ACTION_STOP.equals(action)) {
			try {
				runtime.stop();
			} catch (IllegalStateException e) {
				Log.v(TAG, "AnodeReceiver.onReceive::stop: exception: " + e + "; cause: " + e.getCause());
				return;
			} catch (NodeException e) {
				Log.v(TAG, "AnodeReceiver.onReceive::stop: exception: " + e + "; cause: " + e.getCause());
				return;
			}
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

}
