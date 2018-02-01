package indi.gscienty.gitterkid.webapi.aspect;

import javax.servlet.http.HttpServletRequest;

import org.aspectj.lang.ProceedingJoinPoint;
import org.aspectj.lang.annotation.Around;
import org.aspectj.lang.annotation.Aspect;
import org.springframework.stereotype.Component;
import org.springframework.util.AntPathMatcher;
import org.springframework.web.servlet.HandlerMapping;

import indi.gscienty.gitterkid.repo.GitBranch;
import indi.gscienty.gitterkid.repo.Market;
import indi.gscienty.gitterkid.repo.Repository;
import indi.gscienty.gitterkid.services.GitCommitService;
import indi.gscienty.gitterkid.webapi.App;
import indi.gscienty.gitterkid.webapi.entities.CommitServiceWrapper;
import indi.gscienty.gitterkid.webapi.entities.MarketWrapper;
import indi.gscienty.gitterkid.webapi.entities.RepositoryWrapper;
import indi.gscienty.gitterkid.webapi.services.RepositoryService;


@Aspect
@Component
public class RepositoryControllerAspect {
	
	@Around("execution(* indi.gscienty.gitterkid.webapi.controllers.RepositoryController.getRepositories(..))")
	public Object aroundGetRepositories(
			ProceedingJoinPoint point) throws Throwable {
		Object[] args = point.getArgs();
		
		((MarketWrapper) args[1]).setMarket(this.getRepositories(args[0].toString()));
		
		return point.proceed(args);
	}
	
	@Around("execution(* indi.gscienty.gitterkid.webapi.controllers.RepositoryController.getBranches(..))")
	public Object aroundGetBranches(ProceedingJoinPoint point) throws Throwable {
		Object[] args = point.getArgs();
		
		((RepositoryWrapper) args[2]).setRepository(this.getRepository(args[0].toString(), args[1].toString()));
		
		return point.proceed(args);
	}

	@Around("execution(* indi.gscienty.gitterkid.webapi.controllers.RepositoryController.getTree(..))")
	public Object aroundGetTree(ProceedingJoinPoint point) throws Throwable {
		Object[] args = point.getArgs();
		
		args[4] = this.getGitFSPathByServletRequest((HttpServletRequest) args[0]);
		GitBranch branch = this.getBranch(args[1].toString(), args[2].toString(), args[3].toString());
		if (branch == null) {
			((CommitServiceWrapper) args[5]).setService(null);
		}
		else {
			((CommitServiceWrapper) args[5]).setService(new GitCommitService(branch.getLastCommit()));
		}
		
		return point.proceed(args);
	}

	@Around("execution(* indi.gscienty.gitterkid.webapi.controllers.RepositoryController.getBlob*(..))")
	public Object aroundGetBlob(ProceedingJoinPoint point) throws Throwable {
		Object[] args = point.getArgs();
		
		args[4] = this.getGitFSPathByServletRequest((HttpServletRequest) args[0]);
		GitBranch branch = this.getBranch(args[1].toString(), args[2].toString(), args[3].toString());
		if (branch == null) {
			((CommitServiceWrapper) args[5]).setService(null);
		}
		else {
			((CommitServiceWrapper) args[5]).setService(new GitCommitService(branch.getLastCommit()));
		}
		
		return point.proceed(args);
	}
	
	
	private GitBranch getBranch(String marketName, String repositoryName, String branchName) {
		Repository repository = this.getRepository(marketName, repositoryName);
		if (repository == null) {
			return null;
		}
		
		return repository.getBranches().first(b -> b.getName().equals(branchName));
	}
	
	private Repository getRepository(String marketName, String repositoryName) {
		Market repositories = this.getRepositories(marketName);
		if (repositories == null || repositories.isLegal() == false) {
			return null;
		}
		
		return repositories.first(r -> r.getName().equals(repositoryName));
	}
	
	private Market getRepositories(String marketName) {
		RepositoryService service = App.getBean(RepositoryService.class);
		Market repositories = service.getMarket(marketName);
		
		return repositories;
	}
	
	private String getGitFSPathByServletRequest(HttpServletRequest request) {
		String path = (String) request.getAttribute(HandlerMapping.PATH_WITHIN_HANDLER_MAPPING_ATTRIBUTE);
		String bestMatchPattern = (String) request.getAttribute(HandlerMapping.BEST_MATCHING_PATTERN_ATTRIBUTE);
		
		return "/" + (new AntPathMatcher().extractPathWithinPattern(bestMatchPattern, path));
	}
}
