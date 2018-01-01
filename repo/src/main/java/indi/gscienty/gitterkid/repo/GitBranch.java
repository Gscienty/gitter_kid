package indi.gscienty.gitterkid.repo;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.nativelib.IRepositoryLibrary;

public class GitBranch {
	private Pointer handle;
	private IRepositoryLibrary lib;
	private Repository repository;
	
	public GitBranch(Repository repository, Pointer handle) {
		this.handle = handle;
		this.repository = repository;
		this.lib = IRepositoryLibrary.Instance;
	}
	
	public String getName() {
		return this.lib.git_branch_get_name(this.handle);
	}
	
	public String getLastCommitSignture() {
		return this.lib.git_branch_get_last_commit_sign(this.handle);
	}
	
	public GitCommit getLastCommit() {
		return new GitCommit(
			this.repository,
			this.getLastCommitSignture()
		);
	}
}
