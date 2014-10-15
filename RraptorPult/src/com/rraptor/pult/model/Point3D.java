package com.rraptor.pult.model;

import java.io.Serializable;

public class Point3D extends Point2D implements Serializable {

    private static final long serialVersionUID = 1L;

    private double z;

    public Point3D(double x, double y, double z) {
        super(x, y);
        this.z = z;
    }

    @Override
    public boolean equals(Object o) {
        if (o instanceof Point3D) {
            final Point3D p2 = (Point3D) o;
            return super.equals(p2) && (p2.getZ() == z);
        } else {
            return false;
        }
    }

    public double getZ() {
        return z;
    }

    @Override
    public int hashCode() {
        return super.hashCode() + (int) z;
    }

    public void setZ(double z) {
        this.z = z;
    }
}