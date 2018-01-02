package indi.gscienty.gitterkid.services;

import java.util.List;

import indi.gscienty.gitterkid.repo.Market;
import indi.gscienty.gitterkid.repo.Repository;

public class GitRepositoriesService {
	private Market market;
	
	public GitRepositoriesService(String path) {
		this.market = new Market(path);
	}
	
	/**
	 * 获取当前仓库市场内所有仓库名称
	 * @return 仓库名称集合
	 */
	public List<String> getRepositoriesName() {
		return this.market.filter(repository -> repository.getName());
	}
	
	/**
	 * 根据仓库名称获取仓库。当仓库不存在时返回null
	 * @param name 仓库名称
	 * @return 仓库
	 */
	public Repository getRepository(String name) {
		return this.market.first(repository -> repository.getName().equals(name));
	}
}
