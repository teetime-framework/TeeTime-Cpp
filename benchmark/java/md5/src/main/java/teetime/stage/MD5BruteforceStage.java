package teetime.stage;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;

import com.google.common.hash.HashCode;
import com.google.common.hash.Hasher;
import com.google.common.hash.Hashing;

import teetime.stage.basic.AbstractTransformation;

public class MD5BruteforceStage extends AbstractTransformation<byte[], Integer> {

	private final MessageDigest messageDigest;
	private final static int MAX_BRUTEFORCE_INPUT = 10000000;
	private final static int FAILURE_OUTPUT = -1;
	
	public MD5BruteforceStage() throws NoSuchAlgorithmException {
		messageDigest = MessageDigest.getInstance("MD5");
	}
		
	public static byte[] getMD5(MessageDigest md, int value) {
		md.reset();
		md.update((byte)value);
		md.update((byte)(value >>> 8));
		md.update((byte)(value >>> 16));
		md.update((byte)(value >>> 24));				
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
	protected void execute(byte[] inputHash) {
		this.outputPort.send(bruteforce(messageDigest, inputHash));
	}
	
	
	private static void check(MessageDigest md, int value) throws NoSuchAlgorithmException {
		byte[] a = getMD5(value).asBytes();
		
		
		byte[] b = getMD5(md, value);	
		
		boolean isEqual = Arrays.equals(a,b);
		System.out.println(Integer.toString(value ) + "is equal? " + isEqual);		
	}
	
	public static void main(String[] args) throws NoSuchAlgorithmException {
		MessageDigest md = MessageDigest.getInstance("MD5");
		
		check(md, 0);
		check(md, 1);
		check(md, 128);
		
		byte[] a = getMD5(md, 0);
		int value = bruteforce(md, a);
		System.out.println("brute force: " + value);
	}
}
