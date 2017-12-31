package indi.gscienty.gitterkid.repo;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.function.Function;
import java.util.function.Predicate;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.nativelib.IGitObjectLibrary;

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
	
	public class CommitParents
		implements Iterable<String>, Iterator<String>, IQueryable<String>{
		private Pointer handle;
		private boolean iterateFirst;
		private IGitObjectLibrary lib;
		
		public CommitParents(Pointer handle, IGitObjectLibrary lib) {
			this.handle = handle;
			this.lib = lib;
			this.iterateFirst = true;
			this.lib.git_obj_commit_parent_reset(this.handle);
		}

		public boolean any(Predicate<String> predicate) {
			for (String parent : this) {
				if (predicate.test(parent)) {
					return true;
				}
			}
			return false;
		}

		public boolean all(Predicate<String> predicate) {
			for (String parent : this) {
				if (predicate.test(parent) == false) {
					return false;
				}
			}
			return true;
		}

		public String first(Predicate<String> predicate) {
			for (String parent : this) {
				if (predicate.test(parent)) {
					return parent;
				}
			}
			return null;
		}

		public <R> List<R> filter(Function<String, R> transfer) {
			List<R> result = new LinkedList<R>();
			for (String parent : this) {
				result.add(transfer.apply(parent));
			}
			return result;
		}

		public boolean hasNext() {
			if (this.iterateFirst) {
				this.iterateFirst = false;
				return this.lib.git_obj_commit_parent_current(this.handle) != Pointer.NULL;
			}
			return this.lib.git_obj_commit_parent_move_next(this.handle) == 0;
		}

		public String next() {
			Pointer parentPointer = this.lib.git_obj_commit_parent_current(this.handle);
			return this.lib.git_obj_commit_parent_sign(parentPointer);
		}

		public Iterator<String> iterator() {
			this.lib.git_obj_commit_parent_reset(this.handle);
			return this;
		}
	}
}
