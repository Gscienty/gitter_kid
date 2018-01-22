package indi.gscienty.gitterkid.services;

import java.util.List;
import java.util.Vector;

public class GitCommitCompareResult {
	private List<GitBlobCompareResult> blobCompares;
	
	public GitCommitCompareResult () {
		this.blobCompares = new Vector<> ();
	}
	
	public void appendCompareBlobResult (GitBlobCompareResult blobCompareResult) {
		this.blobCompares.add (blobCompareResult);
	}
	
	public List<GitBlobCompareResult> getGitBlobCompares () {
		return this.blobCompares;
	}
}
