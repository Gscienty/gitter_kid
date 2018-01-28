package indi.gscienty.gitterkid.repo;

import java.util.logging.Logger;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.nativelib.IGitObjectLibrary;

public abstract class GitObject {
	private static Logger logger = Logger.getLogger(GitObject.class.getName()); 
	
    protected IGitObjectLibrary lib;
    protected Pointer objHandle;
    protected Repository repository;
    private String signture;
    private GitObjectType objectType;
    private boolean legal;

    public GitObject(Repository repository, String signture) {
    	GitObject.logger.info("ctor git object");
        this.lib = IGitObjectLibrary.Instance;
        this.signture = signture;
        this.repository = repository;
        this.objHandle = this.lib.gitrepo_get_gitobj (repository.getHandle(), this.signture);
        
        this.transferSpecific();
    }
    
    protected abstract void initialize();
    
    protected abstract GitObjectType entryObjectType();
    
    private void transferSpecific() {
        if (this.objHandle.equals(Pointer.NULL)) {
        	GitObject.logger.warning("not exist this git object, sign: " + this.signture);
        	this.objectType = GitObjectType.NotExist;
        	this.legal = false;
        	return;
        }
        
    	GitObject.logger.info("get git object, sign: " + this.signture);
    	
    	if (this.getObjectType().equals(this.entryObjectType()) == false) {
    		GitObject.logger.warning("type mismatch");
    		this.legal = false;
    		return;
    	}
    	
		this.initialize();
		this.legal = true;
    }
    
    public boolean isLegal() {
    	return this.legal;
    }
    
    public String getSignture() {
    	return this.signture;
    }
    
    /**
     * 获取实体对象的类型
     * @return
     */
    public GitObjectType getObjectType () {
        if (this.objectType == null) {
            int numericObjType = this.lib.gitobj_get_type (this.objHandle);

            switch (numericObjType) {
                case 0: this.objectType = GitObjectType.Unknow; break;
                case 1: this.objectType = GitObjectType.Blob; break;
                case 2: this.objectType = GitObjectType.Commit; break;
                case 3: this.objectType = GitObjectType.Tree; break;
                case 4: this.objectType = GitObjectType.Tag; break;
                default: this.objectType = GitObjectType.Unknow;
            }
        }

        return this.objectType;
    }
    
    /**
     * 析构方法
     */
    @Override
    protected void finalize () throws Throwable {
    	GitObject.logger.info("dtor gitobj, type: " + this.getObjectType() + " signture: " + this.signture);
        this.lib.gitobj_dtor (this.objHandle);
        super.finalize ();
    }
}