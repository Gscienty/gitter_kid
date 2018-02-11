package indi.gscienty.gitterkid.webapi.entities;

import indi.gscienty.gitterkid.repo.Repository;

public class RepositoryWrapper {
	private Repository repository;
	
	public void setRepository(Repository repository) {
		this.repository = repository;
	}
	
	public Repository getRepository() {
		return this.repository;
	}
	
	public boolean isExist() {
		return this.repository != null;
	}
}
