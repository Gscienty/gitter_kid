package indi.gscienty.gitterkid.repo.nativelib;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface IMarketLibrary extends Library {
    /**
     * 加载libgkid.so动态链接库
     */
    IMarketLibrary Instance = (IMarketLibrary) Native.loadLibrary("libgkid.so", IMarketLibrary.class);

    /**
     * 构造仓库市场
     * @param basepath 仓库市场路径
     * @return 仓库市场
     */
    Pointer git_market_build(String basepath);
    /**
     * 析构仓库市场
     * @param marketPointer 仓库市场
     */
    void git_market_dispose(Pointer marketPointer);
    /**
     * 移动仓库市场指向仓库的游标到下一位置
     * @param marketPointer 仓库市场
     * @return 当移动游标到下一位置成功时，返回0
     */
    int git_market_cursor_move_next(Pointer marketPointer);
    /**
     * 重置仓库市场游标
     * @param marketPointer 仓库市场
     */
    void git_market_cursor_reset(Pointer marketPointer);
    /**
     * 获取仓库市场游标所指向的仓库
     * @param marketPointer 仓库市场
     * @return 仓库游标所指向的仓库
     */
    Pointer git_market_cursor_current(Pointer marketPointer);
}