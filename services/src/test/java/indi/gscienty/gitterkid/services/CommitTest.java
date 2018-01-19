package indi.gscienty.gitterkid.services;

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
		
		commService.getTree("/a/b/c/d");
		System.out.println();
		commService.getTree("a/b/c/d");
		System.out.println();
		commService.getTree("/a/b/c/d/");
		System.out.println();
		commService.getTree("//a/b/../../c/d/");
		System.out.println();
		commService.getTree("/a/b//c/d/");
		System.out.println();
		commService.getTree("//a/b/c/d//");
	}
}
