package indi.gscienty.gitterkid.repo;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class TreeTest extends TestCase {
	public TreeTest (String name) {
		super (name);
	}
	
	public static Test suite () {
		return new TestSuite(TreeTest.class);
	}
	
	public void testTree() {
		Market market = new Market("/home/ant");
		Repository repo = market.first(r -> r.getName().equals("gitterRepo"));
		GitBranch branch = repo.getBranches().first(b -> b.getName().equals("master"));
		GitTree tree = branch.getLastCommit().getTree();
		tree.forEach(item -> System.out.println(item.getName()));
	}
}
