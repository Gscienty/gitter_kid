package indi.gscienty.gitterkid.repo;

import java.util.logging.Logger;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.nativelib.IRepositoryLibrary;

public class GitBranch {
	private static Logger logger = Logger.getLogger(GitBranch.class.getName());
	
	private Pointer handle;
	private IRepositoryLibrary lib;
	private Repository repository;
	private GitCommit lastCommit;
	private String lastCommitSignture;
	
	public GitBranch (Repository repository, Pointer handle) {
		this.handle = handle;
		this.repository = repository;
		this.lib = IRepositoryLibrary.Instance;
		this.lastCommit = null;
		this.lastCommitSignture = null;
	}
	
	public String getName () {
		return this.lib.gitbranch_get_name (this.handle);
	}
	
	public String getLastCommitSignture () {
		return this.lib.gitbranch_get_lastcommit_sign (this.handle);
	}
	
	public GitCommit getLastCommit () {
		String currentCommitSignture = this.getLastCommitSignture ();
		
		logger.info(currentCommitSignture);
		
		if (this.lastCommit == null) {
			this.lastCommitSignture = currentCommitSignture;
			this.lastCommit = new GitCommit (this.repository, this.lastCommitSignture);
		}
		else if (currentCommitSignture.equals (this.lastCommitSignture)) {
			this.lastCommitSignture = currentCommitSignture;
			this.lastCommit = new GitCommit (this.repository, this.lastCommitSignture);
		}
		
		return this.lastCommit;
	}
}
