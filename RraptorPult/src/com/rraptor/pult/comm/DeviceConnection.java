package com.rraptor.pult.comm;

import java.io.IOException;

import android.content.Context;
import android.os.Handler;

public interface DeviceConnection {
	public static final String CMD_X_FORWARD = "rr_go x 1";
	public static final String CMD_X_BACKWARD = "rr_go x -1";
	public static final String CMD_Y_FORWARD = "rr_go y 1";
	public static final String CMD_Y_BACKWARD = "rr_go y -1";
	public static final String CMD_Z_FORWARD = "rr_go z 1";
	public static final String CMD_Z_BACKWARD = "rr_go z -1";

	public static final String CMD_X_FORWARD_CAL = "rr_calibrate x 1";
	public static final String CMD_X_BACKWARD_CAL = "rr_calibrate x -1";
	public static final String CMD_Y_FORWARD_CAL = "rr_calibrate y 1";
	public static final String CMD_Y_BACKWARD_CAL = "rr_calibrate y -1";
	public static final String CMD_Z_FORWARD_CAL = "rr_calibrate z 1";
	public static final String CMD_Z_BACKWARD_CAL = "rr_calibrate z -1";

	public static final String CMD_STOP = "rr_stop";

	public static final String CMD_GCODE_G0 = "G0";
	public static final String CMD_GCODE_G01 = "G01";
	public static final String CMD_GCODE_G02 = "G02";
	public static final String CMD_GCODE_G03 = "G03";

	public static final String REPLY_OK = "ok";
	public static final String REPLY_ERROR = "err";
	public static final String REPLY_DONTUNDERSTAND = "dontunderstand";

	public void connectToDevice(String address, int port) throws Exception;

	public void sendToDeviceBackground(final Context context,
			final Handler handler, final String cmd);

	public String sendToDeviceBlocked(String cmd) throws Exception,
			InterruptedException;

	public void unblock();

	public void writeToDevice(String cmd) throws IOException;

	public String writeToDeviceBlocked(String cmd) throws IOException,
			InterruptedException;
}
