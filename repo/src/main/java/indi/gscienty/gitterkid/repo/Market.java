package indi.gscienty.gitterkid.repo;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.Repository;
import indi.gscienty.gitterkid.repo.nativelib.IMarketLibrary;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.function.Function;
import java.util.function.Predicate;

/**
 * 仓库市场
 * @author ant
 *
 */
public class Market
	implements Iterable<Repository>, Iterator<Repository>, IQueryable<Repository> {
    private IMarketLibrary lib;
    private String basePath;
    private Pointer handle;
    private boolean iterateFirst;

    /**
     * 构造方法
     * @param path 仓库市场路径
     */
    public Market(String path) {
        this.basePath = path;
        this.lib = IMarketLibrary.Instance;
        this.iterateFirst = true;

        this.handle = this.lib.git_market_build(this.basePath);
    }
    
    /**
     * 析构方法
     */
    @Override
    protected void finalize() throws Throwable {
        this.lib.git_market_dispose(this.handle);

        super.finalize();
    }

	public Iterator<Repository> iterator() {
        this.lib.git_market_cursor_reset(this.handle);
        this.iterateFirst = true;
		return this;
	}

	public boolean hasNext() {
        if (this.iterateFirst) {
            this.iterateFirst = false;
            return this.lib.git_market_cursor_current(this.handle) != Pointer.NULL;
        }
        else {
            return this.lib.git_market_cursor_move_next(this.handle) == 0;
        }
	}

	public Repository next() {
		return new Repository(this.lib.git_market_cursor_current(this.handle));
	}
	
	/**
	 * 判断仓库市场内是否存在满足某条件的仓库
	 */
	public boolean any(Predicate<Repository> predicate) {
		for (Repository repository : this) {
			if (predicate.test(repository)) {
				return true;
			}
		}
		return false;
	}

	/**
	 * 判断仓库市场内的所有仓库是否都满足某条件
	 */
	public boolean all(Predicate<Repository> predicate) {
		for (Repository repository : this) {
			if (predicate.test(repository) == false) {
				return false;
			}
		}
		return true;
	}
	
	/**
	 * 获取仓库市场内第一个满足某条件的仓库
	 */
	public Repository first(Predicate<Repository> predicate) {
		for (Repository repository : this) {
			if (predicate.test(repository)) {
				return repository;
			}
		}
		return null;
	}
	
	/**
	 * 映射仓库市场内的仓库到新的集合
	 */
	public <R> List<R> filter(Function<Repository, R> transfer) {
		List<R> result = new LinkedList<R>();
		for (Repository repository : this) {
			result.add(transfer.apply(repository));
		}
		return result;
	}
}



