package indi.gscienty.gitterkid.repo;

import java.util.List;
import java.util.function.Function;
import java.util.function.Predicate;

public interface IQueryable<T> {
	/**
	 * 判断集合内是否包含满足某条件的元素
	 * @param predicate 需满足的条件
	 * @return 是否存在满足某条件的元素
	 */
	boolean any(Predicate<T> predicate);
	/**
	 * 判断集合内的元素是否都满足某条件
	 * @param predicate 需满足的条件
	 * @return 集合内的元素是否都满足某条件
	 */
	boolean all(Predicate<T> predicate);
	/**
	 * 获取满足某条件的第一个元素
	 * @param predicate 需满足的某条件
	 * @return 第一个满足某条件的元素，若不尊在则返回null
	 */
	T first(Predicate<T> predicate);
	/**
	 * 将集合映射成为另一个集合
	 * @param transfer 映射子
	 * @return 集合
	 */
	<R> List<R> filter(Function<T, R> transfer);
}
