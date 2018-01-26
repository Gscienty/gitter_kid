package indi.gscienty.gitterkid.services;

import indi.gscienty.gitterkid.repo.GitTree;
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

		GitTree tree = commService.getTree("/services/src/main/java/indi/gscienty/gitterkid/services/");
		
		tree.forEach(i -> System.out.println(i.getName()));
		
	}
}
