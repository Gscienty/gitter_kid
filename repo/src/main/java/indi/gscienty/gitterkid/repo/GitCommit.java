package indi.gscienty.gitterkid.repo;

import java.util.Iterator;
import java.util.List;
import java.util.Vector;
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
	
	/**
	 * 获取commit的上游commit
	 * @return CommitParents
	 */
	public CommitParents getParents() {
		return new CommitParents(this.handle, this.repository);
	}
	
	public class CommitParents
		implements Iterable<GitCommit>, Iterator<GitCommit>, IQueryable<GitCommit>{
		private Repository repository;
		private Pointer handle;
		private boolean iterateFirst;
		private IGitObjectLibrary lib;
		
		public CommitParents(Pointer handle, Repository repository) {
			this.repository = repository;
			this.handle = handle;
			this.lib = IGitObjectLibrary.Instance;
			this.iterateFirst = true;
			this.lib.git_obj_commit_parent_reset(this.handle);
		}

		public boolean any(Predicate<GitCommit> predicate) {
			for (GitCommit parent : this) {
				if (predicate.test(parent)) {
					return true;
				}
			}
			return false;
		}

		public boolean all(Predicate<GitCommit> predicate) {
			for (GitCommit parent : this) {
				if (predicate.test(parent) == false) {
					return false;
				}
			}
			return true;
		}

		public GitCommit first(Predicate<GitCommit> predicate) {
			for (GitCommit parent : this) {
				if (predicate.test(parent)) {
					return parent;
				}
			}
			return null;
		}

		public <R> List<R> filter(Function<GitCommit, R> transfer) {
			List<R> result = new Vector<>();
			for (GitCommit parent : this) {
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

		public GitCommit next() {
			Pointer parentPointer = this.lib.git_obj_commit_parent_current(this.handle);
			String commitSignture = this.lib.git_obj_commit_parent_sign(parentPointer);
			if (commitSignture.isEmpty()) {
				return null;
			}
			return new GitCommit(this.repository, commitSignture);
		}

		public Iterator<GitCommit> iterator() {
			this.lib.git_obj_commit_parent_reset(this.handle);
			return this;
		}
	}
}
