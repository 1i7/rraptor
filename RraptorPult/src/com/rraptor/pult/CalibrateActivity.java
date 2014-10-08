package com.rraptor.pult;

import android.os.Bundle;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.ImageButton;

import com.rraptor.pult.comm.DeviceConnection;
import com.rraptor.pult.core.DeviceControlService.CommandListener;

public class CalibrateActivity extends RRActivity {

    private final CommandListener devCommandListener = new CommandListener() {

        @Override
        public void onCommandExecuted(String cmd, String reply) {
            // TODO Auto-generated method stub

        }

        @Override
        public void onError(String cmd, Exception ex) {
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
                            DeviceConnection.CMD_RR_CALIBRATE_X_FORWARD,
                            devCommandListener);
                    break;
                case R.id.x_backward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_CALIBRATE_X_BACKWARD,
                            devCommandListener);
                    break;
                case R.id.y_forward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_CALIBRATE_Y_FORWARD,
                            devCommandListener);
                    break;
                case R.id.y_backward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_CALIBRATE_Y_BACKWARD,
                            devCommandListener);
                    break;
                case R.id.z_forward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_CALIBRATE_Z_FORWARD,
                            devCommandListener);
                    break;
                case R.id.z_backward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_CALIBRATE_Z_BACKWARD,
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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_calibrate);
        super.initViews();

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

    @Override
    protected void onPause() {
        getDeviceControlService().sendCommand(DeviceConnection.CMD_RR_STOP,
                devCommandListener);

        super.onPause();
    }
}
