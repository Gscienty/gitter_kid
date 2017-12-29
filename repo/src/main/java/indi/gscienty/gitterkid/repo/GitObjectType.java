package indi.gscienty.gitterkid.repo;

public enum GitObjectType {
    Unknow(0), Blob(1), Commit(2), Tree(3);

    private int index;
    private GitObjectType(int index) {
        this.index = index;
    }

    public int getIndex() {
        return this.index;
    }
    public void setIndex(int index) {
        this.index = index;
    }
}