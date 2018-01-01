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
		Market market = new Market("/home/ant");
		
		Repository repository = market.first(repo -> repo.getName().equals("repo"));
		GitCommit commit = repository.getCommit("6516615f6f72011317b85b0726ce80dbe3fe2f82");
		assertNotNull(commit);
		assertEquals(commit.getMessage(), "world\n");
	}
	
	public void testPersonLog() {
		Market market = new Market("/home/ant/");
		Repository repository = market.first(repo -> repo.getName().equals("repo"));
		GitCommit commit = repository.getCommit("6516615f6f72011317b85b0726ce80dbe3fe2f82");
		assertEquals(commit.getAuthor().getName(), "Gscienty");
		assertEquals(commit.getCommitter().getName(), "Gscienty");
	}
}
