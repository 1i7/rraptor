package com.rraptor.pult.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.AttributeSet;

import com.rraptor.pult.core.DeviceControlService;
import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;
import com.rraptor.pult.model.Point3D;

public class PlotterAreaView extends WorkingArea2DView {
    public enum LineDrawingStatus {
        NORMAL, DRAWING_PROGRESS, DRAWING_ERROR, DRAWN
    }

    /**
     * Высота рабочей области по Z, мм
     */
    protected final int WORKING_AREA_Z_HEIGHT = 100;

    private DeviceControlService devControlService;

    private final Paint paint = new Paint();
    private final Path zAxisPath = new Path();

    // Точки в системе координат устройства
    private Point3D workingBlockPosition;

    // Точки в системе координат холста для рисования
    private Point3D canvasWorkingBlockPosition;

    public PlotterAreaView(final Context context, final AttributeSet attrs) {
        super(context, attrs, 30, 80, 30, 30);
    }

    @Override
    public void clearDrawing() {
        super.clearDrawing();
        this.invalidate();
    }

    public LineDrawingStatus getLineStatus(final Line2D line) {
        return devControlService == null ? LineDrawingStatus.NORMAL
                : devControlService.getDeviceDrawingManager().getLineStatus(
                        line);
    }

    /**
     * Посчитать радиус рабочего блока с учетом его "близости" к - чем выше блок
     * по оси z, тем больше радиус.
     * 
     * Вариант с логорифмическим масштабом - чем ниже блок по оси z, тем быстрее
     * меняется радиус (приятнее для визуализации работы двумерного плоттера,
     * т.к. у него рабочий блок гуляет не по всей оси z, а в меняет два
     * положения, оба близкие к поверхности стола).
     * 
     * @return
     */
    private int getWorkingBlockLogProjectionRadius(final double z) {
        int minRadius = 5;
        int maxRadius = 25;
        // прибавляем к z единичку, чтобы не уходить в отрицательные значения
        return (int) (minRadius + (maxRadius - minRadius) * Math.log(z + 1)
                / Math.log(WORKING_AREA_Z_HEIGHT));
    }

    /**
     * Посчитать радиус рабочего блока с учетом его "близости" к - чем выше блок
     * по оси z, тем больше радиус.
     * 
     * @return
     */
    private int getWorkingBlockProjectionRadius(final double z) {
        int minRadius = 5;
        int maxRadius = 25;
        return (int) (minRadius + (maxRadius - minRadius) * z
                / WORKING_AREA_Z_HEIGHT);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        translateDrawing();

        // дорисовать ось Z - справа от рабочей области
        paint.setColor(Color.BLACK);
        paint.setStrokeWidth(1);
        paint.setPathEffect(dashedLineEffect);
        paint.setStyle(Paint.Style.STROKE);
        // drawLine не работает с dashedLineEffect:
        // https://code.google.com/p/android/issues/detail?id=29944
        // canvas.drawLine((float) canvasP2.getX() + 65, (float)
        // canvasP2.getY(),
        // (float) canvasP3.getX() + 65, (float) canvasP3.getY(), paint);
        // вместо drawLine вызовем paintPath
        zAxisPath.reset();
        zAxisPath.moveTo((float) canvasP2.getX() + 65, (float) canvasP2.getY());
        zAxisPath.lineTo((float) canvasP3.getX() + 65, (float) canvasP3.getY());
        canvas.drawPath(zAxisPath, paint);

        // отметки на осях
        paint.setPathEffect(null);
        paint.setTextSize(20);
        canvas.drawText("0", (float) canvasP2.getX() + 71,
                (float) canvasP2.getY() + 20, paint);
        canvas.drawText("z", (float) canvasP3.getX() + 71,
                (float) canvasP3.getY() - 4, paint);

        // нарисовать текущее положение рабочего блока
        if (canvasWorkingBlockPosition != null) {
            paint.setColor(Color.BLUE);
            paint.setStrokeWidth(2);

            // на рабочей области XY
            // canvas.drawCircle(
            // (float) canvasWorkingBlockPosition.getX(),
            // (float) canvasWorkingBlockPosition.getY(),
            // getWorkingBlockProjectionRadius(workingBlockPosition.getZ()),
            // paint);
            canvas.drawCircle((float) canvasWorkingBlockPosition.getX(),
                    (float) canvasWorkingBlockPosition.getY(),
                    getWorkingBlockLogProjectionRadius(workingBlockPosition
                            .getZ()), paint);

            canvas.drawLine((float) canvasWorkingBlockPosition.getX() - 3,
                    (float) canvasWorkingBlockPosition.getY(),
                    (float) canvasWorkingBlockPosition.getX() + 3,
                    (float) canvasWorkingBlockPosition.getY(), paint);
            canvas.drawLine((float) canvasWorkingBlockPosition.getX(),
                    (float) canvasWorkingBlockPosition.getY() - 3,
                    (float) canvasWorkingBlockPosition.getX(),
                    (float) canvasWorkingBlockPosition.getY() + 3, paint);

            // по оси Z
            canvas.drawLine((float) canvasP2.getX() + 45,
                    (float) canvasWorkingBlockPosition.getZ(),
                    (float) canvasP2.getX() + 65,
                    (float) canvasWorkingBlockPosition.getZ(), paint);
        }
    }

    @Override
    protected void prepareDrawLine(final Line2D line, final Paint paint) {
        final LineDrawingStatus status = getLineStatus(line);
        switch (status) {
        case NORMAL:
            paint.setColor(Color.GRAY);
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
    }

    public void setDeviceControlService(
            final DeviceControlService devControlService) {
        this.devControlService = devControlService;
    }

    /**
     * Задать положение рабочего блока.
     * 
     * @param pos
     */
    public void setWorkingBlockPosition(final Point3D pos) {
        this.workingBlockPosition = pos;
        this.invalidate();
    }

    /**
     * Перевести координаты точки из системы координат устройства в систему
     * координат холста canvas: точка (0,0) устройства в левом нижнем углу,
     * точка (0,0) холста в правом верхнем углу. Рабочая область устройства
     * масштабируется до размеров холста.
     * 
     * Значение по оси Z переводится в значение по вертикали, параллельно оси Y,
     * но в собственном масштабе.
     * 
     * @return
     */
    protected Point3D translateDevicePointToCanvas(final Point3D devicePoint) {
        // по X и Y
        final Point2D canvasPoint = super
                .translateDevicePointToCanvas(devicePoint);

        // Z масштабируем отдельно от X и Y
        int canvasHeight = this.getHeight() - canvasIndentTop
                - canvasIndentBottom;

        float scaleFactorZ = (float) canvasHeight
                / (float) WORKING_AREA_Z_HEIGHT;
        int dz = (int) (canvasHeight - WORKING_AREA_Z_HEIGHT * scaleFactorZ)
                / 2 + canvasIndentBottom;

        final Point3D plotterPoint = new Point3D(
                canvasPoint.getX(),
                canvasPoint.getY(),
                // ось z нужно перевернуть вверх ногами
                (int) (this.getHeight() - (devicePoint.getZ() * scaleFactorZ + dz)));
        return plotterPoint;
    }

    private void translateDrawing() {
        // translate working area bounds
        if (workingBlockPosition != null) {
            canvasWorkingBlockPosition = translateDevicePointToCanvas(workingBlockPosition);
        } else {
            canvasWorkingBlockPosition = null;
        }
    }
}
