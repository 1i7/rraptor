package com.rraptor.pult;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.rraptor.pult.core.DeviceControlService;
import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.view.PlotterAreaView;
import com.rraptor.pult.view.PlotterAreaView.LineDrawingStatus;

public class DrawingProgressActivity extends RRActivity {

    private final BroadcastReceiver deviceBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(final Context context, final Intent intent) {
            if (DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE
                    .equals(intent.getAction())) {
                onDeviceStatusUpdate();
            } else if (DeviceControlService.ACTION_DEVICE_STATUS_CHANGE
                    .equals(intent.getAction())) {
                onDeviceStatusUpdate();
            } else if (DeviceControlService.ACTION_DEVICE_CURRENT_POS_CHANGE
                    .equals(intent.getAction())) {
                onDeviceCurrentPosChange();
            } else if (DeviceControlService.ACTION_DEVICE_START_DRAWING
                    .equals(intent.getAction())) {
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

    private final Handler handler = new Handler();

    private PlotterAreaView plotterCanvas;
    private ProgressBar drawingProgress;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_drawing_progress);
        super.initViews();

        plotterCanvas = (PlotterAreaView) findViewById(R.id.plotter_canvas);
        drawingProgress = (ProgressBar) findViewById(R.id.drawing_progress);

        final Button btnPause = (Button) findViewById(R.id.btn_pause);
        btnPause.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                pauseDrawing();
            }
        });

        final Button btnResume = (Button) findViewById(R.id.btn_resume);
        btnResume.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                resumeDrawing();
            }
        });

        final Button btnStop = (Button) findViewById(R.id.btn_stop);
        btnStop.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                stopDrawing();
            }
        });

        // register broadcast receiver
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_STATUS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_CURRENT_POS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_START_DRAWING);
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
        plotterCanvas.setDrawingLines(getDeviceControlService()
                .getDeviceDrawingManager().getDrawingLines());
        onDeviceCurrentPosChange();
        handler.post(new Runnable() {
            @Override
            public void run() {
                updateViews();
            }
        });
    }

    /**
     * Обновить текущее положение рабочего блока.
     */
    void onDeviceCurrentPosChange() {
        if (getDeviceControlService() != null) {
            plotterCanvas.setWorkingBlockPosition(getDeviceControlService()
                    .getDeviceCurrentPosition());
        }
    }

    private void onDeviceDrawingError(final Exception ex) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(DrawingProgressActivity.this,
                        "Не получилось нарисовать: " + ex.getMessage(),
                        Toast.LENGTH_LONG).show();
                updateViews();
            }
        });
    }

    private void onDeviceDrawingUpdate(final Line2D line,
            final LineDrawingStatus lineStatus) {
        plotterCanvas.postInvalidate();
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
        plotterCanvas.setDrawingLines(getDeviceControlService()
                .getDeviceDrawingManager().getDrawingLines());
        handler.post(new Runnable() {
            @Override
            public void run() {
                updateViews();
            }
        });
    }

    private void onDeviceStatusUpdate() {
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

    /**
     * Приостановить процесс рисования.
     */
    private void pauseDrawing() {
        getDeviceControlService().getDeviceDrawingManager().pauseDrawing();
    }

    /**
     * Возобновить процесс рисования.
     */
    private void resumeDrawing() {
        getDeviceControlService().getDeviceDrawingManager().resumeDrawing();
    }

    /**
     * Остановить процесс рисования.
     */
    private void stopDrawing() {
        getDeviceControlService().getDeviceDrawingManager()
                .stopDrawingOnDevice();
    }

    private void updateViews() {
        if (getDeviceControlService() != null
                && getDeviceControlService().getDeviceDrawingManager()
                        .isDrawing()) {
            drawingProgress.setVisibility(View.VISIBLE);
        } else {
            drawingProgress.setVisibility(View.INVISIBLE);
        }
    }
}
