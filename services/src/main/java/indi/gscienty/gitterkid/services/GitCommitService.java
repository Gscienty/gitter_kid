package indi.gscienty.gitterkid.services;

import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Vector;
import java.util.logging.Logger;

import indi.gscienty.gitterkid.repo.GitBlob;
import indi.gscienty.gitterkid.repo.GitCommit;
import indi.gscienty.gitterkid.repo.GitObjectType;
import indi.gscienty.gitterkid.repo.GitTree;
import indi.gscienty.gitterkid.services.types.GitBlobCompareStatus;

public class GitCommitService {
	private static Logger logger = Logger.getLogger(GitCommitService.class.getName()); 
	
	private GitCommit commit;
	
	public GitCommitService (GitCommit commit) {
		this.commit = commit;
	}
	
	public GitCommit getCommit () {
		return this.commit;
	}
	
	public GitTree getTree (String path) {
		GitTree result = this.commit.getTree();

		for (String val : this.splitPath(path)) {
			GitTree.Item item = result.first(t -> t.getName().equals(val));
			if (item == null || item.getGitObjectType() != GitObjectType.Tree) {
				logger.warning(path + " not exist");
				return null;
			}
			result = ((GitTree.TreeItem) item).getTree();
		}
		
		return result;
	}
	
	public GitBlob getBlob (String path) {
		GitTree midTree = this.commit.getTree();
		
		for (String val : this.splitPath(path)) {
			GitTree.Item item = midTree.first(t -> t.getName().equals(val));
			if (item == null) {
				logger.warning(path + " not exist");
				return null;
			}
			else if (item.getGitObjectType() == GitObjectType.Tree) {
				midTree = ((GitTree.TreeItem) item).getTree();
			}
			else {
				return ((GitTree.BlobItem) item).getBlob();
			}
		}
		
		logger.warning(path + " is not a blob");
		return null;
	}

	private List<String> splitPath (String path) {
		List<String> result = new Vector<> ();
		
		String[] splitedPath = path.split("/");
		for (String val : splitedPath) {
			if (val.isEmpty()) continue;
			if (val.equals(".")) continue;
			if (val.equals("..")) {
				if (result.isEmpty()) {
					return result;
				}
				else {
					result.remove(result.size() - 1);
				}
			}
			else {
				result.add(val);
			}
		}
		
		return result;
	}
	
	/**
	 * 比对当前Commit与上游Commit的区别
	 * @param commit 当前Commit
	 * @return 比对结果
	 */
	public List<GitCommitCompareResult> compareHistory () {
		List<GitCommitCompareResult> result = new Vector<> ();
		// 遍历上游Commit
		this.commit.getParents ().forEach (parentCommit -> {
			GitCommitCompareResult compareResult = new GitCommitCompareResult ();
			Map<String, GitBlob> originBlobs = this.getGitBlobs (parentCommit);
			Map<String, GitBlob> currentBlobs = this.getGitBlobs (this.commit);
			
			// 遍历上游Commit中所有的Blob是否在当前Commit中仍然存在
			originBlobs.forEach ((path, blob) -> {
				if (currentBlobs.containsKey (path) == false) {
					// 下游Commit中不存在该Blob
					GitBlobCompareResult compareBlobResult = new GitBlobCompareResult ();
					// 将比对结果更改为“删除文件”
					compareBlobResult.setStatus (GitBlobCompareStatus.Deleted);
					// 将当前文件的行数赋值给比对结果中的删除行数
					compareBlobResult.setRemovedLine (GitBlobCompare.getLinesCount (blob));
					// 保存当前文件的路径
					compareBlobResult.setBlobPath (path);
					
					compareResult.appendCompareBlobResult (compareBlobResult);
				}
			});
			
			// 遍历当前Commit中所有的Blob，用以获取“修改文件”和“添加文件”状态的Blob比对结果
			currentBlobs.forEach ((path, blob) -> {
				if (originBlobs.containsKey (path) == false) {
					// 上游Commit中不存在该Blob
					GitBlobCompareResult compareBlobResult = new GitBlobCompareResult ();
					// 将比对结果更改为“添加文件”
					compareBlobResult.setStatus (GitBlobCompareStatus.Added);
					// 将当前文件的行数赋值给比对结果中添加行数
					compareBlobResult.setAddedLine (GitBlobCompare.getLinesCount (blob));
					// 保存当前文件的路径
					compareBlobResult.setBlobPath (path);
					
					compareResult.appendCompareBlobResult (compareBlobResult);
				}
				else {
					// 获取上游Commit中与当前Commit中相同路径的Blob
					GitBlob originBlob = originBlobs.get (path);
					// 比对两个Blob，获取增删行数
					GitBlobCompareResult compareBlobResult = GitBlobCompare.compare (blob, originBlob);
					// 如果当前比对结果表明Blob没有改变，则跳过
					if (compareBlobResult.getAddedLine () == 0 && compareBlobResult.getRemovedLine () == 0) {
						return;
					}
					// 将比对结果设置为“修改文件”
					compareBlobResult.setStatus (GitBlobCompareStatus.Modified);
					// 保存当前文件的路径
					compareBlobResult.setBlobPath (path);
					
					compareResult.appendCompareBlobResult (compareBlobResult);
				}
			});
			
			result.add (compareResult);
		});
		
		return result;
	}
	

	/**
	 * 获取Commit关联的项目中所有的Blob
	 * @param commit Commit对象
	 * @return 返回Blob字典，Key值为Blob的路径
	 */
	private Map<String, GitBlob> getGitBlobs (GitCommit commit) {
		class Tuple {
			String path;
			GitTree tree;
			public Tuple (String path, GitTree tree) {
				this.path = path;
				this.tree = tree;
			}
			
			public String getPath () {
				return this.path;
			}
			public GitTree getTree () {
				return this.tree;
			}
		}
		
		Map<String, GitBlob> result = new Hashtable<> ();
		
		Queue<Tuple> waitingWalk = new LinkedList<> ();
		// 添加根目录
		waitingWalk.add (new Tuple("/", commit.getTree ()));
		
		while (waitingWalk.isEmpty () == false) {
			Tuple currentTuple = waitingWalk.poll ();
			// 获取等待遍历的Tree
			currentTuple.getTree ().forEach (item -> {
					if (item.getGitObjectType () == GitObjectType.Tree) {
						// 如果包含子文件夹，则将该子文件夹添加进待遍历队列
						waitingWalk.add (new Tuple (
								currentTuple.getPath () + item.getName () + "/",
								((GitTree.TreeItem) item).getTree ()));
					}
					else {
						// 如果为Blob，则将该文件添加进返回集合
						result.put (
								currentTuple.getPath () + item.getName (),
								((GitTree.BlobItem) item).getBlob ());
					}
				});
		}
		
		return result;
	}
}
