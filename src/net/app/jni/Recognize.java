package net.app.jni;

public class Recognize {
	static{
		System.loadLibrary("read");
	}
	public native int[][] recognize(int[][] arrayData);
}
