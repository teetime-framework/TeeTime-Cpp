/**
 * Copyright (C) 2016 Johannes Ohlemacher (http://teetime-framework.github.io)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package teetime.stage;

import com.google.common.hash.HashCode;
import com.google.common.hash.Hasher;
import com.google.common.hash.Hashing;

import teetime.framework.AbstractProducerStage;

public class IntHashProducerStage extends AbstractProducerStage<HashCode> {
	private final long num;
	private final int value;
	
	public IntHashProducerStage(long num, int value) {
		this.num = num;
		this.value = value;
	}
	
	@Override
	protected void execute() {
		for(long i=0; i<num; ++i) {
			final Hasher hasher = Hashing.md5().newHasher();
			hasher.putInt(value);
			this.getOutputPort().send(hasher.hash());
		}
		
		this.terminateStage();
	}
}
