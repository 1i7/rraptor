package com.rraptor.pult.view;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.DashPathEffect;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.View;

import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;

public class WorkingArea2DView extends View {

    private final boolean DEBUG_ON = false;

    // размер рабочей области, в миллиметрах
    /**
     * Ширина рабочей области по X, мм
     */
    protected final int WORKING_AREA_WIDTH = 300;
    /**
     * Высота рабочей области по Y, мм
     */
    protected final int WORKING_AREA_HEIGHT = 216;

    protected int canvasIndentLeft = 30;
    protected int canvasIndentRight = 30;
    protected int canvasIndentTop = 30;
    protected int canvasIndentBottom = 30;

    private final Paint paint = new Paint();
    protected final DashPathEffect dashedLineEffect = new DashPathEffect(
            new float[] { 5, 10 }, 0);

    // Точки в системе координат устройства, в миллиметрах
    protected final Point2D p1 = new Point2D(0, 0);
    protected final Point2D p2 = new Point2D(WORKING_AREA_WIDTH - 1, 0);
    protected final Point2D p3 = new Point2D(WORKING_AREA_WIDTH - 1,
            WORKING_AREA_HEIGHT - 1);
    protected final Point2D p4 = new Point2D(0, WORKING_AREA_HEIGHT - 1);

    protected final List<Line2D> drawingLines = new ArrayList<Line2D>();

    // Точки в системе координат холста для рисования
    protected Point2D canvasP1;
    protected Point2D canvasP2;
    protected Point2D canvasP3;
    protected Point2D canvasP4;

    private final List<Line2D> canvasDrawingLines = new ArrayList<Line2D>();

    public WorkingArea2DView(final Context context, final AttributeSet attrs) {
        super(context, attrs);
    }

    public WorkingArea2DView(final Context context, final AttributeSet attrs,
            int canvasIndentLeft, int canvasIndentRight, int canvasIndentTop,
            int canvasIndentBottom) {
        super(context, attrs);

        this.canvasIndentLeft = canvasIndentLeft;
        this.canvasIndentRight = canvasIndentRight;
        this.canvasIndentTop = canvasIndentTop;
        this.canvasIndentBottom = canvasIndentBottom;
    }

    public void clearDrawing() {
        drawingLines.clear();
        canvasDrawingLines.clear();
        this.invalidate();
    }

    public List<Line2D> getDrawingLines() {
        return drawingLines;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        translateDrawing();

        if (DEBUG_ON) {
            canvas.drawColor(Color.LTGRAY);
        }

        // нарисовать рабочую область устройства - оси X и Y
        paint.setColor(Color.BLACK);
        paint.setStrokeWidth(1);
        paint.setPathEffect(dashedLineEffect);
        paint.setStyle(Paint.Style.STROKE);
        canvas.drawRect((float) canvasP4.getX(), (float) canvasP4.getY(),
                (float) canvasP2.getX(), (float) canvasP2.getY(), paint);

        // отметки на осях
        paint.setPathEffect(null);
        paint.setTextSize(20);
        canvas.drawText("0", (float) canvasP1.getX() - 20,
                (float) canvasP1.getY() + 20, paint);
        canvas.drawText("x", (float) canvasP2.getX() + 4,
                (float) canvasP2.getY() + 20, paint);
        canvas.drawText("y", (float) canvasP4.getX() - 20,
                (float) canvasP4.getY() - 4, paint);

        // рисунок - линии
        int i = 0;
        for (final Line2D line : canvasDrawingLines) {
            prepareDrawLine(drawingLines.get(i), paint);

            final Point2D start = line.getStart();
            final Point2D end = line.getEnd();
            canvas.drawLine((float) start.getX(), (float) start.getY(),
                    (float) end.getX(), (float) end.getY(), paint);
            i++;
        }
    }

    /**
     * Настроить paint для рисования линии.
     * 
     * @param line
     * @param paint
     */
    protected void prepareDrawLine(final Line2D line, final Paint paint) {
        paint.setColor(Color.GRAY);
        paint.setStrokeWidth(2);
    }

