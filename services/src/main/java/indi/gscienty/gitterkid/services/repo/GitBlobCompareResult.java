package indi.gscienty.gitterkid.services.repo;

import indi.gscienty.gitterkid.services.types.GitBlobCompareStatus;

public class GitBlobCompareResult {
	private int addedLine;
	private int removedLine;
	private String blobPath;
	private GitBlobCompareStatus status;
	
	public GitBlobCompareResult() {
		this.addedLine = 0;
		this.removedLine = 0;
		this.blobPath = "";
		this.status = GitBlobCompareStatus.Unknow;
	}
	
	public void setAddedLine(int value) {
		this.addedLine = value;
	}
	
	public void setRemovedLine(int value) {
		this.removedLine = value;
	}
	
	public void accumulationAddedLine(int value) {
		this.addedLine += value;
	}
	
	public void accumulationRemovedLine(int value) {
		this.removedLine += value;
	}
	
	public int getAddedLine() {
		return this.addedLine;
	}
	
	public int getRemovedLine() {
		return this.removedLine;
	}
	
	public void setBlobPath(String path) {
		this.blobPath = path;
	}
	
	public String getBlobPath() {
		return this.blobPath;
	}
	
	public void setStatus(GitBlobCompareStatus status) {
		this.status = status;
	}
	
	public GitBlobCompareStatus getStatus() {
		return this.status;
	}
}
