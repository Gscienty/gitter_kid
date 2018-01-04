package indi.gscienty.gitterkid;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class GitTreeOrBlobTest extends TestCase{
  public GitTreeOrBlobTest(String testName){
    super(testName);
  }

  public static Test suite(){
    return new TestSuite(GitTreeOrBlobTest.class);
  }

  public void testBlob(){
    Market market = new Market("/home/m1231996c/repositories");
    Repository repository = market.first(repo -> repo.getName().equals("repo"));
    GitCommit commit = repository.getCommit("1f94aca4d8793849f52afb2255b55799d9803e5d");

    GitTreeOrBlobService service = new GitTreeOrBlobService(commit);

    assertTrue(new String(service.getBlobContent("/test/testfile"), "ASCII").equals("hello\n"));
  }
}
