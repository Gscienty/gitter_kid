package indi.gscienty.gitterkid.repo;

import indi.gscienty.gitterkid.repo.entity.Repository;
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

    /**
     * Rigourous Test :-)
     */
    public void testApp()
    {
        Market market = new Market("/home");

        for (Repository item : market) {
            System.out.println(item.getName());
            System.out.println();
        }
    }
}
