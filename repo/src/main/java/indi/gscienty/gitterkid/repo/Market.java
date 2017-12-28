package indi.gscienty.gitterkid.repo;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

interface Market extends Library {
    // loading libgkid.so
    Market Instance = (Market) Native.loadLibrary("libgkid.so", Market.class);

    // build git repositories market
    Pointer git_market_build(String basepath);
    // dispose git repositories market
    void git_market_dispose(Pointer marketPointer);
    // move to next git repository
    int git_market_cursor_move_next(Pointer marketPointer);
    // reset the git repositories's cursor
    void git_market_cursor_reset(Pointer marketPointer);
    // get current git repository
    Pointer git_market_cursor_current(Pointer marketPointer);
    // get repository's path
    String git_repo_path (Pointer repositoryPointer);
    // get repository's name
    String git_repo_name (Pointer repositoryPointer);
}