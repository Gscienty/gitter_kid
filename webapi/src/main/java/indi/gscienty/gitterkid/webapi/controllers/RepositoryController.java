package indi.gscienty.gitterkid.webapi.controllers;

import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.validation.Valid;

import org.springframework.http.HttpStatus;
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
			MarketWrapper repositories,
			HttpServletResponse response) {
		if (repositories.isExist() == false) {
			response.setStatus(HttpStatus.NOT_FOUND.value());
			return null;
		}
		return repositories.getMarket().filter(r -> r.getName());
	}
	
	@RequestMapping(value = {"/{repository}"}, method = RequestMethod.GET)
	public List<String> getBranches(
			@PathVariable(name = "market") String market,
			@PathVariable(name = "repository") String repositoryName,
			@Valid RepositoryWrapper repository,
			HttpServletResponse response,
			BindingResult bindingResult) {
		if (repository.isExist() == false) {
			response.setStatus(HttpStatus.NOT_FOUND.value());
			return null;
		}
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
			HttpServletResponse response,
			BindingResult bindingResult) {
		if (commitService.isExist() == false) {
			response.setStatus(HttpStatus.NOT_FOUND.value());
			return null;
		}
		List<TreeItem> result = commitService.getService().getTree(path).filter(i -> {
			TreeItem resultItem = new TreeItem();
			resultItem.setSignture(i.getSignture());
			resultItem.setPath(path + i.getName());
			resultItem.setType(i.getGitObjectType().name());
			// resultItem.setMessage(commitService.getService().getNewestCommitMessage(resultItem.getPath()));
			resultItem.setName(i.getGitObjectType().equals(GitObjectType.Tree)
					? this.constructTreeName((GitTree.TreeItem) i)
					: i.getName());
			
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
		if (commitService.isExist() == false) {
			response.setStatus(HttpStatus.NOT_FOUND.value());
			return null;
		}
		response.setContentType(MediaType.TEXT_PLAIN_VALUE);
		GitBlob blob = commitService.getService().getBlob(path);
		return new String(blob.getContent(), 0, blob.getLength());
	}
}
