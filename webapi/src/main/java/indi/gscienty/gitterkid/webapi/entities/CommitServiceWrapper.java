package indi.gscienty.gitterkid.webapi.entities;

import indi.gscienty.gitterkid.services.GitCommitService;

public class CommitServiceWrapper {
	private GitCommitService service;
	
	public void setService(GitCommitService service) {
		this.service = service;
	}
	
	public GitCommitService getService() {
		return this.service;
	}
}
