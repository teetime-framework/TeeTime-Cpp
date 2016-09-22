package teetime.stage;

import static org.hamcrest.collection.IsIterableContainingInOrder.contains;
import static org.junit.Assert.assertThat;

import java.util.ArrayList;
import java.util.List;

import org.junit.Test;

import teetime.framework.test.StageTester;

public class MD5BruteforceStageTest {

	@Test
	public void test() {
		MD5BruteforceStage md5BruteforceStage = new MD5BruteforceStage();

		final String[] inputHashes = new String[] {
			"e4da3b7fbbce2345d7772b0674a318d5",
			"c0c7c76d30bd3dcaefc96f40275bdc0a",
			"a9b7ba70783b617e9998dc4dd82eb3c5",
			"81dc9bdb52d04dc20036dbd8313ed055",
			"fa246d0262c3925617b0c72bb20eeb1d" };
		final Long[] outputValues = new Long[] { 5L, 50L, 1000L, 1234L, 9999L };
		final List<Long> results = new ArrayList<Long>();

		StageTester.test(md5BruteforceStage).and()
				.send(inputHashes).to(md5BruteforceStage.getInputPort()).and()
				.receive(results).from(md5BruteforceStage.getOutputPort())
				.start();

		assertThat(results, contains(outputValues));
	}

}
