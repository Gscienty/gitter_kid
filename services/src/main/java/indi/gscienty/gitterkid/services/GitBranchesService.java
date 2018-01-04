package indi.gscienty.gitterkid.services;

import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Vector;

import indi.gscienty.gitterkid.repo.GitBlob;
import indi.gscienty.gitterkid.repo.GitBranch;
import indi.gscienty.gitterkid.repo.GitCommit;
import indi.gscienty.gitterkid.repo.GitObjectType;
import indi.gscienty.gitterkid.repo.GitTree;
import indi.gscienty.gitterkid.repo.Repository;
import indi.gscienty.gitterkid.services.types.GitBlobCompareStatus;

public class GitBranchesService {
	private Repository repository;
	
	public GitBranchesService(Repository repository) {
		this.repository = repository;
	}
	
	/**
	 * 获取与该分支关联的所有Commit
	 * @param branchName 分支名称
	 * @return Commit集合
	 */
	public List<GitCommit> getCommits(String branchName) {
		List<GitCommit> result = new Vector<>();
		// 通过分支名称获取分支
		GitBranch branch = this.repository
				.getBranches()
				.first(b -> b.getName().equals(branchName));
		
		if (branch != null) {
			Queue<GitCommit> enumQueue = new LinkedList<>();
			enumQueue.add(branch.getLastCommit());
			
			while (enumQueue.isEmpty() == false) {
				GitCommit currentCommit = enumQueue.poll();
				if (currentCommit == null) {
					break;
				}
				result.add(currentCommit);
				
				currentCommit.getParents().forEach(parent -> {
					if (parent != null) {
						enumQueue.add(parent);
					}
				});
			}
		}
		
		return result;
	}
	
	/**
	 * 获取指定分支下最新的Commit
	 * @param branchName 分支名称
	 * @return Commit
	 */
	public GitCommit getLastCommit(String branchName) {
		GitBranch branch = this.repository
				.getBranches()
				.first(b -> b.getName().equals(branchName));
		if (branch == null) {
			return null;
		}
		return branch.getLastCommit();
	}
	
	/**
	 * 获取所有分支名称
	 * @return 分支名称集合
	 */
	public List<String> getBranchesName() {
		return this.repository.getBranches().filter(branch -> branch.getName());
	}
	
	/**
	 * 比对当前Commit与上游Commit的区别
	 * @param commit 当前Commit
	 * @return 比对结果
	 */
	public List<GitCommitCompareResult> compareCommitHistory(GitCommit commit) {
		List<GitCommitCompareResult> result = new Vector<>();
		// 遍历上游Commit
		commit.getParents().forEach(parentCommit -> {
			GitCommitCompareResult compareResult = new GitCommitCompareResult();
			Map<String, GitBlob> originBlobs = this.getGitBlobs(parentCommit);
			Map<String, GitBlob> currentBlobs = this.getGitBlobs(commit);
			
			// 遍历上游Commit中所有的Blob是否在当前Commit中仍然存在
			originBlobs.forEach((path, blob) -> {
				if (currentBlobs.containsKey(path) == false) {
					// 下游Commit中不存在该Blob
					GitBlobCompareResult compareBlobResult = new GitBlobCompareResult();
					// 将比对结果更改为“删除文件”
					compareBlobResult.setStatus(GitBlobCompareStatus.Deleted);
					// 将当前文件的行数赋值给比对结果中的删除行数
					compareBlobResult.setRemovedLine(GitBlobCompare.getLinesCount(blob));
					// 保存当前文件的路径
					compareBlobResult.setBlobPath(path);
					
					compareResult.appendCompareBlobResult(compareBlobResult);
				}
			});
			
			// 遍历当前Commit中所有的Blob，用以获取“修改文件”和“添加文件”状态的Blob比对结果
			currentBlobs.forEach((path, blob) -> {
				if (originBlobs.containsKey(path) == false) {
					// 上游Commit中不存在该Blob
					GitBlobCompareResult compareBlobResult = new GitBlobCompareResult();
					// 将比对结果更改为“添加文件”
					compareBlobResult.setStatus(GitBlobCompareStatus.Added);
					// 将当前文件的行数赋值给比对结果中添加行数
					compareBlobResult.setAddedLine(GitBlobCompare.getLinesCount(blob));
					// 保存当前文件的路径
					compareBlobResult.setBlobPath(path);
					
					compareResult.appendCompareBlobResult(compareBlobResult);
				}
				else {
					// 获取上游Commit中与当前Commit中相同路径的Blob
					GitBlob originBlob = originBlobs.get(path);
					// 比对两个Blob，获取增删行数
					GitBlobCompareResult compareBlobResult = GitBlobCompare.compare(blob, originBlob);
					// 如果当前比对结果表明Blob没有改变，则跳过
					if (compareBlobResult.getAddedLine() == 0 && compareBlobResult.getRemovedLine() == 0) {
						return;
					}
					// 将比对结果设置为“修改文件”
					compareBlobResult.setStatus(GitBlobCompareStatus.Modified);
					// 保存当前文件的路径
					compareBlobResult.setBlobPath(path);
					
					compareResult.appendCompareBlobResult(compareBlobResult);
				}
			});
			
			result.add(compareResult);
		});
		
		return result;
	}
	
	/**
	 * 获取Commit关联的项目中所有的Blob
	 * @param commit Commit对象
	 * @return 返回Blob字典，Key值为Blob的路径
	 */
	private Map<String, GitBlob> getGitBlobs(GitCommit commit) {
		class Tuple {
			String path;
			GitTree tree;
			public Tuple(String path, GitTree tree) {
				this.path = path;
				this.tree = tree;
			}
			
			public String getPath() {
				return this.path;
			}
			public GitTree getTree() {
				return this.tree;
			}
		}
		
		Map<String, GitBlob> result = new Hashtable<>();
		
		Queue<Tuple> waitingWalk = new LinkedList<>();
		// 添加根目录
		waitingWalk.add(new Tuple("/", commit.getTree()));
		
		while (waitingWalk.isEmpty() == false) {
			Tuple currentTuple = waitingWalk.poll();
			// 获取等待遍历的Tree
			currentTuple.getTree().forEach(item -> {
					if (item.getGitObjectType() == GitObjectType.Tree) {
						// 如果包含子文件夹，则将该子文件夹添加进待遍历队列
						waitingWalk.add(new Tuple(
								currentTuple.getPath() + item.getName() + "/",
								((GitTree.TreeItem) item).getTree()));
					}
					else {
						// 如果为Blob，则将该文件添加进返回集合
						result.put(currentTuple.getPath() + item.getName(),
								((GitTree.BlobItem) item).getBlob());
					}
				});
		}
		
		return result;
	}
	
	
}
