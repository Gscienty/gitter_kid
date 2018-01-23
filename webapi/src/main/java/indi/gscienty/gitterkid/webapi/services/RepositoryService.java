package indi.gscienty.gitterkid.webapi.services;

import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Service;

import indi.gscienty.gitterkid.repo.Market;


@Service
@Scope(value = "session")
public class RepositoryService {
	public Market getMarket(String marketName) {
		if (marketName.equals("default") == false) {
			return null;
		}
		
		Market result = new Market("/home/ant");
		if (result.isLegal() == false) {
			return null;
		}
		
		return result;
	}
}
