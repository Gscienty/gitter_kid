package indi.gscienty.gitterkid.webapi.controllers;

import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.validation.Valid;

import org.springframework.http.MediaType;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import indi.gscienty.gitterkid.repo.GitBlob;
import indi.gscienty.gitterkid.repo.GitObjectType;
import indi.gscienty.gitterkid.repo.GitTree;
import indi.gscienty.gitterkid.webapi.entities.CommitServiceWrapper;
import indi.gscienty.gitterkid.webapi.entities.MarketWrapper;
import indi.gscienty.gitterkid.webapi.entities.RepositoryWrapper;
import indi.gscienty.gitterkid.webapi.entities.TreeItem;

@RestController
@RequestMapping(value = { "/api/git/{market}" })
public class RepositoryController {
	
	@RequestMapping(value = { "", "/" }, method = RequestMethod.GET)
	public List<String> getRepositories(
			@PathVariable(name = "market") String market,
			MarketWrapper repositories) {
		return repositories.getMarket().filter(r -> r.getName());
	}
	
	@RequestMapping(value = {"/{repository}"}, method = RequestMethod.GET)
	public List<String> getBranches(
			@PathVariable(name = "market") String market,
			@PathVariable(name = "repository") String repositoryName,
			@Valid RepositoryWrapper repository,
			BindingResult bindingResult) {
		return repository.getRepository().getBranches().filter(b -> b.getName());
	}
	
	@RequestMapping(value = { "/{repository}/{branch}/tree/**" })
	public List<TreeItem> getTree(
			HttpServletRequest request,
			@PathVariable(name = "market") String market,
			@PathVariable(name = "repository") String repositoryName,
			@PathVariable(name = "branch") String branch,
			@Valid String path,
			@Valid CommitServiceWrapper commitService,
			BindingResult bindingResult) {
		List<TreeItem> result = commitService.getService().getTree(path).filter(i -> {
			TreeItem resultItem = new TreeItem();
			resultItem.setSignture(i.getSignture());
			
			resultItem.setType(i.getGitObjectType().name());
			if (i.getGitObjectType().equals(GitObjectType.Tree)) {
				resultItem.setName(this.constructTreeName((GitTree.TreeItem) i));
			}
			else {
				resultItem.setName(i.getName());
			}
			
			return resultItem;
		});
		
		return result;
	}
	
	private String constructTreeName(GitTree.TreeItem treeItem) {
		StringBuffer buffer = new StringBuffer(treeItem.getName());
		
		GitTree currentTree = treeItem.getTree();
		while (currentTree.count(childItem -> true) == 1
				&& currentTree.first(childItem -> true).getGitObjectType().equals(GitObjectType.Tree)) {
			GitTree.TreeItem tempTreeItem = (GitTree.TreeItem) currentTree.first(childItem -> true);
			buffer.append("/");
			buffer.append(tempTreeItem.getName());
			
			currentTree = tempTreeItem.getTree();
		}
		
		return buffer.toString();
	}

	@RequestMapping(value = { "/{repository}/{branch}/blob/**" }, method = RequestMethod.GET, consumes = { "text/plain" }, produces = { "text/plain" })
	public String getBlobText(
			HttpServletRequest request,
			@PathVariable(name = "market") String market,
			@PathVariable(name = "repository") String repositoryName,
			@PathVariable(name = "branch") String branch,
			@Valid String path,
			@Valid CommitServiceWrapper commitService,
			HttpServletResponse response,
			BindingResult bindingResult) {
		response.setContentType(MediaType.TEXT_PLAIN_VALUE);
		GitBlob blob = commitService.getService().getBlob(path);
		return new String(blob.getContent(), 0, blob.getLength());
	}
}
