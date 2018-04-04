package indi.gscienty.gitterkid.webapi;

import indi.gscienty.gitterkid.repo.GitBranch;
import indi.gscienty.gitterkid.repo.Market;
import indi.gscienty.gitterkid.repo.Repository;
import indi.gscienty.gitterkid.services.repo.GitCommitService;
import indi.gscienty.gitterkid.webapi.services.RepositoryService;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

/**
 * Unit test for simple App.
 */
public class AppTest 
    extends TestCase
{
    /**
     * Create the test case
     *
     * @param testName name of the test case
     */
    public AppTest( String testName )
    {
        super( testName );
    }

    /**
     * @return the suite of tests being tested
     */
    public static Test suite()
    {
        return new TestSuite( AppTest.class );
    }

    
    public void testGetTree()
    {
    	RepositoryService service = new RepositoryService();
    	for (int i = 0; i < 65536; i++) {
        	Market market = service.getMarket("default");
        	Repository repo = market.first(r -> r.getName().equals("gitterRepo"));
        	GitBranch branch = repo.getBranches().first(b -> b.getName().equals("master"));
        	
        	GitCommitService commitService = new GitCommitService(branch.getLastCommit());
        	
        	Object obj = commitService.getTree("/front/scripts");
        	System.out.println(obj);
        	System.out.println(i);
    	}
    	System.out.println("END");
    }
}
