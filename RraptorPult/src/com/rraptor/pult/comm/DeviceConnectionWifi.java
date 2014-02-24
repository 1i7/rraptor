package com.rraptor.pult.comm;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;

import android.content.Context;
import android.os.Handler;
import android.widget.Toast;

public class DeviceConnectionWifi implements DeviceConnection {
	private static class SingletonHolder {
		private static DeviceConnectionWifi instance = new DeviceConnectionWifi();
	}

	public static final String ADDRESS_DEFAULT = "192.168.43.191";

	public static final int PORT_DEFAULT = 44300;

	public static DeviceConnectionWifi getInstance() {
		return SingletonHolder.instance;
	}

	private Socket socket;

	private OutputStreamWriter out;
	private InputStreamReader in;

	private boolean wait = true;

	private DeviceConnectionWifi() {
	}

	@Override
	public void connectToDevice(String address, int port) throws Exception {
		socket = new Socket(address, port);
		out = new OutputStreamWriter(socket.getOutputStream());
		in = new InputStreamReader(socket.getInputStream());
	}

	public boolean isWaitingBlock() {
		return wait;
	}

	public void reset() {
		wait = true;
	}

	synchronized public void sendToDevice(final Context context,
			final Handler handler, final String address, final int port,
			final String cmd) {
		boolean connected = false;
		if (out != null) {
			try {
				System.out.println(cmd);
				writeToDevice(cmd);
				connected = true;
			} catch (IOException e) {
				System.out.println("Failed connection, try reconnect.");
			}
		}
		if (!connected) {
			try {
				connectToDevice(address, port);
				writeToDevice(cmd);
			} catch (Exception e) {
				e.printStackTrace();
				handler.post(new Runnable() {
					@Override
					public void run() {
						Toast.makeText(context, "Failed connection.",
								Toast.LENGTH_LONG).show();
					}
				});
			}
		}
	}

	@Override
	public void sendToDeviceBackground(final Context context,
			final Handler handler, final String cmd) {
		sendToDeviceBackground(context, handler, ADDRESS_DEFAULT, PORT_DEFAULT,
				cmd);
	}

	public void sendToDeviceBackground(final Context context,
			final Handler handler, final String address, final int port,
			final String cmd) {
		new Thread(new Runnable() {

			@Override
			public void run() {
				sendToDevice(context, handler, address, port, cmd);
			}
		}).start();
	}

	@Override
	public int sendToDeviceBlocked(String cmd) throws Exception {
		return sendToDeviceBlocked(ADDRESS_DEFAULT, PORT_DEFAULT, cmd);
	}

	public int sendToDeviceBlocked(final String address, final int port,
			String cmd) throws Exception, InterruptedException {
		int returnCode = -1;

		boolean connected = false;
		if (out != null) {
			try {
				System.out.println(cmd);
				returnCode = writeToDeviceBlocked(cmd);
				connected = true;
			} catch (IOException e) {
				// Only catch IOException for the 2nd trial,
				// InterruptedException will just go up
				System.out.println("Failed connection, try reconnect.");
			}
		}
		if (!connected) {
			connectToDevice(address, port);
			returnCode = writeToDeviceBlocked(cmd);
		}
		return returnCode;
	}

	@Override
	public void unblock() {
		wait = false;
	}

	@Override
	synchronized public void writeToDevice(String cmd) throws IOException {
		out.write(cmd);
		out.flush();
	}

	@Override
	synchronized public int writeToDeviceBlocked(String cmd)
			throws IOException, InterruptedException {
		out.write(cmd);
		out.flush();

		reset();

		// wait for reply
		while (!in.ready() && isWaitingBlock()) {
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
			}
		}

		int replyStatus;
		if (in.ready()) {
			replyStatus = in.read();
			System.out.println("CMD status: " + replyStatus);
		} else {
			throw new InterruptedException("Прервано");
		}
		return replyStatus;
	}
}
