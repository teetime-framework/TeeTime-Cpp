package teetime;

import java.io.File;
import java.io.IOException;

import com.beust.jcommander.JCommander;
import com.beust.jcommander.ParameterException;

import teetime.framework.Execution;
import teetime.measurement.TimeMeasurement;
import teetime.measurement.TimeMeasurementCSVWriter;
import teetime.stage.taskfarm.monitoring.PipeMonitoringService;
import teetime.stage.taskfarm.monitoring.SingleTaskFarmMonitoringService;
import teetime.stage.taskfarm.monitoring.extraction.AbstractMonitoringDataExporter;
import teetime.stage.taskfarm.monitoring.extraction.StackedTimePullThroughput2D;
import teetime.stage.taskfarm.monitoring.extraction.StackedTimePushThroughput2D;
import teetime.stage.taskfarm.monitoring.extraction.StackedTimeSizeWithCapacity2D;
import teetime.stage.taskfarm.monitoring.extraction.TimeBoundary2D;
import teetime.stage.taskfarm.monitoring.extraction.TimeBoundaryMSPullThroughput3D;
import teetime.stage.taskfarm.monitoring.extraction.TimeBoundaryMSPushThroughput3D;
import teetime.stage.taskfarm.monitoring.extraction.TimeBoundaryStages3D;

public class CPUTestExecution {

	private static TimeMeasurementCSVWriter measurementWriter;

	public static void main(String[] args) throws IOException {
		CommandLineArguments arguments = parseArguments(args);
		System.out.println("-- Starting CPU test case --");
		System.out.println("Configuration: warmup=" + arguments.getWarmupExecutions() + ", real=" + arguments.getRealExecutions() + ", pass="
				+ arguments.getPass() + ", directory=" + arguments.getWorkingDirectory() + ", num=" + arguments.getNumOfElements() + ", value="
				+ arguments.getValueOfElements() + ", threads=" + arguments.getNumOfThreads());

		CPUTestExecution.measurementWriter = new TimeMeasurementCSVWriter(null, null, 10);

		startEvaluation(arguments);

		CPUTestExecution.measurementWriter.extractToFile(arguments.getWorkingDirectory() + "/timeofruns.dat");

		System.out.println("Test case "
				+ CPUTestExecution.class.getSimpleName()
				+ " has finished.");
	}

	private static void startEvaluation(CommandLineArguments arguments) throws IOException {
		for (int i = 0; i < arguments.getWarmupExecutions(); i++) {
			System.out.println("Warmup #" + i + " started");
			execute(arguments);
		}

		for (int i = 0; i < arguments.getRealExecutions(); i++) {
			System.out.println("Run #" + i + " started");
			CPUTestConfiguration configuration = execute(arguments);						
		}
		
		System.out.println("avg time: " + CPUTestExecution.measurementWriter.avgTime());
	}

	private static CPUTestConfiguration execute(CommandLineArguments arguments) {
		CPUTestConfiguration configuration = new CPUTestConfiguration(arguments.getNumOfThreads(),
				arguments.getNumOfElements(), arguments.getValueOfElements());
		
		Execution<CPUTestConfiguration> execution = new Execution<CPUTestConfiguration>(configuration);

		TimeMeasurement timeMeasurement = new TimeMeasurement();
		timeMeasurement.startMeasurement();
		execution.executeBlocking();
		//System.out.println("executeBlocking done, collected elements: " + execution.getConfiguration().getCollectedElements().size());
		timeMeasurement.endMeasurement();
		CPUTestExecution.measurementWriter.addTimeMeasurement(timeMeasurement.getMeasuredTimespan());

		return configuration;
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

		if (arguments.isHelp()) {
			commander.usage();
			System.exit(0);
		}

		return arguments;
	}
}
