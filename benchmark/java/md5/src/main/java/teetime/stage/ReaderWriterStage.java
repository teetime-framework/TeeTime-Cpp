package teetime.stage;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import teetime.stage.basic.AbstractFilter;

public class ReaderWriterStage extends AbstractFilter<Integer> {
	private static final int BUFFER_SIZE = 10 * 1024 * 1024; 
	private byte[]           writeBuffer = new byte[BUFFER_SIZE];
	private byte[]           readBuffer = new byte[BUFFER_SIZE];
	private int              counter     = 0;
	private final String     fileprefix;
	
	private void write(String filename, int num) {
		try {
			FileOutputStream stream = new FileOutputStream(filename);
			
			try {
			    stream.write(writeBuffer, 0, num);
			}  finally {
			    stream.close();
			}
		} catch (IOException e) {			
		}
	}
	
	private int read(String filename) {
		int ret = -1;
		
		try {
			FileInputStream stream = new FileInputStream(filename);
			
			try {
			    ret = stream.read(readBuffer, 0, BUFFER_SIZE);
			}  finally {
			    stream.close();
			}
		} catch (IOException e) {			
		}
		
		return ret;
	}
	
	private void remove(String filename) {
		File f = new File(filename);
		f.delete();
	}
	
	public ReaderWriterStage(String fileprefix) {
		this.fileprefix = fileprefix;
		
		for(int i=0; i<BUFFER_SIZE; ++i) {
			writeBuffer[i] = 88;
		}
	}	

	@Override
	protected void execute(Integer element) {
		String filename = fileprefix + "_" + counter;
		counter++;		
		
		write(filename, element);
		int size = read(filename);		
		remove(filename);
		
		getOutputPort().send(size);
	}
}
