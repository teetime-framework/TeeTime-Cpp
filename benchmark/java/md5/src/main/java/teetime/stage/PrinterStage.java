package teetime.stage;

import teetime.framework.AbstractConsumerStage;
import teetime.framework.OutputPort;

public class PrinterStage<T> extends AbstractConsumerStage<T> {

	private final OutputPort<T> outputPort;

	public PrinterStage() {
		outputPort = createOutputPort();
	}

	public OutputPort<T> getOutputPort() {
		return this.outputPort;
	}

	@Override
	protected void execute(final T element) {
		System.out.println("value: " + element + ", thread: " + Thread.currentThread().getId());
		outputPort.send(element);
	}
}