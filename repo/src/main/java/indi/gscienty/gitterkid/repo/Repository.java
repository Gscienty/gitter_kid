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
    
    /**
     * 构造方法
     * @param repositoryPointer 仓库路径
     */
    public Repository(Pointer repositoryPointer) {
        this.handle = repositoryPointer;
        this.lib = IRepositoryLibrary.Instance;
    }

    /**
     * 获取仓库路径
     * @return 仓库路径
     */
    public String getPath() {
        return this.lib.git_repo_path(this.handle);
    }

    /**
     * 获取仓库名称
     * @return 仓库名称
     */
    public String getName() {
        return this.lib.git_repo_name(this.handle);
    }
    
    /**
     * 获取仓库指针
     * @return 仓库指针
     */
    public Pointer getHandle() {
        return this.handle;
    }
    
    /**
     * 获取当前仓库内的Commit
     * @param signture Commit签名
     * @return Commit实体
     */
    public GitCommit getCommit(String signture) {
    	return new GitCommit(this, signture);
    }
    
    /**
     * 获取当前仓库内的Blob
     * @param signture Blob签名
     * @return Blob实体
     */
    public GitBlob getBlob(String signture) {
    	return new GitBlob(this, signture);
    }
    
    /**
     * 获取当前仓库的Tree
     * @param signture Tree签名
     * @return Tree实体
     */
    public GitTree getTree(String signture) {
    	return new GitTree(this, signture);
    }
}