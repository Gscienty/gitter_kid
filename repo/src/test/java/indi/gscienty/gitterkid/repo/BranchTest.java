package indi.gscienty.gitterkid.repo;

import java.util.LinkedList;
import java.util.Queue;

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
	
	public void testBranch() {
		Market market = new Market("/home/ant");
		Repository repository = market.first(r -> r.getName().equals("gitterRepo"));
		GitBranch branch = repository.getBranches().first(b -> b.getName().equals("master"));
		
		Queue<GitCommit> waiting = new LinkedList<GitCommit>();
		waiting.add(branch.getLastCommit());
		
		while (waiting.isEmpty() == false) {
			GitCommit current = waiting.poll();
			System.out.println("Current: " + current.getSignture());
			current.getParents().forEach(parent -> {
				System.out.println("Parent: " + parent.getSignture());
				waiting.add(parent);
			});
			
			System.out.println();
		}
	}
}
