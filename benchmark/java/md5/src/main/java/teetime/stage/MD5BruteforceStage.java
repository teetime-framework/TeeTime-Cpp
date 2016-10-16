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
	private MessageDigest md;
	
	public MD5BruteforceStage() {
		try {
			this.md = MessageDigest.getInstance("MD5");
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		}
	}
	
	public byte[] getMD5_2(int value) {
		this.md.reset();		
		this.md.update((byte)(value >>> 24));
		this.md.update((byte)(value >>> 16));
		this.md.update((byte)(value >>> 8));
		this.md.update((byte)value);		
		return this.md.digest();
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
		md.reset();
		md.update((byte)value);
		md.update((byte)(value >>> 8));
		md.update((byte)(value >>> 16));
		md.update((byte)(value >>> 24));		
		byte[] b = md.digest();
		
		boolean isEqual = Arrays.equals(a,b);
		System.out.println("equal? " + isEqual);
	}
}
