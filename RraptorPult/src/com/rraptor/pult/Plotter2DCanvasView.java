package com.rraptor.pult;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;

public class Plotter2DCanvasView extends View {

	public enum LineDrawingStatus {
		NORMAL, DRAWING_PROGRESS, DRAWING_ERROR, DRAWN
	}

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

	private final Map<Line2D, LineDrawingStatus> lineStatus = new HashMap<Line2D, LineDrawingStatus>();

	private Point2D userPoint1 = null;

	public Plotter2DCanvasView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public void clearDrawing() {
		drawingLines.clear();
		canvasDrawingLines.clear();
		resetLineStatus();
		this.invalidate();
	}

	public List<Line2D> getDrawingLines() {
		return drawingLines;
	}

	public LineDrawingStatus getLineStatus(Line2D line) {
		final LineDrawingStatus status = lineStatus.get(line);
		return status == null ? LineDrawingStatus.NORMAL : status;
	}

	@Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);

		translateDrawing();

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
		canvas.drawText("y", canvasP2.getX() - 10, canvasP2.getY() + 10, paint);
		canvas.drawText("x", canvasP4.getX() + 2, canvasP4.getY() - 2, paint);

		// draw drawing lines
		int i = 0;
		for (final Line2D line : canvasDrawingLines) {
			final LineDrawingStatus status = getLineStatus(drawingLines.get(i));
			switch (status) {
			case NORMAL:
				paint.setColor(Color.BLUE);
				paint.setStrokeWidth(2);
				break;
			case DRAWING_PROGRESS:
				paint.setColor(Color.YELLOW);
				paint.setStrokeWidth(2);
				break;
			case DRAWING_ERROR:
				paint.setColor(Color.RED);
				paint.setStrokeWidth(2);
				break;
			case DRAWN:
				paint.setColor(Color.GREEN);
				paint.setStrokeWidth(2);
				break;
			}

			final Point2D start = line.getStart();
			final Point2D end = line.getEnd();
			canvas.drawLine(start.getX(), start.getY(), end.getX(), end.getY(),
					paint);
			i++;
		}

		// draw user point if set ()
		if (userPoint1 != null) {
			paint.setColor(Color.BLUE);
			canvas.drawCircle(userPoint1.getX(), userPoint1.getY(), 5, paint);
		}
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		if (isEnabled()) {
			final Point2D point = new Point2D((int) event.getX(),
					(int) event.getY());
			if (userPoint1 == null) {
				userPoint1 = point;
			} else {
				drawingLines.add(new Line2D(translateCanvasPoint(userPoint1),
						translateCanvasPoint((point))));
				userPoint1 = null;
			}
			this.invalidate();
		}
		return super.onTouchEvent(event);
	}

	public void resetLineStatus() {
		lineStatus.clear();
		postInvalidate();
	}

	public void setDrawingLines(List<Line2D> lines) {
		drawingLines.clear();
		drawingLines.addAll(lines);
		canvasDrawingLines.clear();
		resetLineStatus();
		this.invalidate();
	}

	@Override
	public void setEnabled(boolean enabled) {
		this.userPoint1 = null;
		super.setEnabled(enabled);
	}

	public void setLineStatus(Line2D line, LineDrawingStatus status) {
		lineStatus.put(line, status);
		postInvalidate();
	}

	/**
	 * Convert canvas point to drawing coordinate system.
	 * 
	 * @param canvasPoint
	 * @return
	 */
	private Point2D translateCanvasPoint(Point2D canvasPoint) {
		int canvasWidth = this.getWidth();
		int canvasHeight = this.getHeight();

		float scaleFactor = (float) canvasWidth / (float) PAPER_WIDTH;
		int dy = (int) (canvasHeight - PAPER_HEIGHT * scaleFactor) / 2;

		final Point2D drawingPoint = new Point2D(
				(int) (canvasPoint.getX() / scaleFactor), (int) ((canvasHeight
						- dy - canvasPoint.getY()) / scaleFactor));
		return drawingPoint;
	}

	private void translateDrawing() {
		// translate working area bounds
		canvasP1 = translateDrawingPoint(p1);
		canvasP2 = translateDrawingPoint(p2);
		canvasP3 = translateDrawingPoint(p3);
		canvasP4 = translateDrawingPoint(p4);

		// translate drawing line
		canvasDrawingLines.clear();
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
