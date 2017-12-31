package indi.gscienty.gitterkid.repo;

import com.sun.jna.Pointer;

import indi.gscienty.gitterkid.repo.nativelib.IGitObjectLibrary;

public class PersonLog {
	private Pointer handle;
	private IGitObjectLibrary lib;
	
	/**
	 * 构造方法
	 * @param handle person log指针
	 */
	public PersonLog(Pointer handle) {
		this.handle = handle;
		this.lib = IGitObjectLibrary.Instance;
	}
	
	/**
	 * 获取person log的名称
	 * @return person log的名称
	 */
	public String getName() {
		return this.lib.git_person_log_name(this.handle);
	}
	
	/**
	 * 获取person log的mail
	 * @return person log的mail
	 */
	public String getMail() {
		return this.lib.git_person_log_mail(this.handle);
	}
	
	/**
	 * 获取person log的时区
	 * @return person log的时区
	 */
	public String getTimeZone() {
		return this.lib.git_person_timezone(this.handle);
	}
	
	/**
	 * 获取person log的时间戳
	 * @return person log的时间戳
	 */
	public long getTimestamp() {
		return this.lib.git_person_log_timestamp(this.handle);
	}
}
