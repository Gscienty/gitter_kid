package indi.gscienty.gitterkid.repo;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class GitBranchTest extends TestCase {

	public GitBranchTest (String name) {
		super (name);
	}
	
	public static Test suite() {
		return new TestSuite(GitBranchTest.class);
	}
	
	public void testBranch() {
		Market market = new Market("/home/ant");
		Repository repository = market.first(repo -> repo.getName().equals("repo"));
		
		assertNull(repository.getBranches().first(b -> b.getName().equals("not_exist")));
		
		GitBranch branch = repository.getBranches().first(b -> b.getName().equals("master"));
		assertNotNull(branch);
		assertEquals(branch.getName(), "master");
		assertNotNull(branch.getLastCommit().getTree());
		System.out.println(branch.getLastCommit().getAuthor().getName());
		System.out.println(branch.getLastCommit().getAuthor().getMail());
		System.out.println(branch.getLastCommit().getAuthor().getTimestamp());
		branch.getLastCommit().getTree().forEach(item -> {
			System.out.println(item.getGitObjectType());
			System.out.println(item.getName());
			System.out.println(item.getSignture());
		});
	}
}
