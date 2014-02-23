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
import android.widget.Toast;

import com.rraptor.pult.comm.DeviceConnection;
import com.rraptor.pult.comm.DeviceConnection.UnblockHook;
import com.rraptor.pult.comm.DeviceConnectionWifi;
import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;

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
					deviceConnection.sendToDeviceBackground(
							Plotter2DActivity.this, handler,
							DeviceConnection.CMD_Z_FORWARD);
					break;
				case R.id.z_down_btn:
					deviceConnection.sendToDeviceBackground(
							Plotter2DActivity.this, handler,
							DeviceConnection.CMD_Z_BACKWARD);
					break;
				}
			} else if (event.getAction() == MotionEvent.ACTION_UP) {
				deviceConnection.sendToDeviceBackground(Plotter2DActivity.this,
						handler, DeviceConnection.CMD_STOP);
			}
			return false;
		}
	};

	private boolean isDrawing = false;

	private final UnblockHook unblockHook = new UnblockHook();

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
		new Thread(new Runnable() {

			@Override
			public void run() {
				if (isDrawing) {
					return;
				}

				isDrawing = true;

				String cmd;
				int replyStatus;

				final List<Line2D> drawingLines = plotterCanvas
						.getDrawingLines();
				unblockHook.reset();
				try {
					Point2D endPoint = null;
					for (final Line2D line : drawingLines) {
						// swap X and Y for natural view when plot 2d pictures
						// on device table
						if (line.getStart() != endPoint) {
							// move to start point with drawing pen up
							cmd = DeviceConnection.CMD_GCODE_G01 + " "
									+ line.getStart().getY() * 1000 + " "
									+ line.getStart().getX() * 1000 + " 1000";
							replyStatus = deviceConnection.sendToDeviceBlocked(
									cmd, unblockHook);
							if (replyStatus == DeviceConnection.REPLY_STATUS_ERROR) {
								throw new Exception("reply status ERROR");
							}
						}

						// move to end point with drawing pen down
						cmd = DeviceConnection.CMD_GCODE_G01 + " "
								+ line.getEnd().getY() * 1000 + " "
								+ line.getEnd().getX() * 1000 + " 0";
						replyStatus = deviceConnection.sendToDeviceBlocked(cmd,
								unblockHook);
						if (replyStatus == DeviceConnection.REPLY_STATUS_ERROR) {
							throw new Exception("reply status ERROR");
						}

						endPoint = line.getEnd();
					}
				} catch (final Exception e) {
					e.printStackTrace();
					handler.post(new Runnable() {
						@Override
						public void run() {
							Toast.makeText(Plotter2DActivity.this,
									"Failed to draw: " + e.getMessage(),
									Toast.LENGTH_LONG).show();
						}
					});
				}
				isDrawing = false;
			}
		}).start();
	}

	private void stopDrawingOnDevice() {
		unblockHook.unblock();
	}
}
