package teetime;


import java.io.File;
import java.net.URL;
import java.net.URLClassLoader;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.concurrent.TimeUnit;

import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.BenchmarkMode;
import org.openjdk.jmh.annotations.Mode;
import org.openjdk.jmh.annotations.OutputTimeUnit;
import org.openjdk.jmh.annotations.Scope;
import org.openjdk.jmh.annotations.State;
import org.openjdk.jmh.runner.Runner;
import org.openjdk.jmh.runner.RunnerException;
import org.openjdk.jmh.runner.options.Options;
import org.openjdk.jmh.runner.options.OptionsBuilder;

import com.google.common.hash.HashCode;

import teetime.stage.MD5BruteforceStage;


public class Md5Benchmark {
	
	@State(Scope.Thread)
	public static class MyState {
        public HashCode a = MD5BruteforceStage.getMD5(0);
        public HashCode b = MD5BruteforceStage.getMD5(10);   
        public MessageDigest md;        
        
        MyState()
        {
    		try {
    			this.md = MessageDigest.getInstance("MD5");
    		} catch (NoSuchAlgorithmException e) {
    			e.printStackTrace();
    		}
        }
    }
	
	@Benchmark
	@BenchmarkMode(Mode.AverageTime)
	@OutputTimeUnit(TimeUnit.NANOSECONDS)
    public int benchmarkRuntimeOverhead(MyState state) {
        return MD5BruteforceStage.bruteforce(state.a);
    }
	
	@Benchmark
	@BenchmarkMode(Mode.AverageTime)
	@OutputTimeUnit(TimeUnit.NANOSECONDS)
    public long benchmarkRuntimeOverhead2(MyState state) {
        return MD5BruteforceStage.bruteforce(state.b);
    }	
	
	public static void main(String[] args) throws RunnerException {
		URLClassLoader classLoader = (URLClassLoader) Md5Benchmark.class.getClassLoader();
		StringBuilder classpath = new StringBuilder();
		for(URL url : classLoader.getURLs())
		    classpath.append(url.getPath()).append(File.pathSeparator);
		System.setProperty("java.class.path", classpath.toString());
		
		
		Options options = new OptionsBuilder()
				.include(Md5Benchmark.class.getSimpleName())
				.forks(1)
				.build();

		new Runner(options).run();
	}

}
