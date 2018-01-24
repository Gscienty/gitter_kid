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
	
	@Around("execution(* indi.gscienty.gitterkid.webapi.controllers.RepositoryController.getRepositories(..)) && args(market, ..)")
	public Object aroundGetRepositories(
			ProceedingJoinPoint point,
			String market) throws Throwable {
		Object[] args = point.getArgs();
		
		RepositoryService service = App.getBean(RepositoryService.class);
		((MarketWrapper) args[1]).setMarket(service.getMarket(market));
		
		return point.proceed(args);
	}
	
	@Around("execution(* indi.gscienty.gitterkid.webapi.controllers.RepositoryController.getBranches(..))")
	public Object aroundGetBranches(ProceedingJoinPoint point) throws Throwable {
		Object[] args = point.getArgs();

		RepositoryService service = App.getBean(RepositoryService.class);
		Market repositories = service.getMarket(args[0].toString());
		Repository repository = repositories.first(r -> r.getName().equals(args[1].toString()));
		((RepositoryWrapper) args[2]).setRepository(repository);
		
		return point.proceed(args);
	}

	@Around("execution(* indi.gscienty.gitterkid.webapi.controllers.RepositoryController.getTree(..))")
	public Object aroundGetTree(ProceedingJoinPoint point) throws Throwable {
		Object[] args = point.getArgs();
		
		args[4] = this.getPathByServletRequest((HttpServletRequest) args[0]);
		
		RepositoryService repositoryService = App.getBean(RepositoryService.class);
		Market repositories = repositoryService.getMarket(args[1].toString());
		
		Repository repository = repositories.first(r -> r.getName().equals(args[2].toString()));
		GitBranch branch = repository.getBranches().first(b -> b.getName().equals(args[3].toString()));
		
		((CommitServiceWrapper) args[5]).setService(new GitCommitService(branch.getLastCommit()));
		
		return point.proceed(args);
	}

	@Around("execution(* indi.gscienty.gitterkid.webapi.controllers.RepositoryController.getBlob(..))")
	public Object aroundGetBlob(ProceedingJoinPoint point) throws Throwable {
		Object[] args = point.getArgs();
		
		args[4] = this.getPathByServletRequest((HttpServletRequest) args[0]);
		
		RepositoryService repositoryService = App.getBean(RepositoryService.class);
		Market repositories = repositoryService.getMarket(args[1].toString());
		
		Repository repository = repositories.first(r -> r.getName().equals(args[2].toString()));
		GitBranch branch = repository.getBranches().first(b -> b.getName().equals(args[3].toString()));
		
		((CommitServiceWrapper) args[5]).setService(new GitCommitService(branch.getLastCommit()));
		
		return point.proceed(args);
	}
	
	private String getPathByServletRequest(HttpServletRequest request) {
		String path = (String) request.getAttribute(HandlerMapping.PATH_WITHIN_HANDLER_MAPPING_ATTRIBUTE);
		String bestMatchPattern = (String) request.getAttribute(HandlerMapping.BEST_MATCHING_PATTERN_ATTRIBUTE);
		
		return "/" + (new AntPathMatcher().extractPathWithinPattern(bestMatchPattern, path));
	}
}
