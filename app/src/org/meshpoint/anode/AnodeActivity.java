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

package org.meshpoint.anode;

import org.meshpoint.anode.Runtime;
import org.meshpoint.anode.Runtime.IllegalStateException;
import org.meshpoint.anode.Runtime.InitialisationException;
import org.meshpoint.anode.Runtime.NodeException;
import org.meshpoint.anode.Runtime.StateListener;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class AnodeActivity extends Activity implements StateListener {

	private static String TAG = "anode::AnodeActivity";
	private Context ctx;
	private Button startButton;
	private Button stopButton;
	private EditText argsText;
	private TextView stateText;
	private Handler viewHandler = new Handler();
	private long uiThread;
	private String instance;
	private Isolate isolate;

	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        ctx = getApplicationContext();
        initUI();
        uiThread = viewHandler.getLooper().getThread().getId();
    }
    
    private void initUI() {
    	startButton = (Button)findViewById(R.id.start_button);
    	startButton.setOnClickListener(new StartClickListener());
    	stopButton = (Button)findViewById(R.id.stop_button);
    	stopButton.setOnClickListener(new StopClickListener());
    	argsText = (EditText)findViewById(R.id.args_editText);
    	stateText = (TextView)findViewById(R.id.args_stateText);
    	argsText.setOnKeyListener(new OnKeyListener() {
    	    public boolean onKey(View v, int keyCode, KeyEvent event) {
    	        /* If the event is a key-down event on the "enter" button */
    	        if ((event.getAction() == KeyEvent.ACTION_DOWN) &&
    	            (keyCode == KeyEvent.KEYCODE_ENTER)) {
    	          startAction();
    	          return true;
    	        }
    	        return false;
    	    }
    	});
    	__stateChanged(Runtime.STATE_CREATED);
    }
    
    private void initRuntime(String[] opts) {
    	try {
    		Runtime.initRuntime(ctx, opts);
		} catch (InitialisationException e) {
			Log.v(TAG, "initRuntime: exception: " + e + "; cause: " + e.getCause());
		}
    }
    
    private void startAction() {
		String options = getIntent().getStringExtra(AnodeReceiver.OPTS);
		String instance = getIntent().getStringExtra(AnodeReceiver.INST);
		String[] opts = options == null ? null : options.split("\\s");
		initRuntime(opts);
		String args = argsText.getText().toString();
		try {
			isolate = Runtime.createIsolate();
			isolate.addStateListener(this);
			this.instance = AnodeService.addInstance(instance, isolate);
			isolate.start(args.split("\\s"));
		} catch (IllegalStateException e) {
			Log.v(TAG, "isolate start: exception: " + e + "; cause: " + e.getCause());
		} catch (NodeException e) {
			Log.v(TAG, "isolate start: exception: " + e);
		}
    }
    
    private void stopAction() {
    	if(instance == null) {
			Log.v(TAG, "AnodeReceiver.onReceive::stop: no instance currently running for this activity");
			return;
		}
		try {
			isolate.stop();
		} catch (IllegalStateException e) {
			Log.v(TAG, "isolate stop : exception: " + e + "; cause: " + e.getCause());
		} catch (NodeException e) {
			Log.v(TAG, "isolate stop: exception: " + e);
		}
    }
    
    class StartClickListener implements OnClickListener {
		public void onClick(View arg0) {
			startAction();
		}
    }

    class StopClickListener implements OnClickListener {
		public void onClick(View arg0) {
			stopAction();
		}
    }

	@Override
	public void stateChanged(final int state) {
		if(Thread.currentThread().getId() == uiThread) {
			__stateChanged(state);
		} else {
			viewHandler.post(new Runnable() {
				public void run() {
					__stateChanged(state);
				}
			});
		}
	}
	
	private void __stateChanged(final int state) {
		stateText.setText(getStateString(state));
		startButton.setEnabled(state == Runtime.STATE_CREATED);
		stopButton.setEnabled(state == Runtime.STATE_STARTED);
		/* exit the activity if the runtime has exited */
		if(state == Runtime.STATE_STOPPED) {
			AnodeService.removeInstance(instance);
			isolate.removeStateListener(this);
			isolate = null;
			finish();
		}
	}
	
	private String getStateString(int state) {
		Resources res = ctx.getResources();
		String result = null;
		switch(state) {
		case Runtime.STATE_CREATED:
			result = res.getString(R.string.created);
			break;
		case Runtime.STATE_STARTED:
			result = res.getString(R.string.started);
			break;
		case Runtime.STATE_STOPPING:
			result = res.getString(R.string.stopping);
			break;
		case Runtime.STATE_STOPPED:
			result = res.getString(R.string.stopped);
			break;
		}
		return result;
	}
}
