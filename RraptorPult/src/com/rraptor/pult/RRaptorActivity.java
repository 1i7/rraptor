package com.rraptor.pult;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class RRaptorActivity extends Activity {

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

		final Button btnGotoPult = (Button) findViewById(R.id.goto_pult_btn);
		btnGotoPult.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				gotoPult();
			}
		});
		final Button btnGotoPlotter2D = (Button) findViewById(R.id.goto_plotter_2d);
		btnGotoPlotter2D.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				gotoPlotter2D();
			}
		});
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.rraptor, menu);
		return true;
	}
}
