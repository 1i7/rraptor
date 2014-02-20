package com.rraptor.pult;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;


import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.Toast;

public class RraptorPultActivity extends Activity {
	
	public static final String CMD_X_FORWARD = "go x 1";
	public static final String CMD_X_BACKWARD = "go x -1";
	public static final String CMD_Y_FORWARD = "go y 1";
	public static final String CMD_Y_BACKWARD = "go y -1";
	public static final String CMD_Z_FORWARD = "go z 1";
	public static final String CMD_Z_BACKWARD = "go z -1";
	public static final String CMD_STOP = "stop";
	

	public static final String ADDRESS_DEFAULT = "192.168.43.191";
	public static final int PORT_DEFAULT = 44300;
	
	private Socket socket;
	private OutputStreamWriter out;
	
	private boolean sendingCommand = false;
	private Handler handler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rraptor_pult);
        
        final Button btnXF = (Button)findViewById(R.id.x_forward_btn);
        btnXF.setOnTouchListener(onTouchListener);
        final Button btnXB = (Button)findViewById(R.id.x_backward_btn);
        btnXB.setOnTouchListener(onTouchListener);
        final Button btnYF = (Button)findViewById(R.id.y_forward_btn);
        btnYF.setOnTouchListener(onTouchListener);
        final Button btnYB = (Button)findViewById(R.id.y_backward_btn);
        btnYB.setOnTouchListener(onTouchListener);
        final Button btnZF = (Button)findViewById(R.id.z_forward_btn);
        btnZF.setOnTouchListener(onTouchListener);
        final Button btnZB = (Button)findViewById(R.id.z_backward_btn);
        btnZB.setOnTouchListener(onTouchListener);
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.rraptor_pult, menu);
        return true;
    }
    
    private OnTouchListener onTouchListener = new OnTouchListener() {

		@Override
		public boolean onTouch(View v, MotionEvent event) {
			if (event.getAction() == MotionEvent.ACTION_DOWN) {
				switch (v.getId()) {
				case R.id.x_forward_btn:
					sendToDeviceBackground(CMD_X_FORWARD);
					break;
				case R.id.x_backward_btn:
					sendToDeviceBackground(CMD_X_BACKWARD);
					break;
				case R.id.y_forward_btn:
					sendToDeviceBackground(CMD_Y_FORWARD);
					break;
				case R.id.y_backward_btn:
					sendToDeviceBackground(CMD_Y_BACKWARD);
					break;
				case R.id.z_forward_btn:
					sendToDeviceBackground(CMD_Z_FORWARD);
					break;
				case R.id.z_backward_btn:
					sendToDeviceBackground(CMD_Z_BACKWARD);
					break;
				}
			} else if (event.getAction() == MotionEvent.ACTION_UP) {
				sendToDeviceBackground(CMD_STOP);
			}
			return false;
		}
	};
	
	void sendToDeviceBackground(final String cmd) {
		sendToDeviceBackground(ADDRESS_DEFAULT, PORT_DEFAULT, cmd);
	}
	void sendToDeviceBackground(final String address, final int port,
			final String cmd) {
		new Thread(new Runnable() {

			@Override
			public void run() {
				if (!sendingCommand) {
					sendingCommand = true;
					sendToDevice(address, port, cmd);
					sendingCommand = false;
				}
			}

		}).start();
	}
	
	void sendToDevice(final String address, final int port, final String cmd) {
		boolean connected = false;
		if (out != null) {
			try {
				System.out.println(cmd);
				writeToCar(cmd);
				connected = true;
			} catch (IOException e) {
				System.out.println("Failed connection, try reconnect.");
			}
		}
		if (!connected) {
			try {
				connectToDevice(address, port);
				writeToCar(cmd);
			} catch (Exception e) {
				e.printStackTrace();
				handler.post(new Runnable(){
					@Override
					public void run() {
						Toast.makeText(RraptorPultActivity.this, "Failed connection.", Toast.LENGTH_LONG)
						.show();
					}});				
			}
		}
	}
	
	void writeToCar(String cmd) throws IOException {
		out.write(cmd);
		out.flush();
	}

	void connectToDevice(String address, int port) throws Exception {
		socket = new Socket(address, port);
		out = new OutputStreamWriter(socket.getOutputStream());
	}
}
