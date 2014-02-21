package com.rraptor.pult.model;

public class Line2D {
	private Point2D start;
	private Point2D end;

	public Line2D(Point2D start, Point2D end) {
		super();
		this.start = start;
		this.end = end;
	}

	public Point2D getEnd() {
		return end;
	}

	public Point2D getStart() {
		return start;
	}

	public void setEnd(Point2D end) {
		this.end = end;
	}

	public void setStart(Point2D start) {
		this.start = start;
	}

}
