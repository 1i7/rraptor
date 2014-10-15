package com.rraptor.pult;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import org.kabeja.parser.ParseException;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
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
import com.rraptor.pult.core.DeviceControlService;
import com.rraptor.pult.core.DeviceControlService.CommandListener;
import com.rraptor.pult.dxf.DxfLoader;
import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.yad.YaDiskActivity;

import edu.android.openfiledialog.OpenFileDialog;

/**
 * 
 */
public class Plotter2DActivity extends RRActivity {
    private Plotter2DCanvasView plotterCanvas;

    private static int REQUEST_CODE_PICK_FILE = 0;

    private final Handler handler = new Handler();

    private ProgressBar drawingProgress;
    private Button btnZUp;
    private Button btnZDown;
    private Button btnStartDrawing;
    private Button btnStopDrawing;
    private Button btnClearDrawing;
    private Button btnOpenFileYaDisk;
    private Button btnOpenFileSdcard;

    private final BroadcastReceiver deviceBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(final Context context, final Intent intent) {
            if (DeviceControlService.ACTION_DEVICE_START_DRAWING.equals(intent
                    .getAction())) {
                onDeviceStartDrawing();
            } else if (DeviceControlService.ACTION_DEVICE_FINISH_DRAWING
                    .equals(intent.getAction())) {
                onDeviceFinishDrawing();
            } else if (DeviceControlService.ACTION_DEVICE_DRAWING_UPDATE
                    .equals(intent.getAction())) {
                final Line2D line = (Line2D) intent
                        .getSerializableExtra(DeviceControlService.EXTRA_LINE);
                final LineDrawingStatus lineStatus = (LineDrawingStatus) intent
                        .getSerializableExtra(DeviceControlService.EXTRA_LINE_STATUS);
                onDeviceDrawingUpdate(line, lineStatus);
            } else if (DeviceControlService.ACTION_DEVICE_DRAWING_ERROR
                    .equals(intent.getAction())) {
                final Exception e = (Exception) intent
                        .getSerializableExtra(DeviceControlService.EXTRA_EXCEPTION);
                onDeviceDrawingError(e);
            }
        }
    };

    private final CommandListener devCommandListener = new CommandListener() {

        @Override
        public void onCommandCanceled(final String cmd) {
            // TODO Auto-generated method stub

        }

        @Override
        public void onCommandExecuted(final String cmd, final String reply) {
            // TODO Auto-generated method stub

        }
    };

    private final OnTouchListener onTouchListener = new OnTouchListener() {

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                switch (v.getId()) {
                case R.id.z_up_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_GO_Z_FORWARD,
                            devCommandListener);
                    break;
                case R.id.z_down_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_GO_Z_BACKWARD,
                            devCommandListener);
                    break;
                }
            } else if (event.getAction() == MotionEvent.ACTION_UP) {
                getDeviceControlService().sendCommand(
                        DeviceConnection.CMD_RR_STOP, devCommandListener);
            }
            return false;
        }
    };

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
        super.initViews();

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
                getDeviceControlService().getDeviceDrawingManager()
                        .startDrawingOnDevice(plotterCanvas.getDrawingLines());
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

        // register broadcast receiver
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_DEVICE_START_DRAWING);
        filter.addAction(DeviceControlService.ACTION_DEVICE_FINISH_DRAWING);
        filter.addAction(DeviceControlService.ACTION_DEVICE_DRAWING_UPDATE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_DRAWING_ERROR);
        registerReceiver(deviceBroadcastReceiver, filter);
    }

    @Override
    protected void onDestroy() {
        unregisterReceiver(deviceBroadcastReceiver);
        super.onDestroy();
    }

    @Override
    protected void onDeviceControlServiceConnected(
            final DeviceControlService service) {
        super.onDeviceControlServiceConnected(service);
        updateViews();
    }

    private void onDeviceDrawingError(final Exception ex) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(Plotter2DActivity.this,
                        "Не получилось нарисовать: " + ex.getMessage(),
                        Toast.LENGTH_LONG).show();
            }
        });
    }

    private void onDeviceDrawingUpdate(final Line2D line,
            final LineDrawingStatus lineStatus) {
        plotterCanvas.setLineStatus(line, lineStatus);
    }

    private void onDeviceFinishDrawing() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                updateViews();
            }
        });
    }

    private void onDeviceStartDrawing() {
        plotterCanvas.resetLineStatus();
        handler.post(new Runnable() {
            @Override
            public void run() {
                updateViews();
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        updateViews();
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

    private void stopDrawingOnDevice() {
        getDeviceControlService().getDeviceDrawingManager()
                .stopDrawingOnDevice();
    }

    private void updateViews() {
        if (getDeviceControlService() != null
                && getDeviceControlService().getDeviceDrawingManager()
                        .isDrawing()) {
            drawingProgress.setVisibility(View.VISIBLE);
            plotterCanvas.setEnabled(false);
            btnClearDrawing.setEnabled(false);
            btnOpenFileYaDisk.setEnabled(false);
            btnOpenFileSdcard.setEnabled(false);
            btnZUp.setEnabled(false);
            btnZDown.setEnabled(false);
            btnStartDrawing.setEnabled(false);
            btnStopDrawing.setEnabled(true);
        } else {
            drawingProgress.setVisibility(View.INVISIBLE);
            plotterCanvas.setEnabled(true);
            btnClearDrawing.setEnabled(true);
            btnOpenFileYaDisk.setEnabled(true);
            btnOpenFileSdcard.setEnabled(true);
            btnZUp.setEnabled(true);
            btnZDown.setEnabled(true);
            btnStartDrawing.setEnabled(true);
            btnStopDrawing.setEnabled(false);
        }
    }
}
