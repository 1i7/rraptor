package com.rraptor.pult.comm;

import java.io.IOException;

import android.content.Context;
import android.os.Handler;

public interface DeviceConnection {
	public class UnblockHook {
		private boolean wait = true;

		public boolean isWaitingBlock() {
			return wait;
		}

		public void reset() {
			wait = true;
		}

		public void unblock() {
			wait = false;
		}
	}

	public static final String CMD_X_FORWARD = "go x 1";
	public static final String CMD_X_BACKWARD = "go x -1";
	public static final String CMD_Y_FORWARD = "go y 1";
	public static final String CMD_Y_BACKWARD = "go y -1";
	public static final String CMD_Z_FORWARD = "go z 1";
	public static final String CMD_Z_BACKWARD = "go z -1";
	public static final String CMD_STOP = "stop";

	public static final String CMD_CALIBRATE = "calibrate";
	public static final String CMD_GCODE_G01 = "G01";
	public static final String CMD_GCODE_G02 = "G02";

	public static final String CMD_GCODE_G03 = "G03";
	public static final int REPLY_STATUS_OK = 0;

	public static final int REPLY_STATUS_ERROR = 1;

	public void connectToDevice(String address, int port) throws Exception;

	public void sendToDeviceBackground(final Context context,
			final Handler handler, final String cmd);

	public int sendToDeviceBlocked(String cmd, UnblockHook unblockHook)
			throws Exception;

	public void writeToDevice(String cmd) throws IOException;

	public int writeToDeviceBlocked(String cmd, UnblockHook unblockHook)
			throws IOException;
}
