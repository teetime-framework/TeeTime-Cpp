package teetime;

import java.util.LinkedList;
import java.util.List;

import teetime.framework.Configuration;
import teetime.framework.Execution;
import teetime.stage.CollectorSink;
import teetime.stage.InitialElementProducer;
import teetime.stage.MD5BruteforceStage;
import teetime.stage.MD5Stage;
import teetime.stage.PrinterStage;
import teetime.stage.basic.distributor.Distributor;
import teetime.stage.basic.merger.Merger;

public class WorkerThreadsConfig extends Configuration {

	public WorkerThreadsConfig() {
		InitialElementProducer<String> producer = new InitialElementProducer<String>(getInputValues(100, 12));
		Distributor<String> distributor = new Distributor<String>();
		Merger<Long> merger = new Merger<Long>();
		CollectorSink<Long> sink = new CollectorSink<Long>();

		producer.declareActive();
		MD5Stage md5 = new MD5Stage();
		
		merger.declareActive();
		connectPorts(producer.getOutputPort(), md5.getInputPort());
		connectPorts( md5.getOutputPort(), distributor.getInputPort());

		for (int i = 0; i < 10; ++i) {
			MD5BruteforceStage md5bf = new MD5BruteforceStage();
			connectPorts(distributor.getNewOutputPort(), md5bf.getInputPort());
			md5bf.declareActive();
			
			PrinterStage<Long> printer = new PrinterStage<Long>();			
			
			connectPorts(md5bf.getOutputPort(), printer.getInputPort());
			connectPorts(printer.getOutputPort(), merger.getNewInputPort());
		}

		connectPorts(merger.getOutputPort(), sink.getInputPort());
	}

	public static void main(final String[] args) {
		WorkerThreadsConfig config = new WorkerThreadsConfig();

		Execution<WorkerThreadsConfig> execution = new Execution<WorkerThreadsConfig>(config);
		execution.executeBlocking();
	}

	private List<String> getInputValues(final long numValues, int value) {
		List<String> values = new LinkedList<String>();

		for (long i = 1; i <= numValues; i++) {
			values.add(Long.toString(value));
		}

		return values;
	}
}
