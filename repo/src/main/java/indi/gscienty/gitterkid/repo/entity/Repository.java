package indi.gscienty.gitterkid.repo.entity;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.nativelib.IRepositoryLibrary;

public class Repository {
    private Pointer handle;
    private IRepositoryLibrary lib;

    public Repository(Pointer repositoryPointer) {
        this.handle = repositoryPointer;

        this.lib = IRepositoryLibrary.Instance;
    }

    public String getPath() {
        return this.lib.git_repo_path(this.handle);
    }

    public String getName() {
        return this.lib.git_repo_name(this.handle);
    }
    
    public Pointer getHandle() {
        return this.handle;
    }
}