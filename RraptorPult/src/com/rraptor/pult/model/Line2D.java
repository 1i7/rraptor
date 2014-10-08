package com.rraptor.pult.model;

import java.io.Serializable;

public class Line2D implements Serializable {

	private static final long serialVersionUID = 1L;

	private Point2D start;
	private Point2D end;

	public Line2D(Point2D start, Point2D end) {
		super();
		this.start = start;
		this.end = end;
	}

	@Override
	public boolean equals(Object o) {
		if (o instanceof Line2D) {
			final Line2D l2 = (Line2D) o;
			return (l2.getStart().equals(getStart()) && (l2.getEnd()
					.equals(getEnd())));
		} else {
			return false;
		}
	}

	public Point2D getEnd() {
		return end;
	}

	public Point2D getStart() {
		return start;
	}

	@Override
	public int hashCode() {
		return start.hashCode() + end.hashCode();
	}

	public void setEnd(Point2D end) {
		this.end = end;
	}

	public void setStart(Point2D start) {
		this.start = start;
	}
}
