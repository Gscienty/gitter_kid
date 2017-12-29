package indi.gscienty.gitterkid.repo;

import com.sun.jna.Pointer;

public class GitBlob extends GitObject {
    private int length;
    private byte[] content;
    private Pointer blobHandle;

    public GitBlob(Repository repository, String signture) {
        super(repository, signture);
        this.blobHandle = this.lib.git_obj_get_blob(this.objHandle);
        this.Initialize();
    }

    private void Initialize() {
        this.length = this.lib.git_obj_blob_length(this.blobHandle);
        this.content = this.lib.git_obj_blob_content(this.blobHandle).getByteArray(0, this.length);
    }

    public int getLength() {
        return this.length;
    }

    public byte[] getContent() {
        return this.content;
    }
}