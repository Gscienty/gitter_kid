package indi.gscienty.gitterkid.webapi.entities;

public class TreeItem {
	private String name;
	private String type;
	private String path;
	private String signture;
	private String message;
	
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public String getPath() {
		return path;
	}
	public void setPath(String path) {
		this.path = path;
	}
	public String getSignture() {
		return signture;
	}
	public void setSignture(String sign) {
		this.signture = sign;
	}
	public String getMessage() {
		return message;
	}
	public void setMessage(String message) {
		this.message = message;
	}
}
