package com.rraptor.pult;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.List;

import org.kabeja.parser.ParseException;

import android.app.Activity;
import android.content.Intent;
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
import com.rraptor.pult.yad.YaDiskActivity;

import edu.android.openfiledialog.OpenFileDialog;

/**
 * 
 */
public class Plotter2DActivity extends Activity {
	private Plotter2DCanvasView plotterCanvas;

	private static int REQUEST_CODE_PICK_FILE = 0;

	private final Handler handler = new Handler();

	private final DeviceConnection deviceConnection = DeviceConnectionWifi
			.getInstance();

	private ProgressBar drawingProgress;
	private Button btnZUp;
	private Button btnZDown;
	private Button btnStartDrawing;
	private Button btnStopDrawing;
	private Button btnClearDrawing;
	private Button btnOpenFileYaDisk;
	private Button btnOpenFileSdcard;

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

	private void loadDrawingFile(String filePath) {

		try {
			plotterCanvas.setDrawingLines(DxfLoader
					.getLines(new FileInputStream(new File(filePath))));
		} catch (FileNotFoundException e) {
			Toast.makeText(getApplicationContext(),
					"Файл " + filePath + " не найден", Toast.LENGTH_LONG)
					.show();
			e.printStackTrace();
		} catch (ParseException e) {
			Toast.makeText(getApplicationContext(),
					"Не получилось загрузить файл " + filePath,
					Toast.LENGTH_LONG).show();
			e.printStackTrace();
		}
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == REQUEST_CODE_PICK_FILE) {
			if (resultCode == RESULT_OK) {
				Bundle bundle = data.getExtras();
				String filePath = bundle
						.getString(YaDiskActivity.KEY_DOWNLOADED_FILE_PATH);
				loadDrawingFile(filePath);
			}
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

		btnOpenFileYaDisk = (Button) findViewById(R.id.open_file_yadisk_btn);
		btnOpenFileYaDisk.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				openDrawingFileYaDisk();
			}
		});

		btnOpenFileSdcard = (Button) findViewById(R.id.open_file_sdcard_btn);
		btnOpenFileSdcard.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				openDrawingFileSdCard();
			}
		});

		loadDemoDxf();
	}

	@Override
	protected void onResume() {
		super.onResume();
		uptdateViewsState();
	}

	private void openDrawingFileSdCard() {
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
							public void OnSelectedFile(String filePath) {
								Toast.makeText(getApplicationContext(),
										"Загружаем " + filePath,
										Toast.LENGTH_LONG).show();
								loadDrawingFile(filePath);
							}
						});
		fileDialog.show();
	}

	private void openDrawingFileYaDisk() {
		startActivityForResult(new Intent(this, YaDiskActivity.class),
				REQUEST_CODE_PICK_FILE);
	}

	private String sendCommand(final String cmd) throws InterruptedException,
			Exception {
		final String replyStatus = deviceConnection.sendToDeviceBlocked(cmd);
		return replyStatus;
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
				String replyStatus;

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
						if (!line.getStart().equals(endPoint)) {
							// поднимем блок
							cmd = DeviceConnection.CMD_GCODE_G0 + " " + "Z" + 5;

							replyStatus = sendCommand(cmd);
							if (!DeviceConnection.REPLY_OK.equals(replyStatus)) {
								throw new Exception("reply status ERROR");
							}

							// переместимся в начальную точку
							cmd = DeviceConnection.CMD_GCODE_G0 + " " + "X"
									+ line.getStart().getY() + " " + "Y"
									+ line.getStart().getX();

							replyStatus = sendCommand(cmd);
							if (!DeviceConnection.REPLY_OK.equals(replyStatus)) {
								throw new Exception("reply status ERROR");
							}
						}

						// опустим блок на уровень для рисования
						cmd = DeviceConnection.CMD_GCODE_G01 + " " + "Z" + 0
								+ " F10";
						replyStatus = sendCommand(cmd);
						if (!DeviceConnection.REPLY_OK.equals(replyStatus)) {
							throw new Exception("reply status ERROR");
						}

						// прочертим линию со скоростью 2мм/с
						cmd = DeviceConnection.CMD_GCODE_G01 + " " + "X"
								+ line.getEnd().getY() + " " + "Y"
								+ line.getEnd().getX() + " F2";

						replyStatus = sendCommand(cmd);
						plotterCanvas.setLineStatus(line,
								LineDrawingStatus.DRAWN);
						if (!DeviceConnection.REPLY_OK.equals(replyStatus)) {
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
							Toast.makeText(
									Plotter2DActivity.this,
									"Не получилось нарисовать: "
											+ e.getMessage(), Toast.LENGTH_LONG)
									.show();
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
			btnOpenFileYaDisk.setEnabled(false);
			btnZUp.setEnabled(false);
			btnZDown.setEnabled(false);
			btnStartDrawing.setEnabled(false);
			btnStopDrawing.setEnabled(true);
		} else {
			drawingProgress.setVisibility(View.INVISIBLE);
			plotterCanvas.setEnabled(true);
			btnClearDrawing.setEnabled(true);
			btnOpenFileYaDisk.setEnabled(true);
			btnZUp.setEnabled(true);
			btnZDown.setEnabled(true);
			btnStartDrawing.setEnabled(true);
			btnStopDrawing.setEnabled(false);
		}
	}
}
