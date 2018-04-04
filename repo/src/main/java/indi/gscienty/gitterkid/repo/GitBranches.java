package indi.gscienty.gitterkid.repo;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.function.Function;
import java.util.function.Predicate;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.nativelib.IRepositoryLibrary;

public class GitBranches
	implements Iterable<GitBranch>, Iterator<GitBranch>, IQueryable<GitBranch> {
	private Repository repository;
	private Pointer handle;
	private IRepositoryLibrary lib;
	private Map<String, GitBranch> branchesMap;
	
	public GitBranches (Repository repository, Pointer branchesPointer) {
		this.handle = branchesPointer;
		this.lib = IRepositoryLibrary.Instance;
		this.repository = repository;
		
		this.branchesMap = new HashMap<>();
		this.forEach(branch -> this.branchesMap.put(branch.getName(), branch));
	}
	
	public GitBranch get(String name) {
		if (this.branchesMap.containsKey(name) == false) {
			return null;
		}
		return this.branchesMap.get(name);
	}
	
	@Override
	public boolean any (Predicate<GitBranch> predicate) {
		for (GitBranch branch : this) {
			if (predicate.test (branch)) {
				return true;
			}
		}
		return false;
	}

	@Override
	public boolean all (Predicate<GitBranch> predicate) {
		for (GitBranch branch : this) {
			if (predicate.test (branch) == false) {
				return false;
			}
		}
		return true;
	}

	@Override
	public GitBranch first (Predicate<GitBranch> predicate) {
		for (GitBranch branch : this) {
			if (predicate.test (branch)) {
				return branch;
			}
		}
		return null;
	}

	@Override
	public <R> List<R> filter (Function<GitBranch, R> transfer) {
		List<R> result = new Vector<> ();
		for (GitBranch branch : this) {
			result.add (transfer.apply (branch));
		}
		return result;
	}
	
	@Override
	public int count(Predicate<GitBranch> predicate) {
		int result = 0;
		for (GitBranch branch : this) {
			if (predicate.test(branch)) {
				result++;
			}
		}
		return result;
	}

	@Override
	public boolean hasNext () {
		return this.lib.gitbranches_hasnext (this.handle) != 0;
	}

	@Override
	public GitBranch next() {
		return new GitBranch (this.repository, this.lib.gitbranches_next (this.handle));
	}

	@Override
	public Iterator<GitBranch> iterator() {
		this.lib.gitbranches_reset (this.handle);
		return this;
	}

    /**
     * 析构方法
     */
    @Override
    protected void finalize() throws Throwable {
        this.lib.gitbranches_dtor (this.handle);
    }
}