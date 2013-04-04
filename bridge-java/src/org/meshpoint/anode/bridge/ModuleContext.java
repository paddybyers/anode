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

package org.meshpoint.anode.bridge;

import org.meshpoint.anode.js.JSObject;
import org.meshpoint.anode.module.IModule;
import org.meshpoint.anode.module.IModuleContext;
import org.meshpoint.anode.type.IIndexedCollection;

public class ModuleContext implements IModuleContext {
	@SuppressWarnings("unused")
	private Env env;
	private JSObject exports;
	private IModule module;
	private long threadId;

	protected ModuleContext(Env env, JSObject exports) {
		this.env = env;
		this.exports = exports;
		threadId = Thread.currentThread().getId();
	}
	
	void setModule(IModule module) {
		this.module = module;
	}

	@Override
	public IIndexedCollection<Object> getModuleExports() {
		return exports;
	}

	@Override
	public long getEventThreadId() {
		return threadId;
	}

	public IModule getModule() {
		return module;
	}

}
