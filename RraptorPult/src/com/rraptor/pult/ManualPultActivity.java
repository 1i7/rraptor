package com.rraptor.pult;

import java.util.ArrayList;

import android.content.ActivityNotFoundException;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.speech.RecognizerIntent;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.Toast;

import com.rraptor.pult.comm.DeviceConnection;
import com.rraptor.pult.core.DeviceControlService;
import com.rraptor.pult.core.DeviceControlService.CommandListener;
import com.rraptor.pult.view.PlotterAreaView;

public class ManualPultActivity extends RRActivity {
    private static final int VOICE_RECOGNITION_REQUEST_CODE = 1;

    private final BroadcastReceiver deviceBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(final Context context, final Intent intent) {
            if (DeviceControlService.ACTION_DEVICE_CURRENT_POS_CHANGE
                    .equals(intent.getAction())) {
                onDeviceCurrentPosChange();
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
                            DeviceConnection.CMD_RR_GO_X_FORWARD,
                            devCommandListener);
                    break;
                case R.id.x_backward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_GO_X_BACKWARD,
                            devCommandListener);
                    break;
                case R.id.y_forward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_GO_Y_FORWARD,
                            devCommandListener);
                    break;
                case R.id.y_backward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_GO_Y_BACKWARD,
                            devCommandListener);
                    break;
                case R.id.z_forward_btn:
                    getDeviceControlService().sendCommand(
                            DeviceConnection.CMD_RR_GO_Z_FORWARD,
                            devCommandListener);
                    break;
                case R.id.z_backward_btn:
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

    private PlotterAreaView plotterCanvas;

    /**
     * http://www.youtube.com/watch?v=gGbYVvU0Z5s&feature=player_embedded
     * http://developer.android.com/resources/articles/speech-input.html
     */
    // TODO
    public void commandVoice() {
        Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);

        // Specify the calling package to identify your application
        // intent.putExtra(RecognizerIntent.EXTRA_CALLING_PACKAGE,
        // getClass().getPackage().getName());

        // Display an hint to the user about what he should say.
        intent.putExtra(RecognizerIntent.EXTRA_PROMPT, "Произнесите команду");

        // Given an hint to the recognizer about what the user is going
        // to say
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL,
                RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);

        // Specify how many results you want to receive. The results
        // will be sorted
        // where the first result is the one with higher confidence.
        intent.putExtra(RecognizerIntent.EXTRA_MAX_RESULTS, 1);

        // Specify the recognition language. This parameter has to be
        // specified only if the
        // recognition has to be done in a specific language and not the
        // default one (i.e., the
        // system locale). Most of the applications do not have to set
        // this parameter.
        // if (!mSupportedLanguageView.getSelectedItem().toString()
        // .equals("Default")) {
        // intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE,
        // mSupportedLanguageView.getSelectedItem().toString());
        // }

        try {
            startActivityForResult(intent, VOICE_RECOGNITION_REQUEST_CODE);
        } catch (ActivityNotFoundException ex) {
            Toast.makeText(
                    this,
                    "Распознавание голоса не поддерживается на этом устройстве",
                    Toast.LENGTH_LONG).show();
        }
    }

    private void handleVoiceCommand(String cmd) {
        System.out.println("Handle voice command: " + cmd);

        if (cmd.contains("вправо") || cmd.contains("право")
                || cmd.contains("права")) {
            Toast.makeText(this, "Голосовая команда: " + "вправо",
                    Toast.LENGTH_LONG).show();
            getDeviceControlService().sendCommand(
                    DeviceConnection.CMD_RR_GO_X_FORWARD, devCommandListener);
        } else if (cmd.contains("влево") || cmd.contains("лево")
                || cmd.contains("лего")) {
            Toast.makeText(this, "Голосовая команда: " + "влево",
                    Toast.LENGTH_LONG).show();
            getDeviceControlService().sendCommand(
                    DeviceConnection.CMD_RR_GO_X_BACKWARD, devCommandListener);
        } else if (cmd.contains("вперед") || cmd.contains("перед")) {
            Toast.makeText(this, "Голосовая команда: " + "вперед",
                    Toast.LENGTH_LONG).show();
            getDeviceControlService().sendCommand(
                    DeviceConnection.CMD_RR_GO_Y_FORWARD, devCommandListener);
        } else if (cmd.contains("назад")) {
            Toast.makeText(this, "Голосовая команда: " + "назад",
                    Toast.LENGTH_LONG).show();
            getDeviceControlService().sendCommand(
                    DeviceConnection.CMD_RR_GO_Y_BACKWARD, devCommandListener);
        } else if (cmd.contains("вверх") || cmd.contains("верх")) {
            Toast.makeText(this, "Голосовая команда: " + "вверх",
                    Toast.LENGTH_LONG).show();
            getDeviceControlService().sendCommand(
                    DeviceConnection.CMD_RR_GO_Z_FORWARD, devCommandListener);
        } else if (cmd.contains("вниз") || cmd.contains("низ")) {
            Toast.makeText(this, "Голосовая команда: " + "вниз",
                    Toast.LENGTH_LONG).show();
            getDeviceControlService().sendCommand(
                    DeviceConnection.CMD_RR_GO_Z_BACKWARD, devCommandListener);
        } else if (cmd.contains("стоп")) {
            Toast.makeText(this, "Голосовая команда: " + "стоп",
                    Toast.LENGTH_LONG).show();
            getDeviceControlService().sendCommand(DeviceConnection.CMD_RR_STOP,
                    devCommandListener);
        } else {
            Toast.makeText(this, "Не понимаю: " + cmd, Toast.LENGTH_LONG)
                    .show();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == VOICE_RECOGNITION_REQUEST_CODE
                && resultCode == RESULT_OK) {
            // Fill the list view with the strings the recognizer thought it
            // could have heard
            final ArrayList<String> matches = data
                    .getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);
            if (matches.size() > 0) {
                handleVoiceCommand(matches.get(0));
            }
        }

        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_manual_pult);
        super.initViews();

        plotterCanvas = (PlotterAreaView) findViewById(R.id.plotter_canvas);

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

        final Button commandVoice = (Button) findViewById(R.id.command_voice_btn);
        commandVoice.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                commandVoice();
            }
        });

        // register broadcast receiver
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_DEVICE_CURRENT_POS_CHANGE);
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
        plotterCanvas.setDrawingLines(getDeviceControlService()
                .getDeviceDrawingManager().getDrawingLines());
        onDeviceCurrentPosChange();
    }

    /**
     * Обновить текущее положение рабочего блока.
     */
    void onDeviceCurrentPosChange() {
        plotterCanvas.setWorkingBlockPosition(getDeviceControlService()
                .getDeviceCurrentPosition());
    }

    @Override
    protected void onPause() {
        getDeviceControlService().sendCommand(DeviceConnection.CMD_RR_STOP,
                devCommandListener);

        super.onPause();
    }
}
