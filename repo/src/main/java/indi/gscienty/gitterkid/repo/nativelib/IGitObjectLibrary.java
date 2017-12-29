package indi.gscienty.gitterkid.repo.nativelib;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface IGitObjectLibrary extends Library {
    IGitObjectLibrary Instance = (IGitObjectLibrary) Native.loadLibrary("libgkid.so", IGitObjectLibrary.class);

    Pointer git_obj_get(Pointer repositoryPointer, String signture);
    void git_obj_dispose(Pointer gitObjectPointer);
    
}