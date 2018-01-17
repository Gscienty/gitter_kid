package indi.gscienty.gitterkid.repo.nativelib;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface IMarketLibrary extends Library {
    /**
     * 加载libgkid.so动态链接库
     */
    IMarketLibrary Instance = (IMarketLibrary) Native.loadLibrary ("libgkid.so", IMarketLibrary.class);

    /**
     * 构造仓库市场
     * @param basepath 仓库市场路径
     * @return 仓库市场
     */
    Pointer get_gitmarket (String basepath);
    /**
     * 析构仓库市场
     * @param marketPointer 仓库市场
     */
    void gitmarket_dtor (Pointer marketPointer);
    /**
     * 仓库市场是否存在下一个仓库
     * @param marketPointer 仓库市场
     * @return 返回是否存在下一个仓库
     */
    int gitmarket_hasnext (Pointer marketPointer);
    /**
     * 重置仓库市场游标
     * @param marketPointer 仓库市场
     */
    void gitmarket_reset (Pointer marketPointer);
    /**
     * 获取仓库市场下一个仓库
     * @param marketPointer 仓库市场
     * @return 返回下一个仓库指针
     */
    Pointer gitmarket_next (Pointer marketPointer);
}