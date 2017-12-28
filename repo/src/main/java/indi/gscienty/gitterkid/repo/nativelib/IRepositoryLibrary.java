package indi.gscienty.gitterkid.repo.nativelib;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface IRepositoryLibrary extends Library {
    // load libgkid.so
    IRepositoryLibrary Instance = (IRepositoryLibrary) Native.loadLibrary("libgkid.so", IRepositoryLibrary.class);

    // get repository's path
    String git_repo_path(Pointer repositoryPointer);
    // get repository's name
    String git_repo_name(Pointer repositoryPointer);
}