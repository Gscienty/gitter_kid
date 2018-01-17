package indi.gscienty.gitterkid.repo.nativelib;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface IRepositoryLibrary extends Library {
    /**
     * 加载libgkid.so动态链接库
     */
    IRepositoryLibrary Instance = (IRepositoryLibrary) Native.loadLibrary ("libgkid.so", IRepositoryLibrary.class);

    /**
     * 获取仓库路径
     * @param repositoryPointer 仓库
     * @return 仓库路径
     */
    String gitrepo_get_path (Pointer repositoryPointer);
    /**
     * 获取仓库名称
     * @param repositoryPointer 仓库
     * @return 仓库名称
     */
    String gitrepo_get_name (Pointer repositoryPointer);
    /**
     * 获取仓库中的分支集合
     * @param repositoryPointer 仓库
     * @return 分支集合
     */
    Pointer gitrepo_get_branches (Pointer repositoryPointer);
    
    /**
     * 析构分支集合
     * @param branchesPointer 分支集合
     */
    void gitbranches_dtor (Pointer branchesPointer);
    
    /**
     * 重置分支集合游标
     * @param branchesPointer 分支集合
     */
    void gitbranches_reset (Pointer branchesPointer);
    /**
     * 分支集合中是否具有下一分支
     * @param branchesPointer 分支集合
     * @return 是否具有下一分支
     */
    int gitbranches_hasnext (Pointer branchesPointer);
    /**
     * 
     * @param branchesPointer 获取下一个分支
     * @return 下一个分支
     */
    Pointer gitbranches_next (Pointer branchesPointer);
    
    /**
     * 获取分支名称
     * @param branchPointer 分支指针
     * @return 分支名称
     */
    String gitbranch_get_name (Pointer branchPointer);
    
    /**
     * 获取分支最后提交的commit签名
     * @param branchPointer 分支指针
     * @return commit签名
     */
    String gitbranch_get_lastcommit_sign (Pointer branchPointer);
}