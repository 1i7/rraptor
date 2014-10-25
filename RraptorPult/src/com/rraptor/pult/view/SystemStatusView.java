package com.rraptor.pult.view;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.rraptor.pult.R;
import com.rraptor.pult.core.DeviceControlService.ConnectionStatus;
import com.rraptor.pult.core.DeviceControlService.DeviceStatus;

public class SystemStatusView extends LinearLayout {
    public enum DeviceStatusPanelMode {
        CONNECTING, CONNECTED, DISCONNCTED
    }

    private final TextView txtConnectionStatus;
    private final TextView txtConnectionInfo;
    private final TextView txtConnectionError;

    private final View frmDeviceInfo;
    private final TextView txtIsDrawing;
    private final TextView txtIsDrawingPaused;
    private final TextView txtDeviceStatus;

    private final View frmNotConnected;
    private final Button btnConnect;

    public SystemStatusView(final Context context, final AttributeSet attrs) {
        super(context, attrs);
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.view_system_status, this);

        txtConnectionStatus = (TextView) findViewById(R.id.rr_txt_connection_status);
        txtConnectionInfo = (TextView) findViewById(R.id.rr_txt_connection_info);
        txtConnectionError = (TextView) findViewById(R.id.rr_txt_connection_error);

        frmDeviceInfo = findViewById(R.id.rr_frm_device_info);
        txtIsDrawing = (TextView) findViewById(R.id.rr_txt_is_drawing);
        txtIsDrawingPaused = (TextView) findViewById(R.id.rr_txt_is_paused);
        txtDeviceStatus = (TextView) findViewById(R.id.rr_txt_device_status);

        frmNotConnected = findViewById(R.id.rr_frm_not_connected);
        btnConnect = (Button) findViewById(R.id.rr_btn_connect);
    }

    public void setConnectionErrorMessage(final String errorMessage) {
        txtConnectionError.setText(": " + errorMessage);
    }

    public void setConnectionInfo(final String connectionInfo) {
        txtConnectionInfo.setText(": " + connectionInfo);
    }

    public void setConnectionStatus(final ConnectionStatus status) {
        switch (status) {
        case CONNECTED:
            frmDeviceInfo.setVisibility(View.VISIBLE);
            frmNotConnected.setVisibility(View.GONE);

            txtConnectionStatus.setText(R.string.status_connected);

            txtConnectionInfo.setVisibility(View.VISIBLE);
            txtConnectionError.setVisibility(View.GONE);

            break;
        case CONNECTING:
            frmDeviceInfo.setVisibility(View.GONE);
            frmNotConnected.setVisibility(View.GONE);

            txtConnectionStatus.setText(R.string.status_connecting);

            txtConnectionInfo.setVisibility(View.GONE);
            txtConnectionError.setVisibility(View.GONE);

            break;
        case DISCONNECTED:
            frmDeviceInfo.setVisibility(View.GONE);
            frmNotConnected.setVisibility(View.VISIBLE);

            txtConnectionStatus.setText(R.string.status_disconnected);

            txtConnectionInfo.setVisibility(View.GONE);
            txtConnectionError.setVisibility(View.GONE);
            break;
        case ERROR:
            frmDeviceInfo.setVisibility(View.GONE);
            frmNotConnected.setVisibility(View.VISIBLE);

            txtConnectionStatus.setText(R.string.status_error);

            txtConnectionInfo.setVisibility(View.GONE);
            txtConnectionError.setVisibility(View.VISIBLE);
            break;
        default:
            break;
        }
    }

    public void setConnectOnClickListener(final OnClickListener l) {
        btnConnect.setOnClickListener(l);
    }

    public void setDeviceStatus(final DeviceStatus status) {
        switch (status) {
        case IDLE:
            txtDeviceStatus.setText("ожидаю");
            txtDeviceStatus.setTextColor(Color.GREEN);
            break;
        case WORKING:
            txtDeviceStatus.setText("работаю");
            txtDeviceStatus.setTextColor(Color.RED);
            break;
        case UNKNOWN:
        default:
            txtDeviceStatus.setText("");
            txtDeviceStatus.setTextColor(Color.BLACK);
            break;
        }
    }

    public void setDrawingStatus(final boolean isDrawing,
            final boolean isDrawingPaused) {
        if (isDrawing) {
            txtIsDrawing.setVisibility(View.VISIBLE);
            txtIsDrawing.setText("рисую");
            if (isDrawingPaused) {
                txtIsDrawingPaused.setVisibility(View.VISIBLE);
                txtIsDrawingPaused.setText("на паузе");
            } else {
                txtIsDrawingPaused.setVisibility(View.GONE);
            }
        } else {
            txtIsDrawing.setVisibility(View.GONE);
            txtIsDrawingPaused.setVisibility(View.GONE);
        }
    }
}
