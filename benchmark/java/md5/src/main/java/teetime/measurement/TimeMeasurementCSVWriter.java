package teetime.measurement;

import java.io.IOException;
import java.io.Writer;
import java.util.LinkedList;

import teetime.stage.taskfarm.monitoring.PipeMonitoringService;
import teetime.stage.taskfarm.monitoring.SingleTaskFarmMonitoringService;
import teetime.stage.taskfarm.monitoring.extraction.AbstractMonitoringDataExporter;

public class TimeMeasurementCSVWriter extends AbstractMonitoringDataExporter {
	
	private final LinkedList<Long> timeMeasurements = new LinkedList<Long>();
	private final double throughputBoundary;

	public TimeMeasurementCSVWriter(PipeMonitoringService pipeMonitoringService, SingleTaskFarmMonitoringService taskFarmMonitoringService, double throughputBoundary) {
		super(pipeMonitoringService, taskFarmMonitoringService);
		this.throughputBoundary = throughputBoundary;
	}

	@Override
	protected void extractToWriter(Writer writer) {
		try {
			addCSVLineToWriter(writer, "throughputscore", "timeofrun");
			
			for (Long measurement : this.timeMeasurements) {
				addCSVLineToWriter(writer, Double.toString(this.throughputBoundary), measurement.toString());
			}
		} catch (IOException e) {
			throw new IllegalStateException(e);
		}
	}

	public void addTimeMeasurement(long measurement) {
		this.timeMeasurements.add(measurement);
	}
	
	public double avgTime() {
		double avg = 0;
		for(Long t : timeMeasurements)
		{
			avg += t;
		}
		return avg/timeMeasurements.size();
	}
}
