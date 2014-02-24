package com.rraptor.pult;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.ImageButton;

import com.rraptor.pult.comm.DeviceConnection;
import com.rraptor.pult.comm.DeviceConnectionWifi;

public class ManualPultActivity extends Activity {

	private final Handler handler = new Handler();

	private final DeviceConnection deviceConnection = DeviceConnectionWifi
			.getInstance();

	private final OnTouchListener onTouchListener = new OnTouchListener() {

		@Override
		public boolean onTouch(View v, MotionEvent event) {
			if (event.getAction() == MotionEvent.ACTION_DOWN) {
				switch (v.getId()) {
				case R.id.x_forward_btn:
					deviceConnection.sendToDeviceBackground(
							ManualPultActivity.this, handler,
							DeviceConnection.CMD_X_FORWARD);
					break;
				case R.id.x_backward_btn:
					deviceConnection.sendToDeviceBackground(
							ManualPultActivity.this, handler,
							DeviceConnection.CMD_X_BACKWARD);
					break;
				case R.id.y_forward_btn:
					deviceConnection.sendToDeviceBackground(
							ManualPultActivity.this, handler,
							DeviceConnection.CMD_Y_FORWARD);
					break;
				case R.id.y_backward_btn:
					deviceConnection.sendToDeviceBackground(
							ManualPultActivity.this, handler,
							DeviceConnection.CMD_Y_BACKWARD);
					break;
				case R.id.z_forward_btn:
					deviceConnection.sendToDeviceBackground(
							ManualPultActivity.this, handler,
							DeviceConnection.CMD_Z_FORWARD);
					break;
				case R.id.z_backward_btn:
					deviceConnection.sendToDeviceBackground(
							ManualPultActivity.this, handler,
							DeviceConnection.CMD_Z_BACKWARD);
					break;
				}
			} else if (event.getAction() == MotionEvent.ACTION_UP) {
				deviceConnection.sendToDeviceBackground(
						ManualPultActivity.this, handler,
						DeviceConnection.CMD_STOP);
			}
			return false;
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_manual_pult);

		final ImageButton btnXF = (ImageButton) findViewById(R.id.x_forward_btn);
		btnXF.setOnTouchListener(onTouchListener);
		final ImageButton btnXB = (ImageButton) findViewById(R.id.x_backward_btn);
		btnXB.setOnTouchListener(onTouchListener);
		final ImageButton btnYF = (ImageButton) findViewById(R.id.y_forward_btn);
		btnYF.setOnTouchListener(onTouchListener);
		final ImageButton btnYB = (ImageButton) findViewById(R.id.y_backward_btn);
		btnYB.setOnTouchListener(onTouchListener);
		final ImageButton btnZF = (ImageButton) findViewById(R.id.z_forward_btn);
		btnZF.setOnTouchListener(onTouchListener);
		final ImageButton btnZB = (ImageButton) findViewById(R.id.z_backward_btn);
		btnZB.setOnTouchListener(onTouchListener);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.rraptor_pult, menu);
		return true;
	}
}
