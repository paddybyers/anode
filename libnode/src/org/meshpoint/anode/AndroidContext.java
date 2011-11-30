package org.meshpoint.anode;

import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.bridge.ModuleContext;
import org.meshpoint.anode.js.JSObject;

import android.content.Context;

public class AndroidContext extends ModuleContext {
	
	private Context ctx;
	
	AndroidContext(Env env, JSObject exports, Context ctx) {
		super(env, exports);
		this.ctx = ctx;
	}
	
	public Context getAndroidContext() {
		return ctx;
	}

}
