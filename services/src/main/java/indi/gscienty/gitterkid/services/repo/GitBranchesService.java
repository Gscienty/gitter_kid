package indi.gscienty.gitterkid.services.repo;

import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.Vector;

import indi.gscienty.gitterkid.repo.GitBranch;
import indi.gscienty.gitterkid.repo.GitCommit;
import indi.gscienty.gitterkid.repo.Repository;

public class GitBranchesService {
	private Repository repository;
	
	public GitBranchesService(Repository repository) {
		this.repository = repository;
	}
	
	public void setRepository(Repository repository) {
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
		GitBranch branch = this.repository.getBranches().get(branchName);
		
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
		GitBranch branch = this.repository.getBranches().get(branchName);
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
}
