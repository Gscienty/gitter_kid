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
	private GitTree tree;
	private PersonLog author;
	private PersonLog committer;
	private CommitParents commitParents;
	
	public GitCommit (Repository repository, String signture) {
		super (repository, signture);
	}
	
	@Override
	protected GitObjectType entryObjectType() {
		return GitObjectType.Commit;
	}
	
	@Override
	protected void initialize() {
		this.handle = this.lib.gitobj_get_commit (this.objHandle);
		this.tree = null;
		this.author = null;
		this.committer = null;
		this.commitParents = null;
	}
	
	/**
	 * 获取commit object中的消息部分
	 * @return commit消息
	 */
	public String getMessage () {
		return this.lib.gitobj_commit_get_message (this.handle);
	}
	
	/**
	 * 获取与commit object关联的Tree
	 * @return commit关联的Tree
	 */
	public GitTree getTree () {
		if (this.tree == null) {
			String treeSignture = this.lib.gitobj_commit_get_treesign (this.handle);
			this.tree = new GitTree (this.repository, treeSignture);	
		}
		
		return this.tree;
	}
	
	/**
	 * 获取与commit object关联的作者提交信息
	 * @return 作者提交信息
	 */
	public PersonLog getAuthor () {
		if (this.author == null) {
			this.author = new PersonLog (this.lib.gitobj_commit_get_author (this.handle));
		}
		return this.author;
	}
	
	/**
	 * 获取与commit object关联的提交者提交信息
	 * @return 提交者提交信息
	 */
	public PersonLog getCommitter () {
		if (this.committer == null) {
			this.committer = new PersonLog (this.lib.gitobj_commit_get_committer (this.handle));
		}
		return this.committer;
	}
	
	/**
	 * 获取commit的上游commit
	 * @return CommitParents
	 */
	public CommitParents getParents() {
		if (this.commitParents == null) {
			this.commitParents = new CommitParents (this.handle, this.repository);
		}
		return this.commitParents;
	}
	
	public class CommitParents
		implements Iterable<GitCommit>, Iterator<GitCommit>, IQueryable<GitCommit>{
		private Repository repository;
		private Pointer handle;
		private IGitObjectLibrary lib;
		
		public CommitParents (Pointer handle, Repository repository) {
			this.repository = repository;
			this.handle = handle;
			this.lib = IGitObjectLibrary.Instance;
			this.lib.gitobj_commitparents_reset (this.handle);
		}

		public boolean any (Predicate<GitCommit> predicate) {
			for (GitCommit parent : this) {
				if (predicate.test (parent)) {
					return true;
				}
			}
			return false;
		}

		public boolean all (Predicate<GitCommit> predicate) {
			for (GitCommit parent : this) {
				if (predicate.test (parent) == false) {
					return false;
				}
			}
			return true;
		}

		public GitCommit first (Predicate<GitCommit> predicate) {
			for (GitCommit parent : this) {
				if (predicate.test (parent)) {
					return parent;
				}
			}
			return null;
		}

		public <R> List<R> filter (Function<GitCommit, R> transfer) {
			List<R> result = new Vector<> ();
			for (GitCommit parent : this) {
				result.add(transfer.apply (parent));
			}
			return result;
		}

		public boolean hasNext () {
			return this.lib.gitobj_commitparents_hasnext(this.handle) != 0;
		}

		public GitCommit next () {
			Pointer parentPointer = this.lib.gitobj_commitparents_next (this.handle);
			String commitSignture = this.lib.gitobj_commitpatent_get_sign (parentPointer);
			if (commitSignture.isEmpty ()) {
				return null;
			}
			return new GitCommit (this.repository, commitSignture);
		}

		public Iterator<GitCommit> iterator () {
			this.lib.gitobj_commitparents_reset (this.handle);
			return this;
		}
	}
}
