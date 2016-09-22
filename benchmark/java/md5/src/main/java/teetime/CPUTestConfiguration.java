package teetime;

import java.util.LinkedList;
import java.util.List;

import teetime.framework.Configuration;
import teetime.stage.CollectorSink;
import teetime.stage.InitialElementProducer;
import teetime.stage.MD5BruteforceStage;
import teetime.stage.MD5Stage;
import teetime.stage.basic.distributor.Distributor;
import teetime.stage.basic.distributor.strategy.NonBlockingRoundRobinStrategy;
import teetime.stage.basic.merger.Merger;
import teetime.stage.basic.merger.strategy.BusyWaitingRoundRobinStrategy;

public class CPUTestConfiguration extends Configuration {

	private CollectorSink<Long> collectorSink;

	public CPUTestConfiguration(final int numThreads, final long numValues, final int value) {
/*
		InitialElementProducer<String> initialElementProducer = new InitialElementProducer<String>(
				getInputValues(numValues, value));
		MD5Stage md5Stage = new MD5Stage();		
		Distributor<String> distributor = new Distributor<String>();
		Merger<Long> merger = new Merger<Long>();
				
		collectorSink = new CollectorSink<Long>();

		initialElementProducer.declareActive();

		connectPorts(initialElementProducer.getOutputPort(),
				md5Stage.getInputPort());
		
		connectPorts(md5Stage.getOutputPort(),
				distributor.getInputPort());
		
		for(int i=0; i<numThreads; ++i) {
			MD5BruteforceStage md5BruteforceStage = new MD5BruteforceStage();
			
			md5BruteforceStage.getInputPort().getOwningStage().declareActive();
			
			connectPorts(distributor.getNewOutputPort(), md5BruteforceStage.getInputPort());
			connectPorts(md5BruteforceStage.getOutputPort(), merger.getNewInputPort());					
		}				

		merger.declareActive();
		
		
		
				
		connectPorts(merger.getOutputPort(),
				collectorSink.getInputPort());
				
*/
		
		InitialElementProducer<String> producer = new InitialElementProducer<String>(getInputValues(numValues, value));
		MD5Stage md5 = new MD5Stage();
		collectorSink = new CollectorSink<Long>();
		
		producer.declareActive();
		
		connectPorts(producer.getOutputPort(), md5.getInputPort());
		
		int capacity = 2048;

		Distributor<String> distributor = new Distributor<String>(new NonBlockingRoundRobinStrategy());
		connectPorts( md5.getOutputPort(), distributor.getInputPort());
		
		Merger<Long> merger = new Merger<Long>(new BusyWaitingRoundRobinStrategy());			
		merger.declareActive();						

		for (int i = 0; i < numThreads; ++i) {
			MD5BruteforceStage md5bf = new MD5BruteforceStage();
			md5bf.declareActive();
			
			connectPorts(distributor.getNewOutputPort(), md5bf.getInputPort(), capacity);			
			connectPorts(md5bf.getOutputPort(), merger.getNewInputPort(), capacity);
		}

		connectPorts(merger.getOutputPort(), collectorSink.getInputPort(), capacity);
	}



	private List<String> getInputValues(final long numValues, int value) {
		List<String> values = new LinkedList<String>();

		for (long i = 1; i <= numValues; i++) {
			values.add(Long.toString(value));
		}

		return values;
	}

	public List<Long> getCollectedElements() {
		return collectorSink.getElements();
	}
}
