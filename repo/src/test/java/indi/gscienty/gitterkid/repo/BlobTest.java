package indi.gscienty.gitterkid.repo;

import java.io.UnsupportedEncodingException;

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
        Repository repository = null;

        for (Repository item : market) {
            if (item.getName().equals("repo")) {
                repository = item;
                break;
            }
        }
        
        GitBlob blob = new GitBlob(repository, "ce013625030ba8dba906f756967f9e9ca394464a");
        assertTrue(blob.getObjectType() == GitObjectType.Blob);
        try {
			assertTrue(new String(blob.getContent(), "ASCII").equals("hello\n"));
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
}