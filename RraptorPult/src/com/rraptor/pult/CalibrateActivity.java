package com.rraptor.pult;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.ImageButton;

import com.rraptor.pult.comm.DeviceProtocol;
import com.rraptor.pult.core.DeviceControlService;
import com.rraptor.pult.core.DeviceControlService.CommandListener;
import com.rraptor.pult.core.DeviceControlService.ConnectionStatus;

public class CalibrateActivity extends RRActivity {

    private final BroadcastReceiver deviceBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(final Context context, final Intent intent) {
            if (DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE
                    .equals(intent.getAction())) {
                updateViews();
            } else if (DeviceControlService.ACTION_DEVICE_STATUS_CHANGE
                    .equals(intent.getAction())) {
                updateViews();
            } else if (DeviceControlService.ACTION_DEVICE_START_DRAWING
                    .equals(intent.getAction())) {
                updateViews();
            } else if (DeviceControlService.ACTION_DEVICE_FINISH_DRAWING
                    .equals(intent.getAction())) {
                updateViews();
            } else if (DeviceControlService.ACTION_DEVICE_DRAWING_ERROR
                    .equals(intent.getAction())) {
                updateViews();
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
                case R.id.x_forward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceProtocol.CMD_RR_CALIBRATE_X_FORWARD,
                            devCommandListener);
                    break;
                case R.id.x_backward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceProtocol.CMD_RR_CALIBRATE_X_BACKWARD,
                            devCommandListener);
                    break;
                case R.id.y_forward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceProtocol.CMD_RR_CALIBRATE_Y_FORWARD,
                            devCommandListener);
                    break;
                case R.id.y_backward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceProtocol.CMD_RR_CALIBRATE_Y_BACKWARD,
                            devCommandListener);
                    break;
                case R.id.z_forward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceProtocol.CMD_RR_CALIBRATE_Z_FORWARD,
                            devCommandListener);
                    break;
                case R.id.z_backward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceProtocol.CMD_RR_CALIBRATE_Z_BACKWARD,
                            devCommandListener);
                    break;
                }
            } else if (event.getAction() == MotionEvent.ACTION_UP) {
                getDeviceControlService().sendCommand(
                        DeviceProtocol.CMD_RR_STOP, devCommandListener);
            }
            return false;
        }
    };

    private ImageButton btnXF;
    private ImageButton btnXB;
    private ImageButton btnYF;
    private ImageButton btnYB;
    private ImageButton btnZF;
    private ImageButton btnZB;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_calibrate);
        super.initViews();

        btnXF = (ImageButton) findViewById(R.id.x_forward_btn);
        btnXF.setOnTouchListener(onTouchListener);
        btnXB = (ImageButton) findViewById(R.id.x_backward_btn);
        btnXB.setOnTouchListener(onTouchListener);
        btnYF = (ImageButton) findViewById(R.id.y_forward_btn);
        btnYF.setOnTouchListener(onTouchListener);
        btnYB = (ImageButton) findViewById(R.id.y_backward_btn);
        btnYB.setOnTouchListener(onTouchListener);
        btnZF = (ImageButton) findViewById(R.id.z_forward_btn);
        btnZF.setOnTouchListener(onTouchListener);
        btnZB = (ImageButton) findViewById(R.id.z_backward_btn);
        btnZB.setOnTouchListener(onTouchListener);

        // зарегистрировать приёмник широковещательных сообщений (broadcast
        // receiver)
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_STATUS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_START_DRAWING);
        filter.addAction(DeviceControlService.ACTION_DEVICE_FINISH_DRAWING);
        filter.addAction(DeviceControlService.ACTION_DEVICE_DRAWING_ERROR);
        registerReceiver(deviceBroadcastReceiver, filter);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.rraptor_pult, menu);
        return true;
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

    @Override
    protected void onResume() {
        super.onResume();
        updateViews();
    }

    private void updateViews() {
        boolean enabled;
        if (getDeviceControlService() != null
                && getDeviceControlService().getConnectionStatus() == ConnectionStatus.CONNECTED) {
            if (getDeviceControlService().getDeviceDrawingManager().isDrawing()) {
                enabled = false;
            } else {
                enabled = true;
            }
        } else {
            enabled = false;
        }

        btnXF.setEnabled(enabled);
        btnXB.setEnabled(enabled);
        btnYF.setEnabled(enabled);
        btnYB.setEnabled(enabled);
        btnZF.setEnabled(enabled);
        btnZB.setEnabled(enabled);
    }
}