    /**
     * Установить контуры для рисования на холсте в системе координат
     * устройства.
     * 
     * @param lines
     */
    public void setDrawingLines(final List<Line2D> lines) {
        drawingLines.clear();
        if (lines != null) {
            drawingLines.addAll(lines);
        }

        canvasDrawingLines.clear();
        this.invalidate();
    }

    /**
     * Перевести точку на рабочей области холста в систему координат устройства.
     * 
     * @param canvasPoint
     * @return
     */
    protected Point2D translateCanvasPointToDevice(final Point2D canvasPoint) {
        // оставим свободное место для отступов по краям
        int canvasWidth = this.getWidth() - canvasIndentLeft
                - canvasIndentRight;
        int canvasHeight = this.getHeight() - canvasIndentTop
                - canvasIndentBottom;

        float scaleFactorX = (float) canvasWidth / (float) WORKING_AREA_WIDTH;
        float scaleFactorY = (float) canvasHeight / (float) WORKING_AREA_HEIGHT;
        float scaleFactor = Math.min(scaleFactorX, scaleFactorY);

        // рабочая область сдвинута в центр по каждой из осей плюс отступ от
        // границ
        int dx = (int) (canvasWidth - WORKING_AREA_WIDTH * scaleFactor) / 2
                + canvasIndentLeft;
        int dy = (int) (canvasHeight - WORKING_AREA_HEIGHT * scaleFactor) / 2
                + canvasIndentBottom;

        final Point2D devicePoint = new Point2D(
                (int) ((canvasPoint.getX() - dx) / scaleFactor),
                // ось y нужно перевернуть вверх ногами
                (int) ((this.getHeight() - (canvasPoint.getY() + dy)) / scaleFactor));
        return devicePoint;
    }

    /**
     * Перевести координаты точки из системы координат устройства в систему
     * координат холста canvas: точка (0,0) устройства в левом нижнем углу,
     * точка (0,0) холста в правом верхнем углу. Рабочая область устройства
     * масштабируется до размеров холста.
     * 
     * @return
     */
    protected Point2D translateDevicePointToCanvas(final Point2D devicePoint) {
        // оставим свободное место для отступов по краям
        int canvasWidth = this.getWidth() - canvasIndentLeft
                - canvasIndentRight;
        int canvasHeight = this.getHeight() - canvasIndentTop
                - canvasIndentBottom;

        float scaleFactorX = (float) canvasWidth / (float) WORKING_AREA_WIDTH;
        float scaleFactorY = (float) canvasHeight / (float) WORKING_AREA_HEIGHT;
        float scaleFactor = Math.min(scaleFactorX, scaleFactorY);

        // сдвинем рабочую область в центр по каждой из осей плюс отступ от
        // границ
        int dx = (int) (canvasWidth - WORKING_AREA_WIDTH * scaleFactor) / 2
                + canvasIndentLeft;
        int dy = (int) (canvasHeight - WORKING_AREA_HEIGHT * scaleFactor) / 2
                + canvasIndentBottom;

        final Point2D canvasPoint = new Point2D(
                (int) (devicePoint.getX() * scaleFactor + dx),
                // ось y нужно перевернуть вверх ногами
                (int) (this.getHeight() - (devicePoint.getY() * scaleFactor + dy)));
        return canvasPoint;
    }

    /**
     * Перевести весь рисунок из системы координат устройства в систему
     * координат холста.
     */
    private void translateDrawing() {
        // translate working area bounds
        canvasP1 = translateDevicePointToCanvas(p1);
        canvasP2 = translateDevicePointToCanvas(p2);
        canvasP3 = translateDevicePointToCanvas(p3);
        canvasP4 = translateDevicePointToCanvas(p4);

        // translate drawing line
        canvasDrawingLines.clear();
        for (Line2D line : drawingLines) {
            canvasDrawingLines.add(new Line2D(translateDevicePointToCanvas(line
                    .getStart()), translateDevicePointToCanvas(line.getEnd())));
        }
    }
}
