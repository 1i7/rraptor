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

public class DebugActivity extends RRActivity {

    private final BroadcastReceiver deviceBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(final Context context, final Intent intent) {
            if (DeviceControlService.ACTION_DEBUG_MESSAGE_POSTED.equals(intent
                    .getAction())) {
                onDebugMessagePosted(intent
                        .getStringExtra(DeviceControlService.EXTRA_DEBUG_MESSAGE));
            }
        }
    };

    private final Handler handler = new Handler();

    private TextView txtDebug;

    /**
     * Очистить лог отладочных сообщений.
     */
    private void clearDebugMessages() {
        getDeviceControlService().clearDebugMessages();
        txtDebug.setText("");
    }

    /**
     * Напечатать отладочное сообщение.
     * 
     * @param msg
     */
    @Override
    protected void debug(final String msg) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                txtDebug.append(msg + "\n");
            }
        });
        System.out.println(msg);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_debug);
        super.initViews();

        txtDebug = (TextView) findViewById(R.id.txt_debug);

        final Button btnClearLog = (Button) findViewById(R.id.btn_clear_log);
        btnClearLog.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                clearDebugMessages();
            }
        });

        // register broadcast receiver
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_DEBUG_MESSAGE_POSTED);
        registerReceiver(deviceBroadcastReceiver, filter);
    }

    private void onDebugMessagePosted(final String msg) {
        debug(msg);
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
        txtDebug.setText(getDeviceControlService().getDebugMessages());
    }
}
