package indi.gscienty.gitterkid.repo.nativelib;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface IGitObjectLibrary extends Library {
    IGitObjectLibrary Instance = (IGitObjectLibrary) Native.loadLibrary("libgkid.so", IGitObjectLibrary.class);

    Pointer git_obj_get(Pointer repositoryPointer, String signture);
    void git_obj_dispose(Pointer gitObjectPointer);
    int git_obj_type(Pointer gitObjectPointer);

    Pointer git_obj_get_blob(Pointer gitObjectPointer);
    Pointer git_obj_get_commit(Pointer gitObjectPointer);
    Pointer git_obj_get_tree(Pointer gitObjectPointer);

    int git_obj_blob_length(Pointer blobPointer);
    Pointer git_obj_blob_content(Pointer blobPointer);
    
    void git_obj_tree_reset(Pointer treeObjectPointer);
    int git_obj_tree_move_next(Pointer treeObjectPointer);
    Pointer git_obj_tree_current(Pointer treeObjectPointer);
    
    int git_obj_tree_item_type(Pointer treeItemObjectPointer);
    String git_obj_tree_item_name(Pointer treeItemObjectPointer);
}