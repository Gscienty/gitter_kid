package indi.gscienty.gitterkid.services;

import indi.gscienty.gitterkid.services.repo.GitBranchesService;
import indi.gscienty.gitterkid.services.repo.GitCommitService;
import indi.gscienty.gitterkid.services.repo.GitRepositoriesService;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class TreeTest extends TestCase {
	public TreeTest(String name) {
		super(name);
	}
	
	public static Test suite() {
		return new TestSuite(TreeTest.class);
	}
	
	public void testTree() {
		GitRepositoriesService reposService = new GitRepositoriesService("/home/ant");
		GitBranchesService bransService = new GitBranchesService(reposService.getRepository("gitterRepo"));
		
		
		GitCommitService commService = new GitCommitService(bransService.getLastCommit("master"));

		commService.getTree("/");
		
	}
	
	public void testNewestMessage() {
		//GitRepositoriesService reposService = new GitRepositoriesService("/home/ant");
		//GitBranchesService bransService = new GitBranchesService(reposService.getRepository("gitterRepo"));
		
		
		//GitCommitService commService = new GitCommitService(bransService.getLastCommit("master"));
	}
}
