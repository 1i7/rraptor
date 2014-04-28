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

	// public static final String ADDRESS_DEFAULT = "192.168.115.115";
	public static final String ADDRESS_DEFAULT = "192.168.43.115";

	public static final int PORT_DEFAULT = 44115;

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
		System.out.println("Connecting to device...");
		socket = new Socket(address, port);
		out = new OutputStreamWriter(socket.getOutputStream());
		in = new InputStreamReader(socket.getInputStream());
		System.out.println("Connection to device success");
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
	public String sendToDeviceBlocked(String cmd) throws Exception {
		return sendToDeviceBlocked(ADDRESS_DEFAULT, PORT_DEFAULT, cmd);
	}

	public String sendToDeviceBlocked(final String address, final int port,
			String cmd) throws Exception, InterruptedException {
		String replyLine = null;

		boolean connected = false;
		if (out != null) {
			try {
				System.out.println(cmd);
				replyLine = writeToDeviceBlocked(cmd);
				connected = true;
			} catch (IOException e) {
				// Only catch IOException for the 2nd trial,
				// InterruptedException will just go up
				System.out.println("Failed connection, try reconnect.");
			}
		}
		if (!connected) {
			connectToDevice(address, port);
			replyLine = writeToDeviceBlocked(cmd);
		}
		return replyLine;
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
	synchronized public String writeToDeviceBlocked(String cmd)
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

		final char[] readBuffer = new char[256];
		String replyLine;
		final int readSize = in.read(readBuffer);
		if (readSize != -1) {
			replyLine = new String(readBuffer, 0, readSize - 1);
			System.out
					.println("CMD reply: " + replyLine + ", size=" + readSize);
		} else {
			throw new InterruptedException("прервано");
		}

		return replyLine;
	}
}
