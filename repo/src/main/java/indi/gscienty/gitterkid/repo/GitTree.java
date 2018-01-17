package indi.gscienty.gitterkid.repo;

import java.util.Iterator;
import java.util.List;
import java.util.Vector;
import java.util.function.Function;
import java.util.function.Predicate;

import com.sun.jna.Pointer;

/**
 * Tree 实体
 * @author ant
 *
 */
public class GitTree 
	extends GitObject
	implements Iterable<GitTree.Item>, Iterator<GitTree.Item>, IQueryable<GitTree.Item> {
	
	private Pointer handle;

	public GitTree (Repository repository, String signture) {
		super (repository, signture);
		this.handle = this.lib.gitobj_get_tree (this.objHandle);
	}

	public Iterator<Item> iterator () {
		this.lib.gitobj_tree_reset (this.handle);
		return this;
	}

	public boolean hasNext () {
		return this.lib.gitobj_tree_hasnext (this.handle) != 0;
	}


	public Item next () {
		Pointer itemPointer = this.lib.gitobj_tree_next (this.handle);
		String name = this.lib.gitobj_treeitem_get_name (itemPointer);
		String signture = this.lib.gitobj_treeitem_get_sign (itemPointer);
		
		switch (this.lib.gitobj_treeitem_get_type (itemPointer)) {
			case 3:
				return new TreeItem(this.repository, name, signture);
			default:
				return new BlobItem(this.repository, name, signture);
		}
	}
	
	/**
	 * Tree中是否存在满足某条件的Tree Item
	 */
	public boolean any (Predicate<Item> predicate) {
		for (Item item : this) {
			if (predicate.test (item)) {
				return true;
			}
		}
		return false;
	}
	
	/**
	 * Tree中的元素是否全部满足某条件
	 */
	public boolean all (Predicate<Item> predicate) {
		for (Item item : this) {
			if (predicate.test (item) == false) {
				return false;
			}
		}
		return true;
	}
	
	/**
	 * 返回Tree中第一个满足某条件的Tree Item
	 */
	public Item first (Predicate<Item> predicate) {
		for (Item item : this) {
			if (predicate.test (item)) {
				return item;
			}
		}
		return null;
	}

	/**
	 * 将Tree内的元素映射成为新的集合
	 */
	public <R> List<R> filter (Function<Item, R> transfer) {
		List<R> result = new Vector<>();
		for (Item item : this) {
			result.add (transfer.apply (item));
		}
		return result;
	}
	
	/**
	 * Tree Item 实体
	 * @author ant
	 *
	 */
	public abstract class Item {
		private GitObjectType treeItemType;
		private String name;
		private String signture;
		
		Item (GitObjectType treeItemType, String name, String signture) {
			this.treeItemType = treeItemType;
			this.name = name;
			this.signture = signture;
		}
		
		/**
		 * 获取当前Tree Item实体类型
		 * @return
		 */
		public GitObjectType getGitObjectType () {
			return this.treeItemType;
		}
		
		/**
		 * 获取Tree Item实体的名称
		 * @return
		 */
		public String getName () {
			return this.name;
		}
		
		/**
		 * 获取Tree Item实体的签名
		 * @return
		 */
		public String getSignture () {
			return this.signture;
		}
	}
	
	/**
	 * Tree类型的Tree Item实体
	 * @author ant
	 *
	 */
	public class TreeItem extends Item {
		private Repository repository;
		
		public TreeItem (Repository repository, String name, String signture) {
			super (GitObjectType.Tree, name, signture);
			this.repository = repository;
		}
		
		/**
		 * 获取Tree实体 
		 * @return Tree实体
		 */
		public GitTree getTree () {
			return new GitTree (this.repository, this.getSignture ());
		}
	}
	
	/**
	 * Blob类型的Tree Item实体
	 * @author ant
	 *
	 */
	public class BlobItem extends Item {
		private Repository repository;
		
		public BlobItem (Repository repository, String name, String signture) {
			super (GitObjectType.Blob, name, signture);
			this.repository = repository;
		}
		
		/**
		 * 获取Blob实体
		 * @return Blob实体
		 */
		public GitBlob getBlob () {
			return new GitBlob (this.repository, this.getSignture ());
		}
	}
}
