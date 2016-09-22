package teetime;

import com.beust.jcommander.Parameter;

public class CommandLineArguments {
	
	@Parameter(names = { "-w", "--warmup" }, description = "Number of required warm-up executions", required = true)
	private int warmupExecutions;
	
	@Parameter(names = { "-r", "--real" }, description = "Number of real executions", required = true)
	private int realExecutions;

	@Parameter(names = { "-p", "--pass" }, description = "Current pass number", required = true)
	private int pass;

	@Parameter(names = { "-d", "--dir" }, description = "Working directory", required = false)
	private String workingDirectory = ".";

	@Parameter(names = { "-n", "--num" }, description = "Number of generated Elements", required = true)
	private long numOfElements;

	@Parameter(names = { "-v", "--value" }, description = "int value to be hashed", required = true)
	private int valueOfElements;
	
	@Parameter(names = { "-t", "--threads" }, description = "number of threads", required = true)
	private int numOfThreads;	
	
	@Parameter(names = { "-h", "--help" }, help = true)
	private boolean help;

	public int getWarmupExecutions() {
		return warmupExecutions;
	}

	public int getRealExecutions() {
		return realExecutions;
	}

	public int getPass() {
		return pass;
	}

	public String getWorkingDirectory() {
		return workingDirectory;
	}

	public long getNumOfElements() {
		return numOfElements;
	}
	
	public int getValueOfElements() {
		return valueOfElements;
	}

	public boolean isHelp() {
		return help;
	}
	
	public int getNumOfThreads() {
		return numOfThreads;
	}
}
