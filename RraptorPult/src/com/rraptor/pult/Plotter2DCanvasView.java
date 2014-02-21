package com.rraptor.pult;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.kabeja.parser.ParseException;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.View;

import com.rraptor.pult.dxf.DxfLoader;
import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;

public class Plotter2DCanvasView extends View {

	private final int PAPER_WIDTH = 300;
	private final int PAPER_HEIGHT = 216;

	private final Paint paint = new Paint();

	private final Point2D p1 = new Point2D(0, 0);
	private final Point2D p2 = new Point2D(PAPER_WIDTH - 1, 0);
	private final Point2D p3 = new Point2D(PAPER_WIDTH - 1, PAPER_HEIGHT - 1);
	private final Point2D p4 = new Point2D(0, PAPER_HEIGHT - 1);
	private final List<Line2D> drawingLines = new ArrayList<Line2D>();

	private Point2D canvasP1;
	private Point2D canvasP2;
	private Point2D canvasP3;
	private Point2D canvasP4;
	private final List<Line2D> canvasDrawingLines = new ArrayList<Line2D>();

	public Plotter2DCanvasView(Context context, AttributeSet attrs) {
		super(context, attrs);

		fillTestDataDxf();
	}

	public void clearDrawing() {
		drawingLines.clear();
		canvasDrawingLines.clear();
		this.invalidate();
	}

	private void fillTestData1() {
		// drawingPoints.add(new Point2D(10, 10));
		// drawingPoints.add(new Point2D(10, 30));
		// drawingPoints.add(new Point2D(50, 50));
		// drawingPoints.add(new Point2D(10, 10));
	}

	private void fillTestDataDxf() {
		try {
			drawingLines.addAll(DxfLoader.getLines(DxfLoader.getTestDxfFile()));
		} catch (ParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public List<Line2D> getDrawingLines() {
		return drawingLines;
	}

	@Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);

		// draw working area
		paint.setColor(Color.WHITE);
		canvas.drawRect(canvasP1.getX(), canvasP1.getY(), canvasP3.getX(),
				canvasP3.getY(), paint);

		paint.setColor(Color.RED);
		paint.setStrokeWidth(1);

		canvas.drawLine(canvasP1.getX(), canvasP1.getY(), canvasP2.getX(),
				canvasP2.getY(), paint);
		canvas.drawLine(canvasP2.getX(), canvasP2.getY(), canvasP3.getX(),
				canvasP3.getY(), paint);
		canvas.drawLine(canvasP3.getX(), canvasP3.getY(), canvasP4.getX(),
				canvasP4.getY(), paint);
		canvas.drawLine(canvasP4.getX(), canvasP4.getY(), canvasP1.getX(),
				canvasP1.getY(), paint);

		paint.setTextSize(10);
		canvas.drawText("0", canvasP1.getX() + 2, canvasP1.getY() + 10, paint);
		canvas.drawText("X", canvasP2.getX() - 10, canvasP2.getY() + 10, paint);
		canvas.drawText("Y", canvasP4.getX() + 2, canvasP4.getY() - 2, paint);

		// draw drawing lines
		paint.setColor(Color.BLUE);
		paint.setStrokeWidth(2);
		for (final Line2D line : canvasDrawingLines) {
			final Point2D start = line.getStart();
			final Point2D end = line.getEnd();
			canvas.drawLine(start.getX(), start.getY(), end.getX(), end.getY(),
					paint);
		}
	}

	@Override
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		super.onMeasure(widthMeasureSpec, heightMeasureSpec);
		translateDrawing();
	}

	private void translateDrawing() {
		// translate working area bounds
		canvasP1 = translateDrawingPoint(p1);
		canvasP2 = translateDrawingPoint(p2);
		canvasP3 = translateDrawingPoint(p3);
		canvasP4 = translateDrawingPoint(p4);

		// translate drawing line
		for (Line2D line : drawingLines) {
			canvasDrawingLines.add(new Line2D(translateDrawingPoint(line
					.getStart()), translateDrawingPoint(line.getEnd())));
		}
	}

	/**
	 * Translate point from drawing to canvas coordinate system: drawing (0,0)
	 * point is in bottom left corner, and canvas (0,0) point is top left
	 * corner. And scale it to working area bounds.
	 * 
	 * @return
	 */
	private Point2D translateDrawingPoint(Point2D drawingPoint) {
		int canvasWidth = this.getWidth();
		int canvasHeight = this.getHeight();

		float scaleFactor = (float) canvasWidth / (float) PAPER_WIDTH;
		int dy = (int) (canvasHeight - PAPER_HEIGHT * scaleFactor) / 2;

		final Point2D canvasPoint = new Point2D(
				(int) (drawingPoint.getX() * scaleFactor), (int) (canvasHeight
						- dy - drawingPoint.getY() * scaleFactor));
		return canvasPoint;
	}
}
