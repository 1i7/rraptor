package com.rraptor.pult.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.MotionEvent;

import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;

public class VectorDrawing2DView extends WorkingArea2DView {

    private final Paint paint = new Paint();

    private Point2D userPoint1 = null;

    public VectorDrawing2DView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        // нарисовать точку, указанную пользователем
        if (userPoint1 != null) {
            paint.setColor(Color.GRAY);
            canvas.drawCircle((float) userPoint1.getX(),
                    (float) userPoint1.getY(), 5, paint);
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
                drawingLines.add(new Line2D(
                        translateCanvasPointToDevice(userPoint1),
                        translateCanvasPointToDevice((point))));
                userPoint1 = null;
            }
            this.invalidate();
        }
        return super.onTouchEvent(event);
    }

    @Override
    public void setEnabled(boolean enabled) {
        this.userPoint1 = null;
        super.setEnabled(enabled);
    }
}
