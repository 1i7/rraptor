package com.rraptor.pult;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.List;

import org.kabeja.parser.ParseException;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.rraptor.pult.Plotter2DCanvasView.LineDrawingStatus;
import com.rraptor.pult.comm.DeviceConnection;
import com.rraptor.pult.comm.DeviceConnectionWifi;
import com.rraptor.pult.dxf.DxfLoader;
import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;

import edu.android.openfiledialog.OpenFileDialog;

/**
 * 
 */
public class Plotter2DActivity extends Activity {
	private Plotter2DCanvasView plotterCanvas;

	private final Handler handler = new Handler();

	private final DeviceConnection deviceConnection = DeviceConnectionWifi
			.getInstance();

	private ProgressBar drawingProgress;
	private Button btnZUp;
	private Button btnZDown;
	private Button btnStartDrawing;
	private Button btnStopDrawing;
	private Button btnClearDrawing;
	private Button btnOpenFile;

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

	private void clearDrawing() {
		plotterCanvas.clearDrawing();
	}

	private void loadDemoDxf() {
		try {
			plotterCanvas.setDrawingLines(DxfLoader.getLines(DxfLoader
					.getTestDxfFile()));
		} catch (ParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	private void loadDrawingFile(String fileName) {

		try {
			plotterCanvas.setDrawingLines(DxfLoader
					.getLines(new FileInputStream(new File(fileName))));
		} catch (FileNotFoundException e) {
			Toast.makeText(getApplicationContext(),
					"Файл " + fileName + " не найден", Toast.LENGTH_LONG)
					.show();
			e.printStackTrace();
		} catch (ParseException e) {
			Toast.makeText(getApplicationContext(),
					"Не получилось загрузить файл " + fileName,
					Toast.LENGTH_LONG).show();
			e.printStackTrace();
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_plotter_2d);

		plotterCanvas = (Plotter2DCanvasView) findViewById(R.id.plotter_canvas);
		drawingProgress = (ProgressBar) findViewById(R.id.drawing_progress);

		btnZUp = (Button) findViewById(R.id.z_up_btn);
		btnZUp.setOnTouchListener(onTouchListener);
		btnZDown = (Button) findViewById(R.id.z_down_btn);
		btnZDown.setOnTouchListener(onTouchListener);

		btnStartDrawing = (Button) findViewById(R.id.draw_btn);
		btnStartDrawing.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				startDrawingOnDevice();
			}
		});

		btnStopDrawing = (Button) findViewById(R.id.stop_btn);
		btnStopDrawing.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				stopDrawingOnDevice();
			}
		});

		btnClearDrawing = (Button) findViewById(R.id.clear_btn);
		btnClearDrawing.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				clearDrawing();
			}
		});

		btnOpenFile = (Button) findViewById(R.id.open_file_btn);
		btnOpenFile.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				openDrawingFile();
			}
		});

		loadDemoDxf();
	}

	@Override
	protected void onResume() {
		super.onResume();
		uptdateViewsState();
	}

	private void openDrawingFile() {
		// http://habrahabr.ru/post/203884/
		// https://github.com/Scogun/Android-OpenFileDialog

		String startDir = Environment.getExternalStorageDirectory().getPath()
				+ File.separator + "rraptor";
		if (!new File(startDir).exists()) {
			startDir = Environment.getExternalStorageDirectory().getPath();
		}

		OpenFileDialog fileDialog = new OpenFileDialog(this, startDir)
				.setFilter(".*\\.dxf").setOpenDialogListener(
						new OpenFileDialog.OpenDialogListener() {
							@Override
							public void OnSelectedFile(String fileName) {
								Toast.makeText(getApplicationContext(),
										"Открываем " + fileName,
										Toast.LENGTH_LONG).show();
								loadDrawingFile(fileName);
							}
						});
		fileDialog.show();
	}

	private void startDrawingOnDevice() {
		new Thread(new Runnable() {

			@Override
			public void run() {
				if (isDrawing) {
					return;
				}

				isDrawing = true;
				plotterCanvas.resetLineStatus();
				handler.post(new Runnable() {
					@Override
					public void run() {
						uptdateViewsState();
					}
				});

				String cmd;
				int replyStatus;

				final List<Line2D> drawingLines = plotterCanvas
						.getDrawingLines();
				Line2D currentLine = null;

				try {
					Point2D endPoint = null;
					for (final Line2D line : drawingLines) {
						currentLine = line;
						plotterCanvas.setLineStatus(line,
								LineDrawingStatus.DRAWING_PROGRESS);
						// swap X and Y for natural view when plot 2d pictures
						// on device table
						if (line.getStart() != endPoint) {
							// move to start point with drawing pen up
							cmd = DeviceConnection.CMD_GCODE_G01 + " "
									+ line.getStart().getY() * 1000 + " "
									+ line.getStart().getX() * 1000 + " 1000";

							replyStatus = deviceConnection
									.sendToDeviceBlocked(cmd);

							if (replyStatus == DeviceConnection.REPLY_STATUS_ERROR) {
								throw new Exception("reply status ERROR");
							}
						}

						// move to end point with drawing pen down
						cmd = DeviceConnection.CMD_GCODE_G01 + " "
								+ line.getEnd().getY() * 1000 + " "
								+ line.getEnd().getX() * 1000 + " 0";
						replyStatus = deviceConnection.sendToDeviceBlocked(cmd);
						plotterCanvas.setLineStatus(line,
								LineDrawingStatus.DRAWN);
						if (replyStatus == DeviceConnection.REPLY_STATUS_ERROR) {
							throw new Exception("reply status ERROR");
						}

						endPoint = line.getEnd();
					}
				} catch (final Exception e) {
					if (currentLine != null) {
						plotterCanvas.setLineStatus(currentLine,
								LineDrawingStatus.DRAWING_ERROR);
					}
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
				handler.post(new Runnable() {
					@Override
					public void run() {
						uptdateViewsState();
					}
				});
			}
		}).start();
	}

	private void stopDrawingOnDevice() {
		deviceConnection.unblock();
	}

	private void uptdateViewsState() {
		if (isDrawing) {
			drawingProgress.setVisibility(View.VISIBLE);
			plotterCanvas.setEnabled(false);
			btnClearDrawing.setEnabled(false);
			btnOpenFile.setEnabled(false);
			btnZUp.setEnabled(false);
			btnZDown.setEnabled(false);
			btnStartDrawing.setEnabled(false);
			btnStopDrawing.setEnabled(true);
		} else {
			drawingProgress.setVisibility(View.INVISIBLE);
			plotterCanvas.setEnabled(true);
			btnClearDrawing.setEnabled(true);
			btnOpenFile.setEnabled(true);
			btnZUp.setEnabled(true);
			btnZDown.setEnabled(true);
			btnStartDrawing.setEnabled(true);
			btnStopDrawing.setEnabled(false);
		}
	}
}
