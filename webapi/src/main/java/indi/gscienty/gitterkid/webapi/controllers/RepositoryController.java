package indi.gscienty.gitterkid.webapi.controllers;

import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.validation.Valid;

import org.springframework.validation.BindingResult;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import indi.gscienty.gitterkid.webapi.entities.CommitServiceWrapper;
import indi.gscienty.gitterkid.webapi.entities.MarketWrapper;
import indi.gscienty.gitterkid.webapi.entities.RepositoryWrapper;
import indi.gscienty.gitterkid.webapi.entities.TreeItem;

@RestController
@RequestMapping(value = { "/{market}" })
public class RepositoryController {
	
	@RequestMapping(value = { "", "/" }, method = RequestMethod.GET)
	public List<String> getRepositories(
			@PathVariable(name = "market") String market,
			MarketWrapper repositories) {
		return repositories.getMarket().filter(r -> r.getName());
	}
	
	@RequestMapping(value = {"/{repository}"}, method = RequestMethod.GET )
	public List<String> getBranches(
			@PathVariable(name = "market") String market,
			@PathVariable(name = "repository") String repositoryName,
			@Valid RepositoryWrapper repository,
			BindingResult bindingResult) {
		return repository.getRepository().getBranches().filter(b -> b.getName());
	}
	
	@RequestMapping(value = { "/{repository}/{branch}/tree/**" })
	public List<Object> getTree(
			HttpServletRequest request,
			@PathVariable(name = "market") String market,
			@PathVariable(name = "repository") String repositoryName,
			@PathVariable(name = "branch") String branch,
			@Valid String path,
			@Valid CommitServiceWrapper commitService,
			BindingResult bindingResult) {
		return commitService.getService().getTree(path).filter(i -> {
			TreeItem result = new TreeItem();
			
			result.setName(i.getName());
			result.setType(i.getGitObjectType().name());
			
			return result;
		});
	}
}
