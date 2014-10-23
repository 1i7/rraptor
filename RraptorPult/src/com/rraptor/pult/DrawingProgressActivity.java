package com.rraptor.pult;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.rraptor.pult.core.DeviceControlService;
import com.rraptor.pult.core.DeviceControlService.ConnectionStatus;
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
                onDeviceDrawingStatusChange();
            } else if (DeviceControlService.ACTION_DEVICE_FINISH_DRAWING
                    .equals(intent.getAction())) {
                onDeviceDrawingStatusChange();
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
            } else if (DeviceControlService.ACTION_DEVICE_DRAWING_PAUSED
                    .equals(intent.getAction())) {
                onDeviceDrawingStatusChange();
            } else if (DeviceControlService.ACTION_DEVICE_DRAWING_RESUMED
                    .equals(intent.getAction())) {
                onDeviceDrawingStatusChange();
            }
        }
    };

    private PlotterAreaView plotterCanvas;
    private ProgressBar drawingProgress;
    private Button btnStart;
    private Button btnStop;
    private Button btnPause;
    private Button btnResume;
    private TextView txtDrawingStatus;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_drawing_progress);
        super.initViews();

        plotterCanvas = (PlotterAreaView) findViewById(R.id.plotter_canvas);
        drawingProgress = (ProgressBar) findViewById(R.id.drawing_progress);
        txtDrawingStatus = (TextView) findViewById(R.id.txt_drawing_status);

        btnStart = (Button) findViewById(R.id.btn_start);
        btnStart.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                startDrawing();
            }
        });

        btnStop = (Button) findViewById(R.id.btn_stop);
        btnStop.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                stopDrawing();
            }
        });

        btnPause = (Button) findViewById(R.id.btn_pause);
        btnPause.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                pauseDrawing();
            }
        });

        btnResume = (Button) findViewById(R.id.btn_resume);
        btnResume.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                resumeDrawing();
            }
        });

        // зарегистрировать приёмник широковещательных сообщений (broadcast
        // receiver)
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_STATUS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_CURRENT_POS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_START_DRAWING);
        filter.addAction(DeviceControlService.ACTION_DEVICE_FINISH_DRAWING);
        filter.addAction(DeviceControlService.ACTION_DEVICE_DRAWING_UPDATE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_DRAWING_ERROR);
        filter.addAction(DeviceControlService.ACTION_DEVICE_DRAWING_PAUSED);
        filter.addAction(DeviceControlService.ACTION_DEVICE_DRAWING_RESUMED);

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
        plotterCanvas.setDrawingLines(getDeviceControlService()
                .getDeviceDrawingManager().getDrawingLines());
        onDeviceCurrentPosChange();

        updateViews();
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
        Toast.makeText(DrawingProgressActivity.this,
                "Не получилось нарисовать: " + ex.getMessage(),
                Toast.LENGTH_LONG).show();
        updateViews();
    }

    private void onDeviceDrawingStatusChange() {
        updateViews();
    }

    private void onDeviceDrawingUpdate(final Line2D line,
            final LineDrawingStatus lineStatus) {
        plotterCanvas.postInvalidate();
    }

    private void onDeviceStatusUpdate() {
        updateViews();
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
     * Начать процесс рисования.
     */
    private void startDrawing() {
        getDeviceControlService().getDeviceDrawingManager()
                .startDrawingOnDevice();
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
                && getDeviceControlService().getConnectionStatus() == ConnectionStatus.CONNECTED) {
            if (getDeviceControlService().getDeviceDrawingManager().isDrawing()) {
                drawingProgress.setVisibility(View.VISIBLE);
                btnStart.setEnabled(false);
                btnStop.setEnabled(true);
                if (getDeviceControlService().getDeviceDrawingManager()
                        .isDrawingPaused()) {
                    txtDrawingStatus.setText(R.string.drawing_status_paused);
                    btnPause.setEnabled(false);
                    btnResume.setEnabled(true);
                } else {
                    txtDrawingStatus.setText(R.string.drawing_status_drawing);
                    btnPause.setEnabled(true);
                    btnResume.setEnabled(false);
                }
            } else {
                drawingProgress.setVisibility(View.INVISIBLE);
                if (getDeviceControlService().getDeviceDrawingManager()
                        .getDrawingLines().size() > 0) {
                    txtDrawingStatus
                            .setText(R.string.drawing_status_ready_to_draw);
                    btnStart.setEnabled(true);
                } else {
                    txtDrawingStatus
                            .setText(R.string.drawing_status_load_drawing);
                    btnStart.setEnabled(false);
                }
                btnStop.setEnabled(false);
                btnPause.setEnabled(false);
                btnResume.setEnabled(false);
            }
        } else {
            txtDrawingStatus.setText(R.string.drawing_status_not_connected);

            drawingProgress.setVisibility(View.INVISIBLE);
            btnStart.setEnabled(false);
            btnStop.setEnabled(false);
            btnPause.setEnabled(false);
            btnResume.setEnabled(false);
        }
    }
}
