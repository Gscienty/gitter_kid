package indi.gscienty.gitterkid.services;

import indi.gscienty.gitterkid.services.repo.GitBranchesService;
import indi.gscienty.gitterkid.services.repo.GitCommitService;
import indi.gscienty.gitterkid.services.repo.GitRepositoriesService;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;;

public class CompareTest extends TestCase {
	
	public CompareTest(String name) {
		super(name);
	}
	
	public static Test suite() {
		return new TestSuite(CompareTest.class);
	}
	
	public void testCompare() {
		GitRepositoriesService reposService = new GitRepositoriesService("/home/ant");
		GitBranchesService bransService = new GitBranchesService(reposService.getRepository("gitterRepo"));
		
		
		GitCommitService commService = new GitCommitService(bransService.getLastCommit("master"));
		
		commService.compareHistory().forEach(item -> item.getGitBlobCompares().forEach(c -> {
			System.out.println(c.getBlobPath() + "\n" + c.getStatus() + "\tadd: " + c.getAddedLine() + "\tremove: " + c.getRemovedLine());
		}));
	}
}
