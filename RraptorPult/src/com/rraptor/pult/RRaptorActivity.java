package com.rraptor.pult;

import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class RRaptorActivity extends RRActivity {

    private void gotoCalibrate() {
        startActivity(new Intent(this, CalibrateActivity.class));
    }

    private void gotoDebug() {
        startActivity(new Intent(this, DebugActivity.class));
    }

    private void gotoDrawingProgress() {
        startActivity(new Intent(this, DrawingProgressActivity.class));
    }

    private void gotoInfo() {
        startActivity(new Intent(this, DeviceInfoActivity.class));
    }

    private void gotoPlotter2D() {
        startActivity(new Intent(this, Plotter2DActivity.class));
    }

    private void gotoPult() {
        startActivity(new Intent(this, ManualPultActivity.class));
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rraptor);
        super.initViews();

        final Button btnGotoPlotter2D = (Button) findViewById(R.id.btn_goto_plotter_2d);
        btnGotoPlotter2D.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                gotoPlotter2D();
            }
        });

        final Button btnGotoDrawingProgress = (Button) findViewById(R.id.btn_goto_drawing_progress);
        btnGotoDrawingProgress.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                gotoDrawingProgress();
            }
        });

        final Button btnGotoPult = (Button) findViewById(R.id.btn_goto_manual_pult);
        btnGotoPult.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                gotoPult();
            }
        });

        final Button btnGotoCalibrate = (Button) findViewById(R.id.btn_goto_calibrate);
        btnGotoCalibrate.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                gotoCalibrate();
            }
        });

        final Button btnGotoInfo = (Button) findViewById(R.id.btn_goto_info);
        btnGotoInfo.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                gotoInfo();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.rraptor, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(final MenuItem item) {
        // Handle item selection
        switch (item.getItemId()) {
        case R.id.action_debug:
            gotoDebug();
            return true;
        default:
            return super.onOptionsItemSelected(item);
        }
    }
}
