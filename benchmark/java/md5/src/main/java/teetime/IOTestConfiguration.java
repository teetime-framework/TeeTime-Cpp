package teetime;

import java.util.List;

import teetime.framework.Configuration;
import teetime.stage.CollectorSink;
import teetime.stage.InitialElementProducer;
import teetime.stage.ReaderWriterStage;
import teetime.stage.basic.distributor.Distributor;
import teetime.stage.basic.distributor.strategy.NonBlockingRoundRobinStrategy;
import teetime.stage.basic.merger.Merger;
import teetime.stage.basic.merger.strategy.BusyWaitingRoundRobinStrategy;

public class IOTestConfiguration extends Configuration {

	private CollectorSink<Integer> collectorSink;

	public IOTestConfiguration(final int numThreads, final List<Integer> sizes) {
		
		InitialElementProducer<Integer> producer = new InitialElementProducer<Integer>(sizes);
		collectorSink = new CollectorSink<Integer>();
		
		producer.declareActive();		
		
		int capacity = 4096;

		Distributor<Integer> distributor = new Distributor<Integer>(new NonBlockingRoundRobinStrategy());
		
		connectPorts(producer.getOutputPort(), distributor.getInputPort());		
		
		Merger<Integer> merger = new Merger<Integer>(new BusyWaitingRoundRobinStrategy());			
		merger.declareActive();						

		for (int i = 0; i < numThreads; ++i) {
			ReaderWriterStage writerReaderStage = new ReaderWriterStage("writer_" + i);
			writerReaderStage.declareActive();
			
			connectPorts(distributor.getNewOutputPort(), writerReaderStage.getInputPort(), capacity);			
			connectPorts(writerReaderStage.getOutputPort(), merger.getNewInputPort(), capacity);
		}

		connectPorts(merger.getOutputPort(), collectorSink.getInputPort(), capacity);
	}

	public List<Integer> getCollectedElements() {
		return collectorSink.getElements();
	}
}
