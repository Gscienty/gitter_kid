package indi.gscienty.gitterkid.repo;

import com.sun.jna.Pointer;

/**
 * Commit实体
 * @author ant
 *
 */
public class GitCommit extends GitObject {
	private Pointer handle;
	
	public GitCommit(Repository repository, String signture) {
		super(repository, signture);
		
		this.handle = this.lib.git_obj_get_commit(this.objHandle);
	}
	
	/**
	 * 获取commit object中的消息部分
	 * @return commit消息
	 */
	public String getMessage() {
		return this.lib.git_obj_commit_message(this.handle);
	}
	
	/**
	 * 获取与commit object关联的Tree
	 * @return commit关联的Tree
	 */
	public GitTree getTree() {
		String treeSignture = this.lib.git_obj_commit_tree_sign(this.handle);
		return new GitTree(this.repository, treeSignture);
	}
	
	/**
	 * 获取与commit object关联的作者提交信息
	 * @return 作者提交信息
	 */
	public PersonLog getAuthor() {
		return new PersonLog(this.lib.git_obj_commit_author(this.handle));
	}
	
	/**
	 * 获取与commit object关联的提交者提交信息
	 * @return 提交者提交信息
	 */
	public PersonLog getCommitter() {
		return new PersonLog(this.lib.git_obj_commit_committer(this.handle));
	}
}
