package teetime;

import java.security.NoSuchAlgorithmException;
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

	public CPUTestConfiguration(final int numThreads, final List<byte[]> hashes) throws NoSuchAlgorithmException {
		
		IntHashProducerStage producer = new IntHashProducerStage(hashes);
		collectorSink = new CollectorSink<Integer>();
		
		producer.declareActive();	
		
		
		int capacity = 4096;

		Distributor<byte[]> distributor = new Distributor<byte[]>(new NonBlockingRoundRobinStrategy());
		
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
