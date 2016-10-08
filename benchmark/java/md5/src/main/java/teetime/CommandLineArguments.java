package teetime;

import com.beust.jcommander.Parameter;

public class CommandLineArguments {

	@Parameter(names = { "--file" }, description = "gnuplot data file", required = false)
	public String datafile;	
	
	@Parameter(names = { "-w", "--warmup" }, description = "Number of required warm-up executions", required = true)
	public int warmupExecutions;
	
	@Parameter(names = { "-r", "--real" }, description = "Number of real executions", required = true)
	public int realExecutions;

	@Parameter(names = { "-d", "--dir" }, description = "Working directory", required = false)
	public String workingDirectory = ".";

	@Parameter(names = { "-n", "--num" }, description = "Number of generated Elements", required = true)
	public long numOfElements;

	@Parameter(names = { "-v", "--value" }, description = "int value to be hashed", required = true)
	public int valueOfElements;
	
	@Parameter(names = { "-t", "--threads" }, description = "number of threads", required = true)
	public int numOfThreads;	
	
	@Parameter(names = { "-h", "--help" }, help = true)
	public boolean help;
	
	@Parameter(names = { "--fine" }, required = false)
	public boolean fine;
	
	@Parameter(names = { "--medium-fine" }, required = false)
	public boolean medium_fine;	
	
	@Parameter(names = { "--medium" }, required = false)
	public boolean medium;	
	
	@Parameter(names = { "--coarse" }, required = false)
	public boolean coarse;	
}
