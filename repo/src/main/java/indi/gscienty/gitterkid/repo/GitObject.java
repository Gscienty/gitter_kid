package indi.gscienty.gitterkid.repo;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.nativelib.IGitObjectLibrary;

public abstract class GitObject {
    protected IGitObjectLibrary lib;
    protected Pointer objHandle;
    protected Repository repository;
    private String signture;
    private GitObjectType objectType;

    public GitObject(Repository repository, String signture) {
        this.lib = IGitObjectLibrary.Instance;
        this.signture = signture;
        this.repository = repository;
        this.objHandle = this.lib.git_obj_get(repository.getHandle(), this.signture);
        
        this.objectType = null;
    }
    
    /**
     * 获取实体对象的类型
     * @return
     */
    public GitObjectType getObjectType() {
        if (this.objectType == null) {
            int numericObjType = this.lib.git_obj_type(this.objHandle);

            switch (numericObjType) {
                case 0: this.objectType = GitObjectType.Unknow; break;
                case 1: this.objectType = GitObjectType.Blob; break;
                case 2: this.objectType = GitObjectType.Commit; break;
                case 3: this.objectType = GitObjectType.Tree; break;
                default: this.objectType = GitObjectType.Unknow;
            }
        }

        return this.objectType;
    }
}