package teetime.stage;

import com.google.common.hash.HashCode;
import com.google.common.hash.Hasher;
import com.google.common.hash.Hashing;

import teetime.stage.basic.AbstractTransformation;

public class MD5BruteforceStage extends AbstractTransformation<HashCode, Integer> {

	private final static int MAX_BRUTEFORCE_INPUT = 10000000;
	private final static int FAILURE_OUTPUT = -1;
	
	public static int bruteforce(final HashCode inputHash) {
		for (int i = 0; i <= MAX_BRUTEFORCE_INPUT; i++) {			
			if (getMD5(i).equals(inputHash)) {
				return i;
			}
		}
		
		return FAILURE_OUTPUT;		
	}
	
	public static HashCode getMD5(int i) {
		final Hasher hasher = Hashing.md5().newHasher();
		hasher.putInt(i);
		return hasher.hash();
	}

	@Override
	protected void execute(HashCode inputHash) {
		this.outputPort.send(bruteforce(inputHash));
	}
}
