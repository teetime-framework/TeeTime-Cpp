package teetime;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;

import com.beust.jcommander.JCommander;
import com.beust.jcommander.ParameterException;

import teetime.framework.Execution;
import teetime.measurement.TimeMeasurement;

public class IOTestExecution {

	public static void main(String[] args) throws IOException {
		CommandLineArguments arguments = parseArguments(args);
		System.out.println("-- Starting IO test case --");
		System.out.println("Configuration: warmup=" + arguments.warmupExecutions + ", real=" + arguments.realExecutions 
				+ ", directory=" + arguments.workingDirectory + ", num=" + arguments.numOfElements + ", value="
				+ arguments.valueOfElements + ", threads=" + arguments.numOfThreads);

		Double d = startEvaluation(arguments, arguments.numOfThreads);
		
		if(arguments.datafile != null && !arguments.datafile.isEmpty()) {
			System.out.println("append result to " + arguments.datafile + "...");
			
			FileWriter fw = new FileWriter(arguments.datafile, true);
			BufferedWriter bw = new BufferedWriter(fw);
			PrintWriter out = new PrintWriter(bw);
			try
			{
				out.print(Double.toString(d) + " ");
				if(arguments.numOfThreads == 34)
				{
					out.println();
				}
			}
			finally
			{
				out.close();
			}
		};
	}

	private static Double startEvaluation(CommandLineArguments arguments, int numThreads) throws IOException {
		
		List<Integer> sizes = new ArrayList<Integer>();
		for(long i=0; i<arguments.numOfElements; ++i) {
			sizes.add(arguments.valueOfElements);
		}		
		
		for (int i = 0; i < arguments.warmupExecutions; i++) {
			for(int j=0; j<10; ++j)
				gc();			
			System.out.println("Warmup #" + i + " started");
			execute(sizes, numThreads);
		}
		
		List<Long> measurements = new ArrayList<Long>();
		for (int i = 0; i < arguments.realExecutions; i++) {
			for(int j=0; j<10; ++j)
				gc();
			System.out.print("Run #" + i + " started... ");
			Long ms = execute(sizes, numThreads);
			System.out.println("DONE, time: " + ms + "ms");
			measurements.add(ms);
		}
		
		double d = 0;
		for(Long l : measurements) {
			d += l;
		}
		return d/measurements.size();
	}

	private static Long execute(final List<Integer> sizes, int numThreads) {
		IOTestConfiguration configuration = new IOTestConfiguration(numThreads,
				sizes);
		
		Execution<IOTestConfiguration> execution = new Execution<IOTestConfiguration>(configuration);

		TimeMeasurement timeMeasurement = new TimeMeasurement();
		timeMeasurement.startMeasurement();
		execution.executeBlocking();		
		timeMeasurement.endMeasurement();
		
		return timeMeasurement.getMeasuredTimespan();
	}

	private static CommandLineArguments parseArguments(String[] args) {
		CommandLineArguments arguments = new CommandLineArguments();
		JCommander commander = null;
		try {
			commander = new JCommander(arguments, args);
		} catch (ParameterException e) {
			System.out.println(e.getMessage());
			System.out.println("Use -h or --help to get usage information.");
			System.exit(0);
		}

		if (arguments.help) {
			commander.usage();
			System.exit(0);
		}

		return arguments;
	}
	
	public static void gc() {
	     Object obj = new Object();
	     WeakReference<Object> ref = new WeakReference<Object>(obj);
	     obj = null;
	     while(ref.get() != null) {
	       System.gc();
	     }
	   }
}
