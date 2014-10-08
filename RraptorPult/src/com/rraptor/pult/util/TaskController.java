package com.rraptor.pult.util;

public class TaskController {
	private int progress = 0;
	private boolean aborted = false;

	public int getProgress() {
		return progress;
	}

	public void setProgress(int progress) {
		this.progress = progress;
	}

	public boolean isAborted() {
		return aborted;
	}

	public void setAborted(boolean aborted) {
		this.aborted = aborted;
	}
}
