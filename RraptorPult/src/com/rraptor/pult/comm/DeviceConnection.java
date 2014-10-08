package com.rraptor.pult.comm;

import java.io.IOException;
import java.util.concurrent.TimeoutException;

public interface DeviceConnection {

    public static final String CMD_PING = "ping";

    public static final String CMD_RR_STATUS = "rr_status";
    public static final String CMD_RR_STOP = "rr_stop";

    public static final String CMD_RR_GO_X_FORWARD = "rr_go x 1";
    public static final String CMD_RR_GO_X_BACKWARD = "rr_go x -1";
    public static final String CMD_RR_GO_Y_FORWARD = "rr_go y 1";
    public static final String CMD_RR_GO_Y_BACKWARD = "rr_go y -1";
    public static final String CMD_RR_GO_Z_FORWARD = "rr_go z 1";
    public static final String CMD_RR_GO_Z_BACKWARD = "rr_go z -1";

    public static final String CMD_RR_CALIBRATE_X_FORWARD = "rr_calibrate x 1";
    public static final String CMD_RR_CALIBRATE_X_BACKWARD = "rr_calibrate x -1";
    public static final String CMD_RR_CALIBRATE_Y_FORWARD = "rr_calibrate y 1";
    public static final String CMD_RR_CALIBRATE_Y_BACKWARD = "rr_calibrate y -1";
    public static final String CMD_RR_CALIBRATE_Z_FORWARD = "rr_calibrate z 1";
    public static final String CMD_RR_CALIBRATE_Z_BACKWARD = "rr_calibrate z -1";

    public static final String CMD_GCODE_G0 = "G0";
    public static final String CMD_GCODE_G01 = "G01";
    public static final String CMD_GCODE_G02 = "G02";
    public static final String CMD_GCODE_G03 = "G03";

    public static final String REPLY_OK = "ok";
    public static final String REPLY_DONTUNDERSTAND = "dontunderstand";
    public static final String REPLY_ERROR = "error";
    public static final String REPLY_BUSY = "busy";

    public static final String STATUS_WORKING = "working";
    public static final String STATUS_IDLE = "idle";

    public void connectToDevice(final String address, final int port)
            throws IOException;

    public String readFromDevice() throws IOException, TimeoutException;

    public void writeToDevice(final String cmd) throws IOException,
            InterruptedException;
}
