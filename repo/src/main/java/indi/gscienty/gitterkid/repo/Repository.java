package indi.gscienty.gitterkid.repo;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.nativelib.IRepositoryLibrary;

/**
 * 仓库实体
 * @author ant
 *
 */
public class Repository {
    private Pointer handle;
    private IRepositoryLibrary lib;
    private GitBranches branches;
    private Market storeMarket;
    
    /**
     * 构造方法
     * @param repositoryPointer 仓库路径
     */
    public Repository (Market market, Pointer repositoryPointer) {
    	this.storeMarket = market;
        this.handle = repositoryPointer;
        this.lib = IRepositoryLibrary.Instance;
        this.branches = null;
    }
    
    public boolean isLegal() {
    	return this.handle.equals(Pointer.NULL);
    }

    /**
     * 获取仓库路径
     * @return 仓库路径
     */
    public String getPath () {
        return this.lib.gitrepo_get_path (this.handle);
    }

    /**
     * 获取仓库名称
     * @return 仓库名称
     */
    public String getName () {
        return this.lib.gitrepo_get_name (this.handle);
    }
    
    /**
     * 获取仓库指针
     * @return 仓库指针
     */
    public Pointer getHandle () {
        return this.handle;
    }
    
    /**
     * 获取当前仓库内的Commit
     * @param signture Commit签名
     * @return Commit实体
     */
    public GitCommit getCommit (String signture) {
    	return new GitCommit (this, signture);
    }
    
    /**
     * 获取当前仓库内的Blob
     * @param signture Blob签名
     * @return Blob实体
     */
    public GitBlob getBlob (String signture) {
    	return new GitBlob (this, signture);
    }
    
    /**
     * 获取当前仓库的Tree
     * @param signture Tree签名
     * @return Tree实体
     */
    public GitTree getTree (String signture) {
    	return new GitTree (this, signture);
    }
    
    /**
     * 获取当前仓库的Branches
     * @return Branches
     */
    public GitBranches getBranches () {
    	if (this.branches == null) {
    		this.branches = new GitBranches (this, this.lib.gitrepo_get_branches (this.handle));
    	}
    	return this.branches;
    }
    
    public Market getBaseMarket () {
    	return this.storeMarket;
    }
    
}