package com.rraptor.pult.model;

import java.io.Serializable;

public class Point2D implements Serializable {

	private static final long serialVersionUID = 1L;

	private int x;
	private int y;

	public Point2D(int x, int y) {
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

	public int getX() {
		return x;
	}

	public int getY() {
		return y;
	}

	@Override
	public int hashCode() {
		return x + y;
	}

	public void setX(int x) {
		this.x = x;
	}

	public void setY(int y) {
		this.y = y;
	}
}