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
import android.widget.TextView;

import com.rraptor.pult.core.DeviceControlService;

public class DeviceStatusActivity extends RRActivity {

    public BroadcastReceiver deviceBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(final Context context, final Intent intent) {
            if (DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE
                    .equals(intent.getAction())) {
                onDeviceStatusUpdate();
            } else if (DeviceControlService.ACTION_DEVICE_STATUS_CHANGE
                    .equals(intent.getAction())) {
                onDeviceStatusUpdate();
            }
        }
    };

    private final Handler handler = new Handler();

    private TextView txtConnectionType;
    private TextView txtConnectionStatus;
    private TextView txtConnectionInfo;
    private TextView txtDeviceName;
    private TextView txtDeviceModel;
    private TextView txtDeviceSerialNumber;
    private TextView txtDeviceDescription;
    private TextView txtDeviceVersion;
    private TextView txtDeviceStatus;
    private TextView txtDeviceWorkingAreaDim;
    private TextView txtDeviceCurrentPos;

    private void disconnectDevice() {

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_status);
        super.initViews();

        txtConnectionType = (TextView) findViewById(R.id.txt_connection_type);
        txtConnectionStatus = (TextView) findViewById(R.id.txt_connection_status);
        txtConnectionInfo = (TextView) findViewById(R.id.txt_connection_info);
        txtDeviceName = (TextView) findViewById(R.id.txt_device_name);
        txtDeviceModel = (TextView) findViewById(R.id.txt_device_model);
        txtDeviceSerialNumber = (TextView) findViewById(R.id.txt_device_serial_number);
        txtDeviceDescription = (TextView) findViewById(R.id.txt_device_description);
        txtDeviceVersion = (TextView) findViewById(R.id.txt_device_version);
        txtDeviceStatus = (TextView) findViewById(R.id.txt_device_status);
        txtDeviceWorkingAreaDim = (TextView) findViewById(R.id.txt_working_area_dim);
        txtDeviceCurrentPos = (TextView) findViewById(R.id.txt_current_pos);

        final Button btnPause = (Button) findViewById(R.id.btn_pause);
        btnPause.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                pauseDevice();
            }
        });

        final Button btnResume = (Button) findViewById(R.id.btn_resume);
        btnResume.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                resumeDevice();
            }
        });

        final Button btnStop = (Button) findViewById(R.id.btn_stop);
        btnStop.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                stopDevice();
            }
        });

        final Button btnDisconnect = (Button) findViewById(R.id.btn_disconnect);
        btnDisconnect.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                disconnectDevice();
            }
        });

        // register broadcast receiver
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_DEBUG_MESSAGE_POSTED);
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
        updateStatusViews();
    }

    private void onDeviceStatusUpdate() {
        handler.post(new Runnable() {

            @Override
            public void run() {
                // TODO Auto-generated method stub

            }

        });
    }

    private void pauseDevice() {

    }

    private void resumeDevice() {

    }

    private void stopDevice() {

    }

    private void updateStatusViews() {
        txtConnectionStatus.setText(getDeviceControlService()
                .getConnectionStatus().name());
        txtConnectionType
                .setText(getDeviceControlService().getConnectionType());
        txtConnectionInfo
                .setText(getDeviceControlService().getConnectionInfo());
        txtDeviceName.setText(getDeviceControlService().getDeviceName());
        txtDeviceModel.setText(getDeviceControlService().getDeviceModel());
        txtDeviceSerialNumber.setText(getDeviceControlService()
                .getDeviceSerialNumber());
        txtDeviceDescription.setText(getDeviceControlService()
                .getDeviceDescription());
        txtDeviceVersion.setText(getDeviceControlService().getDeviceVersion());
        txtDeviceStatus.setText(getDeviceControlService().getDeviceStatus()
                .name());
        txtDeviceWorkingAreaDim.setText(getDeviceControlService()
                .getDeviceWorkingArea());
        txtDeviceCurrentPos.setText(getDeviceControlService()
                .getDeviceCurrentPosition());
    }
}
