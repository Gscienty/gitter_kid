package indi.gscienty.gitterkid.repo;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class BranchTest extends TestCase {
	public BranchTest (String name) {
		super (name);
	}
	
	public static Test suite() {
		return new TestSuite(BranchTest.class);
	}
	
	public void testBranch () {
		Market market = new Market ("/home/ant");
		Repository repository = market.first (r -> r.getName ().equals ("gitterRepo"));
		repository.getBranches ().forEach (branch -> System.out.println(branch.getName()));
	}
	
	public void testBranchCommit () {
		Market market = new Market ("/home/ant");
		Repository repository = market.first(r -> r.getName().equals("gitterRepo"));
		GitBranch branch = repository.getBranches().first(b -> b.getName().equals("master"));
		
		System.out.println(branch.getLastCommit());
	}
}
