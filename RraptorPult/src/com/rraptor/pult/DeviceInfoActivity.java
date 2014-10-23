package com.rraptor.pult;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.widget.TextView;

import com.rraptor.pult.core.DeviceControlService;

public class DeviceInfoActivity extends RRActivity {

    private final BroadcastReceiver deviceBroadcastReceiver = new BroadcastReceiver() {

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

    private TextView txtConnectionType;
    private TextView txtConnectionStatus;
    private TextView txtConnectionInfo;
    private TextView txtDeviceName;
    private TextView txtDeviceModel;
    private TextView txtDeviceSerialNumber;
    private TextView txtDeviceDescription;
    private TextView txtDeviceVersion;
    private TextView txtDeviceManufacturer;
    private TextView txtDeviceUri;
    private TextView txtDeviceStatus;
    private TextView txtDeviceWorkingAreaDim;
    private TextView txtDeviceCurrentPos;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_info);
        super.initViews();

        txtConnectionType = (TextView) findViewById(R.id.txt_connection_type);
        txtConnectionStatus = (TextView) findViewById(R.id.txt_connection_status);
        txtConnectionInfo = (TextView) findViewById(R.id.txt_connection_info);
        txtDeviceName = (TextView) findViewById(R.id.txt_device_name);
        txtDeviceModel = (TextView) findViewById(R.id.txt_device_model);
        txtDeviceSerialNumber = (TextView) findViewById(R.id.txt_device_serial_number);
        txtDeviceDescription = (TextView) findViewById(R.id.txt_device_description);
        txtDeviceVersion = (TextView) findViewById(R.id.txt_device_version);
        txtDeviceManufacturer = (TextView) findViewById(R.id.txt_device_manufacturer);
        txtDeviceUri = (TextView) findViewById(R.id.txt_device_uri);
        txtDeviceStatus = (TextView) findViewById(R.id.txt_device_status);
        txtDeviceWorkingAreaDim = (TextView) findViewById(R.id.txt_working_area_dim);
        txtDeviceCurrentPos = (TextView) findViewById(R.id.txt_current_pos);

        // зарегистрировать приёмник широковещательных сообщений (broadcast
        // receiver)
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_STATUS_CHANGE);
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

    private void onDeviceStatusUpdate() {
        updateViews();
    }

    private void updateViews() {
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
        txtDeviceManufacturer.setText(getDeviceControlService()
                .getDeviceManufacturer());
        txtDeviceUri.setText(getDeviceControlService().getDeviceUri());
        txtDeviceStatus.setText(getDeviceControlService().getDeviceStatus()
                .name());
        if (getDeviceControlService().getDeviceWorkingArea() != null) {
            txtDeviceWorkingAreaDim.setText(getDeviceControlService()
                    .getDeviceWorkingArea().getX()
                    + "x"
                    + getDeviceControlService().getDeviceWorkingArea().getY()
                    + "x"
                    + getDeviceControlService().getDeviceWorkingArea().getZ()
                    + " ");
        } else {
            txtDeviceWorkingAreaDim.setText("");
        }
        if (getDeviceControlService().getDeviceCurrentPosition() != null) {
            txtDeviceCurrentPos.setText(getDeviceControlService()
                    .getDeviceCurrentPosition().getX()
                    + " "
                    + getDeviceControlService().getDeviceCurrentPosition()
                            .getY()
                    + " "
                    + getDeviceControlService().getDeviceCurrentPosition()
                            .getZ());
        } else {
            txtDeviceCurrentPos.setText("");
        }
    }
}
