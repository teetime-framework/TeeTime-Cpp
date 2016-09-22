package teetime;


import java.io.IOException;

import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.Setup;
import org.openjdk.jmh.runner.Runner;
import org.openjdk.jmh.runner.RunnerException;
import org.openjdk.jmh.runner.options.Options;
import org.openjdk.jmh.runner.options.OptionsBuilder;

import teetime.stage.MD5BruteforceStage;

public class Md5Benchmark {
	
	private MD5BruteforceStage bfstage;
	private String inputString;
	private Long outputLong;
/*	
	@Setup
	public void init() {
		bfstage = new MD5BruteforceStage();
		inputString = Long.toString(2);				
	}
	
	@Benchmark
    public void benchmarkRuntimeOverhead() {
        //outputLong = MD5BruteforceStage.bruteforce(inputString);
    }
	*/
	
	@Benchmark
	public void foobar() {
		
	}
	
	public static void main(String... args) throws RunnerException, IOException {
		Options options = new OptionsBuilder()
				.include(Md5Benchmark.class.getSimpleName())
				.forks(1)
				.build();

		new Runner(options).run();
	}
}
