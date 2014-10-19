package com.rraptor.pult;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import org.kabeja.parser.ParseException;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.rraptor.pult.core.DeviceControlService;
import com.rraptor.pult.dxf.DxfLoader;
import com.rraptor.pult.view.VectorDrawing2DView;
import com.rraptor.pult.yad.YaDiskActivity;

import edu.android.openfiledialog.OpenFileDialog;

/**
 * 
 */
public class Plotter2DActivity extends RRActivity {

    private static int REQUEST_CODE_PICK_FILE = 0;

    private VectorDrawing2DView plotterCanvas;
    private ProgressBar drawingProgress;
    private Button btnOpenFileYaDisk;
    private Button btnOpenFileSdcard;
    private Button btnClearDrawing;
    private Button btnStartDrawing;

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

        plotterCanvas = (VectorDrawing2DView) findViewById(R.id.plotter_canvas);
        drawingProgress = (ProgressBar) findViewById(R.id.drawing_progress);

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

        btnClearDrawing = (Button) findViewById(R.id.clear_btn);
        btnClearDrawing.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                clearDrawing();
            }
        });

        btnStartDrawing = (Button) findViewById(R.id.draw_btn);
        btnStartDrawing.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                getDeviceControlService().getDeviceDrawingManager()
                        .startDrawingOnDevice(plotterCanvas.getDrawingLines());
            }
        });

        loadDemoDxf();
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

    private void updateViews() {
        if (getDeviceControlService() != null
                && getDeviceControlService().getDeviceDrawingManager()
                        .isDrawing()) {
            drawingProgress.setVisibility(View.VISIBLE);
            plotterCanvas.setEnabled(false);
            btnClearDrawing.setEnabled(false);
            btnOpenFileYaDisk.setEnabled(false);
            btnOpenFileSdcard.setEnabled(false);
            btnStartDrawing.setEnabled(false);
        } else {
            drawingProgress.setVisibility(View.INVISIBLE);
            plotterCanvas.setEnabled(true);
            btnClearDrawing.setEnabled(true);
            btnOpenFileYaDisk.setEnabled(true);
            btnOpenFileSdcard.setEnabled(true);
            btnStartDrawing.setEnabled(true);
        }
    }
}
