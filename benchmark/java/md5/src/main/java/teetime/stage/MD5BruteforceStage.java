package teetime.stage;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;

import com.google.common.hash.HashCode;
import com.google.common.hash.Hasher;
import com.google.common.hash.Hashing;

import teetime.stage.basic.AbstractTransformation;

public class MD5BruteforceStage extends AbstractTransformation<HashCode, Integer> {

	private final static int MAX_BRUTEFORCE_INPUT = 10000000;
	private final static int FAILURE_OUTPUT = -1;
		
	public static byte[] getMD5(MessageDigest md, int value) {
		md.reset();		
		md.update((byte)(value >>> 24));
		md.update((byte)(value >>> 16));
		md.update((byte)(value >>> 8));
		md.update((byte)value);		
		return md.digest();
	}
	
	public static int bruteforce(MessageDigest md, byte[] inputHash) {
		for (int i = 0; i <= MAX_BRUTEFORCE_INPUT; i++) {			
			if (Arrays.equals(getMD5(md, i), inputHash)) {
				return i;
			}
		}
		
		return FAILURE_OUTPUT;	
	}
	
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
	
	
	public static void main(String[] args) throws NoSuchAlgorithmException {
		int value = 128;
		
		byte[] a = getMD5(value).asBytes();
		
		MessageDigest md = MessageDigest.getInstance("MD5");
		byte[] b = getMD5(md, value);
		
		boolean isEqual = Arrays.equals(a,b);
		System.out.println("equal? " + isEqual);
	}
}
