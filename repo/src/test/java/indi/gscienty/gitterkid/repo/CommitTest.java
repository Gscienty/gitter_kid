package indi.gscienty.gitterkid.repo;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class CommitTest extends TestCase {
	public CommitTest (String name) {
		super (name);
	}
	
	public static Test suite() {
		return new TestSuite(CommitTest.class);
	}
	
	public void testCommit() {
		Market market = new Market("/home/m1231996c/repositories");
		
		Repository repository = market.first(repo -> repo.getName().equals("repo"));
		GitCommit commit = repository.getCommit("1f94aca4d8793849f52afb2255b55799d9803e5d");
		assertNotNull(commit);
		assertEquals(commit.getMessage(), "test1\n");
	}
	
	public void testPersonLog() {
		Market market = new Market("/home/m1231996c/repositories");
		Repository repository = market.first(repo -> repo.getName().equals("repo"));
		GitCommit commit = repository.getCommit("1f94aca4d8793849f52afb2255b55799d9803e5d");
		assertEquals(commit.getAuthor().getName(), "mfs");
		assertEquals(commit.getCommitter().getName(), "mfs");
	}
}
