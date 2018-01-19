package indi.gscienty.gitterkid.repo;

import com.sun.jna.Pointer;

public class GitBlob extends GitObject {
    private int length;
    private byte[] content;
    private Pointer blobHandle;

    public GitBlob(Repository repository, String signture) {
        super(repository, signture);
    }
    
    @Override
    protected GitObjectType entryObjectType() {
    	return GitObjectType.Blob;
    }
    
    @Override
    protected void initialize() {
        this.blobHandle = this.lib.gitobj_get_blob (this.objHandle);
        
        this.length = this.lib.gitobj_blob_length (this.blobHandle);
        this.content = this.lib.gitobj_blob_content (this.blobHandle).getByteArray(0, this.length);
    }

    public int getLength() {
        return this.length;
    }

    public byte[] getContent() {
        return this.content;
    }
}