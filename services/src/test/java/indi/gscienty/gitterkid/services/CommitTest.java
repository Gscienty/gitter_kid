package indi.gscienty.gitterkid.services;

import indi.gscienty.gitterkid.services.repo.GitBranchesService;
import indi.gscienty.gitterkid.services.repo.GitCommitService;
import indi.gscienty.gitterkid.services.repo.GitRepositoriesService;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class CommitTest extends TestCase {
	public CommitTest(String name) {
		super (name);
	}
	
	public static Test suite() {
		return new TestSuite(CommitTest.class);
	}
	
	public void testGetSubTree() {
		GitRepositoriesService reposService = new GitRepositoriesService("/home/ant");
		GitBranchesService bransService = new GitBranchesService(reposService.getRepository("gitterRepo"));
		
		
		GitCommitService commService = new GitCommitService(bransService.getLastCommit("master"));
		
		assertNotNull(commService);
	}
}
