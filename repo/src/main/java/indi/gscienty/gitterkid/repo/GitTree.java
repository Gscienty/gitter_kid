package indi.gscienty.gitterkid.repo;

import java.util.Iterator;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.nativelib.IGitObjectLibrary;

/**
 * Tree 实体
 * @author ant
 *
 */
public class GitTree 
	extends GitObject
	implements Iterable<GitTree.Item>, Iterator<GitTree.Item> {
	
	private Pointer handle;
	private boolean iterateFirst;

	public GitTree(Repository repository, String signture) {
		super(repository, signture);
		this.iterateFirst = true;
		this.handle = this.lib.git_obj_get_tree(this.objHandle);
	}

	public Iterator<Item> iterator() {
		this.lib.git_obj_tree_reset(this.handle);
		this.iterateFirst = true;
		return this;
	}

	public boolean hasNext() {
		if (this.iterateFirst) {
			this.iterateFirst = false;
			return this.lib.git_obj_tree_current(this.handle) != Pointer.NULL;
		}
		
		return this.lib.git_obj_tree_move_next(this.handle) == 0;
	}


	public Item next() {
		Pointer itemPointer = this.lib.git_obj_tree_current(this.handle);
		String name = this.lib.git_obj_tree_item_name(itemPointer);
		String signture = this.lib.git_obj_tree_item_sign(itemPointer);
		
		switch (this.lib.git_obj_tree_item_type(itemPointer)) {
			case 3:
				return new TreeItem(this.repository, name, signture);
			default:
				return new BlobItem(this.repository, name, signture);
		}
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
		
		Item(GitObjectType treeItemType, String name, String signture) {
			this.treeItemType = treeItemType;
			this.name = name;
			this.signture = signture;
		}
		
		/**
		 * 获取当前Tree Item实体类型
		 * @return
		 */
		public GitObjectType getGitObjectType() {
			return this.treeItemType;
		}
		
		/**
		 * 获取Tree Item实体的名称
		 * @return
		 */
		public String getName() {
			return this.name;
		}
		
		/**
		 * 获取Tree Item实体的签名
		 * @return
		 */
		public String getSignture() {
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
		
		public TreeItem(Repository repository, String name, String signture) {
			super(GitObjectType.Tree, name, signture);
			this.repository = repository;
		}
		
		/**
		 * 获取Tree实体 
		 * @return Tree实体
		 */
		public GitTree getTree() {
			return new GitTree(this.repository, this.getSignture());
		}
	}
	
	/**
	 * Blob类型的Tree Item实体
	 * @author ant
	 *
	 */
	public class BlobItem extends Item {
		private Repository repository;
		
		public BlobItem(Repository repository, String name, String signture) {
			super(GitObjectType.Blob, name, signture);
			this.repository = repository;
		}
		
		/**
		 * 获取Blob实体
		 * @return Blob实体
		 */
		public GitBlob getBlob() {
			return new GitBlob(this.repository, this.getSignture());
		}
	}
}
