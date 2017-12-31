package indi.gscienty.gitterkid.repo.nativelib;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface IRepositoryLibrary extends Library {
    /**
     * 加载libgkid.so动态链接库
     */
    IRepositoryLibrary Instance = (IRepositoryLibrary) Native.loadLibrary("libgkid.so", IRepositoryLibrary.class);

    /**
     * 获取仓库路径
     * @param repositoryPointer 仓库
     * @return 仓库路径
     */
    String git_repo_path(Pointer repositoryPointer);
    /**
     * 获取仓库名称
     * @param repositoryPointer 仓库
     * @return 仓库名称
     */
    String git_repo_name(Pointer repositoryPointer);
}