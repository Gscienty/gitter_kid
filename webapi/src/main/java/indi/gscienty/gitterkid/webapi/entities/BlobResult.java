package indi.gscienty.gitterkid.webapi.entities;

public class BlobResult {
	private byte[] content;
	private int length;

	public byte[] getContent() {
		return content;
	}

	public void setContent(byte[] content) {
		this.content = content;
	}

	public int getLength() {
		return length;
	}

	public void setLength(int length) {
		this.length = length;
	}
}
