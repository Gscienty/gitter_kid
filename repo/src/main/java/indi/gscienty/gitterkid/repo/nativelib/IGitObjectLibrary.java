package indi.gscienty.gitterkid.repo.nativelib;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface IGitObjectLibrary extends Library {
	/**
	 * 加载libgkid.so动态链接库
	 */
    IGitObjectLibrary Instance = (IGitObjectLibrary) Native.loadLibrary ("libgkid.so", IGitObjectLibrary.class);
    
    /**
     * 根据签名返回仓库中的实体
     * @param repositoryPointer 仓库
     * @param signture 签名
     * @return 实体
     */
    Pointer gitrepo_get_gitobj (Pointer repositoryPointer, String signture);
    /**
     * 析构实体
     * @param gitObjectPointer 待析构实体
     */
    void gitobj_dtor (Pointer gitObjectPointer);
    /**
     * 获取实体的类型
     * @param gitObjectPointer 实体
     * @return 实体类型
     */
    int gitobj_get_type (Pointer gitObjectPointer);

    /**
     * 获取Blob类型的实体
     * @param gitObjectPointer 实体
     * @return Blob类型的实体
     */
    Pointer gitobj_get_blob (Pointer gitObjectPointer);
    /**
     * 获取Commit类型的实体
     * @param gitObjectPointer 实体
     * @return Commit类型的实体
     */
    Pointer gitobj_get_commit (Pointer gitObjectPointer);
    /**
     * 获取Tree类型的实体
     * @param gitObjectPointer 实体
     * @return Tree类型的实体
     */
    Pointer gitobj_get_tree (Pointer gitObjectPointer);

    /**
     * 获取Blob类型的长度
     * @param blobPointer Blob类型实体
     * @return Blob类型的长度
     */
    int gitobj_blob_length (Pointer blobPointer);
    /**
     * 获取Blob类型的内容
     * @param blobPointer Blob类型实体
     * @return Blob类型的内容
     */
    Pointer gitobj_blob_content (Pointer blobPointer);
    
    /**
     * 重置Tree类型实体的游标
     * @param treeObjectPointer Tree类型实体
     */
    void gitobj_tree_reset (Pointer treeObjectPointer);
    /**
     * 将Tree类型实体的游标向下移动
     * @param treeObjectPointer Tree类型实体
     * @return 游标向下移动是否成功，成功则返回0
     */
    int gitobj_tree_hasnext (Pointer treeObjectPointer);
    /**
     * 获取Tree类型实体的游标指向的Tree Item实体
     * @param treeObjectPointer Tree类型实体 
     * @return Item实体
     */
    Pointer gitobj_tree_next (Pointer treeObjectPointer);
    
    /**
     * 获取Tree Item实体的类型
     * @param treeItemObjectPointer Tree Item类型实体
     * @return Tree Item实体类型，当返回值为1时，为Blob类型; 当返回值为3时，为Tree类型
     */
    int gitobj_treeitem_get_type (Pointer treeItemObjectPointer);
    /**
     * 获取Tree Item实体的名称
     * @param treeItemObjectPointer Tree Item类型实体
     * @return Tree Item实体名称
     */
    String gitobj_treeitem_get_name (Pointer treeItemObjectPointer);
    /**
     * 获取Tree Item实体的签名
     * @param treeItemObjectPointer Tree Item类型实体
     * @return Tree Item实体签名
     */
    String gitobj_treeitem_get_sign (Pointer treeItemObjectPointer);
    
    /**
     * 获取commit实体关联的Tree实体签名
     * @param commitObjectPointer commit实体
     * @return Tree实体签名
     */
    String gitobj_commit_get_treesign (Pointer commitObjectPointer);
    /**
     * 获取commit实体关联的作者提交信息
     * @param commitObjectPointer commit实体
     * @return 作者提交信息
     */
    Pointer gitobj_commit_get_author (Pointer commitObjectPointer);
    /**
     * 获取commit实体关联的提交者提交信息
     * @param commitObjectPointer commit实体
     * @return 提交者提交信息
     */
    Pointer gitobj_commit_get_committer (Pointer commitObjectPointer);
    /**
     * 获取commit实体的提交消息
     * @param commitObjectPointer commit实体
     * @return 提交消息
     */
    String gitobj_commit_get_message (Pointer commitObjectPointer);
    /**
     * 重置commit实体中parent游标
     * @param commitObjectPointer commit实体
     */
    void gitobj_commitparents_reset (Pointer commitObjectPointer);
    /**
     * 获取 commit 签名列表游标下一个 commit 签名结构体
     * @param commitObjectPointer commit实体
     * @return commit 上游 commit 签名结构体
     */
    Pointer gitobj_commitparents_next (Pointer commitObjectPointer);
    /**
     * 获取 commit 的上游 commit 签名列表是否存在下一个元素
     * @param commitObjectPointer commit实体
     * @return 是否存在下一个元素
     */
    int gitobj_commitparents_hasnext (Pointer commitObjectPointer);
    
    /**
     * 获取当前commit实体中parent游标所指向的parent签名
     * @param commitParentObjectPointer commit实体
     * @return 返回parent签名
     */
    String gitobj_commitpatent_get_sign (Pointer commitParentObjectPointer);
    
    /**
     * 获取person log的名称
     * @param personLogObjectPointer person log实体
     * @return person log的名称
     */
    String gitperson_get_name (Pointer personLogObjectPointer);
    /**
     * 获取person log的mail
     * @param personLogObjectPointer person log实体
     * @return person log的mail
     */
    String gitperson_get_mail (Pointer personLogObjectPointer);
    /**
     * 获取person log的时间戳
     * @param personLogObjectPointer person log实体
     * @return person log的时间戳
     */
    long gitperson_get_timestamp (Pointer personLogObjectPointer);
    /**
     * 获取person log的时区
     * @param personLogObjectPointer person log实体
     * @return person log的时区
     */
    String gitperson_get_timezone (Pointer personLogObjectPointer);
}