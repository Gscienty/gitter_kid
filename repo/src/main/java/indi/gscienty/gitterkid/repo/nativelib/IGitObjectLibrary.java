package indi.gscienty.gitterkid.repo.nativelib;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface IGitObjectLibrary extends Library {
	/**
	 * 加载libgkid.so动态链接库
	 */
    IGitObjectLibrary Instance = (IGitObjectLibrary) Native.loadLibrary("libgkid.so", IGitObjectLibrary.class);
    
    /**
     * 根据签名返回仓库中的实体
     * @param repositoryPointer 仓库
     * @param signture 签名
     * @return 实体
     */
    Pointer git_obj_get(Pointer repositoryPointer, String signture);
    /**
     * 析构实体
     * @param gitObjectPointer 待析构实体
     */
    void git_obj_dispose(Pointer gitObjectPointer);
    /**
     * 获取实体的类型
     * @param gitObjectPointer 实体
     * @return 实体类型
     */
    int git_obj_type(Pointer gitObjectPointer);

    /**
     * 获取Blob类型的实体
     * @param gitObjectPointer 实体
     * @return Blob类型的实体
     */
    Pointer git_obj_get_blob(Pointer gitObjectPointer);
    /**
     * 获取Commit类型的实体
     * @param gitObjectPointer 实体
     * @return Commit类型的实体
     */
    Pointer git_obj_get_commit(Pointer gitObjectPointer);
    /**
     * 获取Tree类型的实体
     * @param gitObjectPointer 实体
     * @return Tree类型的实体
     */
    Pointer git_obj_get_tree(Pointer gitObjectPointer);

    /**
     * 获取Blob类型的长度
     * @param blobPointer Blob类型实体
     * @return Blob类型的长度
     */
    int git_obj_blob_length(Pointer blobPointer);
    /**
     * 获取Blob类型的内容
     * @param blobPointer Blob类型实体
     * @return Blob类型的内容
     */
    Pointer git_obj_blob_content(Pointer blobPointer);
    
    /**
     * 重置Tree类型实体的游标
     * @param treeObjectPointer Tree类型实体
     */
    void git_obj_tree_reset(Pointer treeObjectPointer);
    /**
     * 将Tree类型实体的游标向下移动
     * @param treeObjectPointer Tree类型实体
     * @return 游标向下移动是否成功，成功则返回0
     */
    int git_obj_tree_move_next(Pointer treeObjectPointer);
    /**
     * 获取Tree类型实体的游标指向的Tree Item实体
     * @param treeObjectPointer Tree类型实体 
     * @return Item实体
     */
    Pointer git_obj_tree_current(Pointer treeObjectPointer);
    
    /**
     * 获取Tree Item实体的类型
     * @param treeItemObjectPointer Tree Item类型实体
     * @return Tree Item实体类型，当返回值为1时，为Blob类型; 当返回值为3时，为Tree类型
     */
    int git_obj_tree_item_type(Pointer treeItemObjectPointer);
    /**
     * 获取Tree Item实体的名称
     * @param treeItemObjectPointer Tree Item类型实体
     * @return Tree Item实体名称
     */
    String git_obj_tree_item_name(Pointer treeItemObjectPointer);
    /**
     * 获取Tree Item实体的签名
     * @param treeItemObjectPointer Tree Item类型实体
     * @return Tree Item实体签名
     */
    String git_obj_tree_item_sign(Pointer treeItemObjectPointer);
    
    /**
     * 获取commit实体关联的Tree实体签名
     * @param commitObjectPointer commit实体
     * @return Tree实体签名
     */
    String git_obj_commit_tree_sign(Pointer commitObjectPointer);
    /**
     * 获取commit实体关联的作者提交信息
     * @param commitObjectPointer commit实体
     * @return 作者提交信息
     */
    Pointer git_obj_commit_author(Pointer commitObjectPointer);
    /**
     * 获取commit实体关联的提交者提交信息
     * @param commitObjectPointer commit实体
     * @return 提交者提交信息
     */
    Pointer git_obj_commit_committer(Pointer commitObjectPointer);
    /**
     * 获取commit实体的提交消息
     * @param commitObjectPointer commit实体
     * @return 提交消息
     */
    String git_obj_commit_message(Pointer commitObjectPointer);
    /**
     * 重置commit实体中parent游标
     * @param commitObjectPointer commit实体
     */
    void git_obj_commit_parent_reset(Pointer commitObjectPointer);
    /**
     * 获取commit实体中parent游标指向的当前parent
     * @param commitObjectPointer commit实体
     * @return commit parent实体
     */
    Pointer git_obj_commit_parent_current(Pointer commitObjectPointer);
    /**
     * 将commit实体中parent的游标向下移动
     * @param commitObjectPointer commit实体
     * @return 当向下移动成功时，则返回0
     */
    int git_obj_commit_parent_move_next(Pointer commitObjectPointer);
    
    /**
     * 获取当前commit实体中parent游标所指向的parent签名
     * @param commitParentObjectPointer commit实体
     * @return 返回parent签名
     */
    String git_obj_commit_parent_sign(Pointer commitParentObjectPointer);
    
    /**
     * 获取person log的名称
     * @param personLogObjectPointer person log实体
     * @return person log的名称
     */
    String git_person_log_name(Pointer personLogObjectPointer);
    /**
     * 获取person log的mail
     * @param personLogObjectPointer person log实体
     * @return person log的mail
     */
    String git_person_log_mail(Pointer personLogObjectPointer);
    /**
     * 获取person log的时间戳
     * @param personLogObjectPointer person log实体
     * @return person log的时间戳
     */
    long git_person_log_timestamp(Pointer personLogObjectPointer);
    /**
     * 获取person log的时区
     * @param personLogObjectPointer person log实体
     * @return person log的时区
     */
    String git_person_timezone(Pointer personLogObjectPointer);
}