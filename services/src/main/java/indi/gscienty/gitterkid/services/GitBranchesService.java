package indi.gscienty.gitterkid.services;

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
}
