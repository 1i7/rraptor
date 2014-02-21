package com.rraptor.pult;

import java.util.List;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.Button;

import com.rraptor.pult.comm.DeviceConnection;
import com.rraptor.pult.comm.DeviceConnectionWifi;
import com.rraptor.pult.model.Line2D;

/**
 * 
 */
public class Plotter2DActivity extends Activity {
	private Plotter2DCanvasView plotterCanvas;

	private final Handler handler = new Handler();

	private final DeviceConnection deviceConnection = DeviceConnectionWifi
			.getInstance();

	private final OnTouchListener onTouchListener = new OnTouchListener() {

		@Override
		public boolean onTouch(View v, MotionEvent event) {
			if (event.getAction() == MotionEvent.ACTION_DOWN) {
				switch (v.getId()) {
				case R.id.z_up_btn:
					deviceConnection.writeToDeviceBackground(
							Plotter2DActivity.this, handler,
							DeviceConnection.CMD_Z_FORWARD);
					break;
				case R.id.z_down_btn:
					deviceConnection.writeToDeviceBackground(
							Plotter2DActivity.this, handler,
							DeviceConnection.CMD_Z_BACKWARD);
					break;
				}
			} else if (event.getAction() == MotionEvent.ACTION_UP) {
				deviceConnection.writeToDeviceBackground(
						Plotter2DActivity.this, handler,
						DeviceConnection.CMD_STOP);
			}
			return false;
		}
	};

	private void clearDrawing() {
		plotterCanvas.clearDrawing();
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_plotter_2d);

		plotterCanvas = (Plotter2DCanvasView) findViewById(R.id.plotter_canvas);

		final Button btnZUp = (Button) findViewById(R.id.z_up_btn);
		btnZUp.setOnTouchListener(onTouchListener);
		final Button btnZDown = (Button) findViewById(R.id.z_down_btn);
		btnZDown.setOnTouchListener(onTouchListener);

		final Button btnStartDrawing = (Button) findViewById(R.id.draw_btn);
		btnStartDrawing.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				startDrawingOnDevice();
			}
		});

		final Button btnStopDrawing = (Button) findViewById(R.id.stop_btn);
		btnStopDrawing.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				stopDrawingOnDevice();
			}
		});

		final Button btnClearDrawing = (Button) findViewById(R.id.clear_btn);
		btnClearDrawing.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				clearDrawing();
			}
		});

	}

	private void startDrawingOnDevice() {
		final List<Line2D> drawingLines = plotterCanvas.getDrawingLines();

		for (final Line2D line : drawingLines) {
			String cmd = DeviceConnection.CMD_G01 + " "
					+ line.getStart().getX() + " " + line.getStart().getY();
			deviceConnection.writeToDeviceBackground(this, handler, cmd);
			cmd = DeviceConnection.CMD_G01 + " " + line.getEnd().getX() + " "
					+ line.getEnd().getY();
			deviceConnection.writeToDeviceBackground(this, handler, cmd);
		}
	}

	private void stopDrawingOnDevice() {
		deviceConnection.writeToDeviceBackground(Plotter2DActivity.this,
				handler, DeviceConnection.CMD_STOP);
	}
}
