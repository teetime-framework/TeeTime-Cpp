package teetime.measurement;

public class TimeMeasurement {
	
	private long millisAtStart = 0l;
	private long millisAtEnd = 0l;
	
	public void startMeasurement() {
		millisAtStart = System.currentTimeMillis();
	}
	
	public void endMeasurement() {
		millisAtEnd = System.currentTimeMillis();
	}

	public long getMeasuredTimespan() {
		return millisAtEnd - millisAtStart;
	}
}
