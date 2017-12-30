package indi.gscienty.gitterkid.repo;

import java.util.Iterator;

import com.sun.jna.Pointer;

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
		GitObjectType itemType = GitObjectType.Unknow;
		switch (this.lib.git_obj_tree_item_type(itemPointer)) {
		}
		
		return new Item(itemType, name);
	}
	
	public class Item {
		private GitObjectType treeItemType;
		private String name;
		
		Item(GitObjectType treeItemType, String name) {
			this.treeItemType = treeItemType;
			this.name = name;
		}
		
		public GitObjectType getGitObjectType() {
			return this.treeItemType;
		}
		
		public String getName() {
			return this.name;
		}
	}
}
