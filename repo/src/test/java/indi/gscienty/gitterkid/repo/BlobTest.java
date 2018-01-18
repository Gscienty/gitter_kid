package indi.gscienty.gitterkid.repo;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

/**
 * BlobTest
 */
public class BlobTest extends TestCase {
    public BlobTest(String testName) {
        super (testName);
    }

    public static Test suite() {
        return new TestSuite(BlobTest.class);
    }

    public void testBlob() {
        Market market = new Market("/home/ant");
        assertNotNull (market);
        Repository repository = null;

        for (Repository item : market) {
            if (item.getName().equals("gitterRepo")) {
                repository = item;
                break;
            }
        }
        
        assertNotNull (repository);
        
        GitBlob blob = repository.getBlob("997cc5e85aff5cc35e317cd993f9979c858b17fa");
        
        System.out.println(new String (blob.getContent(), 0, blob.getLength()));
    }
}