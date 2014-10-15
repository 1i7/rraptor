package com.rraptor.pult.model;

import java.io.Serializable;

public class Point2D implements Serializable {

    private static final long serialVersionUID = 1L;

    private double x;
    private double y;

    public Point2D(double x, double y) {
        super();
        this.x = x;
        this.y = y;
    }

    @Override
    public boolean equals(Object o) {
        if (o instanceof Point2D) {
            final Point2D p2 = (Point2D) o;
            return (p2.getX() == x) && (p2.getY() == y);
        } else {
            return false;
        }
    }

    public double getX() {
        return x;
    }

    public double getY() {
        return y;
    }

    @Override
    public int hashCode() {
        return (int) (x + y);
    }

    public void setX(double x) {
        this.x = x;
    }

    public void setY(double y) {
        this.y = y;
    }
}