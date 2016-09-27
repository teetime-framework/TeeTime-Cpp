package teetime.stage;

import java.nio.charset.Charset;

import com.google.common.hash.Hasher;
import com.google.common.hash.Hashing;

import teetime.stage.basic.AbstractTransformation;
import teetime.stage.taskfarm.ITaskFarmDuplicable;

public class MD5BruteforceStage extends AbstractTransformation<String, Long>
		implements ITaskFarmDuplicable<String, Long> {

	private final static long MAX_BRUTEFORCE_INPUT = 10000000L;
	private final static long FAILURE_OUTPUT = -1L;
	
	public static long bruteforce(final String inputHash) {
		for (long i = 0; i <= MAX_BRUTEFORCE_INPUT; i++) {
			final String currentHash = getMD5(Long.toString(i));
			if (currentHash.equals(inputHash)) {
				return i;
			}
		}
		
		return FAILURE_OUTPUT;		
	}

	@Override
	protected void execute(String inputHash) {
		this.outputPort.send(bruteforce(inputHash));
	}

	public static String getMD5(String input) {
		Hasher hasher = Hashing.md5().newHasher();
		hasher.putString(input, Charset.forName("UTF-8"));
		return hasher.hash().toString();
	}

	public ITaskFarmDuplicable<String, Long> duplicate() {
		return new MD5BruteforceStage();
	}
}
