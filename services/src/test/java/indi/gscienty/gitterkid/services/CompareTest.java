package indi.gscienty.gitterkid.services;

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
		GitRepositoriesService repos = new GitRepositoriesService("/home/ant");
		GitBranchesService brans = new GitBranchesService(repos.getRepository("gitterRepo"));
		
		brans.compareCommitHistory(brans.getLastCommit("master")).forEach(item -> {
			item.getGitBlobCompares().forEach(c -> {
				System.out.println(c.getBlobPath());
				System.out.println(c.getStatus());
				System.out.println(c.getAddedLine());
				System.out.println(c.getRemovedLine());
			});
		});
	}
}
