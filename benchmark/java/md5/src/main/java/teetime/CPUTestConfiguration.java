package teetime;

import java.util.List;

import com.google.common.hash.HashCode;

import teetime.framework.Configuration;
import teetime.stage.CollectorSink;
import teetime.stage.IntHashProducerStage;
import teetime.stage.MD5BruteforceStage;
import teetime.stage.basic.distributor.Distributor;
import teetime.stage.basic.distributor.strategy.NonBlockingRoundRobinStrategy;
import teetime.stage.basic.merger.Merger;
import teetime.stage.basic.merger.strategy.BusyWaitingRoundRobinStrategy;

public class CPUTestConfiguration extends Configuration {

	private CollectorSink<Integer> collectorSink;

	public CPUTestConfiguration(final int numThreads, final long numValues, final int value) {
		
		IntHashProducerStage producer = new IntHashProducerStage(numValues, value);
		collectorSink = new CollectorSink<Integer>();
		
		producer.declareActive();	
		
		
		int capacity = 2048;

		Distributor<HashCode> distributor = new Distributor<HashCode>(new NonBlockingRoundRobinStrategy());
		
		connectPorts(producer.getOutputPort(), distributor.getInputPort());		
		
		Merger<Integer> merger = new Merger<Integer>(new BusyWaitingRoundRobinStrategy());			
		merger.declareActive();						

		for (int i = 0; i < numThreads; ++i) {
			MD5BruteforceStage md5bf = new MD5BruteforceStage();
			md5bf.declareActive();
			
			connectPorts(distributor.getNewOutputPort(), md5bf.getInputPort(), capacity);			
			connectPorts(md5bf.getOutputPort(), merger.getNewInputPort(), capacity);
		}

		connectPorts(merger.getOutputPort(), collectorSink.getInputPort(), capacity);
	}

	public List<Integer> getCollectedElements() {
		return collectorSink.getElements();
	}
}
