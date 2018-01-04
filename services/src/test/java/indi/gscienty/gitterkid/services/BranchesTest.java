package indi.gscienty.gitterkid.services;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;;

public class BranchesTest extends TestCase {
	
	public BranchesTest(String name) {
		super(name);
	}
	
	public static Test suite() {
		return new TestSuite(BranchesTest.class);
	}
	
	public void testBranches() {
		GitRepositoriesService repos = new GitRepositoriesService("/home/ant");
		GitBranchesService brans = new GitBranchesService(repos.getRepository("repo"));
		
		brans.getCommits("master").forEach(commit -> {
			System.out.println(commit.getMessage());
		});
	}
}
