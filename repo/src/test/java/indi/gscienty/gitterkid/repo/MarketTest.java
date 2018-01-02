package indi.gscienty.gitterkid.repo;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

/**
 * Unit test for simple App.
 */
public class MarketTest 
    extends TestCase
{
    /**
     * Create the test case
     *
     * @param testName name of the test case
     */
    public MarketTest( String testName )
    {
        super( testName );
    }

    /**
     * @return the suite of tests being tested
     */
    public static Test suite()
    {
        return new TestSuite( MarketTest.class );
    }
    
    public void testQueryable() {
    	Market market = new Market("/home/m1231996c/repositories");
    	assertTrue(market.any(repository -> repository.getName().equals("repo")));
    	assertFalse(market.all(repository -> repository.getName().equals("not_exist")));
    	assertNotNull(market.first(repo -> repo.getName().equals("repo")));
    }
}
