package com.rraptor.pult.comm;

import java.io.IOException;
import java.util.concurrent.TimeoutException;

public interface DeviceConnection {
    public void connectToDevice(final String address, final int port)
            throws IOException;

    public String readFromDevice() throws IOException, TimeoutException;

    public void writeToDevice(final String cmd) throws IOException,
            InterruptedException;
}
