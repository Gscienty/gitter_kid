package indi.gscienty.gitterkid.services;

import java.util.List;
import java.util.regex;

import java.gscienty.gitterkid.repo.GitBlob;
import java.gscienty.gitterkid.repo.GitTree;
import java.gscienty.gitterkid.repo.GitCommit;

public class GitTreeOrBlobService{
  private GitCommit commit;

  public GitTreeOrBlobService(GitCommit commit){
    this.commit = commit;
  }

  public List<String> getTreeList(String path){
    GitTree tree = getLastTree(path);
    return tree.filter(item -> item.getName());
  }

  public byte[] getBlobContent(String path){
    String blobname = path.substring(path.lastIndexOf('/') + 1, path.length() - 1);
    GitTree tree = getLastTree(path.substring(0, path.lastIndexOf('/')));
    BlobItem it = (BlobItem)tree.first(item -> item.getName().equals(blobname));
    GitBlob blob = it.getBlob();
    return blob.getContent();
  }

  private GitTree getLastTree(String path){
    GitTree tree = this.commit.getTree();
    if (path[0] != '/') { path = '/' + path; }
    if (path[path.length() - 1] != '/') { path = path + '/' }
    String str = getTreeName(path);
    while (str != ""){
      TreeItem it = (TreeItem)tree.first(item -> item.getName().equals(str));
      tree = it.getTree();
      path = path.replace(str,"");
      str = getTreeName(path);
    }
    return tree;
  }

  private String getTreeName(String path){
    Pattern p = Pattern.compile("/[^/]+/");
    Matcher m = m.matcher(path);
    path = m.group().ToString();
    return path.substring(2, path.length()-1);
  }
}
