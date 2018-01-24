package indi.gscienty.gitterkid.repo;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.Repository;
import indi.gscienty.gitterkid.repo.nativelib.IMarketLibrary;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.logging.Logger;

/**
 * 仓库市场
 * @author ant
 *
 */
public class Market
	implements Iterable<Repository>, Iterator<Repository>, IQueryable<Repository> {
	private static Logger logger = Logger.getLogger(Market.class.getName());
	
    private IMarketLibrary lib;
    private String basePath;
    private Pointer handle;

    /**
     * 构造方法
     * @param path 仓库市场路径
     */
    public Market (String path) {
        this.basePath = path;
        this.lib = IMarketLibrary.Instance;

        this.handle = this.lib.get_gitmarket (this.basePath);
    	logger.info("ctor market: " + this.basePath);
    }
    
    public boolean isLegal() {
    	return this.handle.equals(Pointer.NULL) == false;
    }
    
    public String getPath() {
    	return this.basePath;
    }

	public Iterator<Repository> iterator () {
        this.lib.gitmarket_reset (this.handle);
		return this;
	}

	public boolean hasNext () {
        return this.lib.gitmarket_hasnext (this.handle) != 0;
	}

	public Repository next () {
		return new Repository (this.lib.gitmarket_next (this.handle));
	}
	
	/**
	 * 判断仓库市场内是否存在满足某条件的仓库
	 */
	public boolean any (Predicate<Repository> predicate) {
		for (Repository repository : this) {
			if (predicate.test (repository)) {
				return true;
			}
		}
		return false;
	}

	/**
	 * 判断仓库市场内的所有仓库是否都满足某条件
	 */
	public boolean all (Predicate<Repository> predicate) {
		for (Repository repository : this) {
			if (predicate.test (repository) == false) {
				return false;
			}
		}
		return true;
	}
	
	/**
	 * 获取仓库市场内第一个满足某条件的仓库
	 */
	public Repository first (Predicate<Repository> predicate) {
		for (Repository repository : this) {
			if (predicate.test (repository)) {
				return repository;
			}
		}
		return null;
	}
	
	/**
	 * 映射仓库市场内的仓库到新的集合
	 */
	public <R> List<R> filter (Function<Repository, R> transfer) {
		List<R> result = new Vector<> ();
		for (Repository repository : this) {
			result.add (transfer.apply (repository));
		}
		return result;
	}
    
    /**
     * 析构方法
     */
    @Override
    protected void finalize () throws Throwable {
    	logger.info("dtor market: " + this.basePath);
        super.finalize ();
        
        this.lib.gitmarket_dtor (this.handle);
    }
}



