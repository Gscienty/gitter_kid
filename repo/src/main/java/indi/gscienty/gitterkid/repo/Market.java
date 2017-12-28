package indi.gscienty.gitterkid.repo;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.entity.Repository;
import indi.gscienty.gitterkid.repo.nativelib.IMarketLibrary;
import java.util.Iterator;

public class Market implements Iterable<Repository>, Iterator<Repository> {
    private IMarketLibrary lib;
    private String basePath;
    private Pointer handle;
    private boolean iterateFirst;


    public Market(String path) {
        this.basePath = path;
        this.lib = IMarketLibrary.Instance;
        this.iterateFirst = true;

        this.handle = this.lib.git_market_build(this.basePath);
    }

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
}