package com.rraptor.pult.comm;

import java.io.IOException;

import android.content.Context;
import android.os.Handler;

public interface DeviceConnection {
	public static final String CMD_X_FORWARD = "go x 1";
	public static final String CMD_X_BACKWARD = "go x -1";
	public static final String CMD_Y_FORWARD = "go y 1";
	public static final String CMD_Y_BACKWARD = "go y -1";
	public static final String CMD_Z_FORWARD = "go z 1";
	public static final String CMD_Z_BACKWARD = "go z -1";
	public static final String CMD_STOP = "stop";

	public static final String CMD_G01 = "g01";

	public static final int REPLY_STATUS_OK = 0;
	public static final int REPLY_STATUS_ERROR = 1;

	public void connectToDevice(String address, int port) throws Exception;

	public void writeToDevice(String cmd) throws IOException;

	public void writeToDeviceBackground(final Context context,
			final Handler handler, final String cmd);

	public int writeToDeviceLocked(String cmd) throws IOException;
}
