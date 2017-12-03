using System;
using System.Linq;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Filters;
using Microsoft.AspNetCore.Http;
using GitterKid.Service;
using GitterKid.Service.Extension;

namespace GitterKid.WebApi.Controllers
{
    [Route("api/{repository}/fs-{flag}")]
    public class GitFileSystemController : Controller
    {
        private readonly RepositoryFactory _repositoryFactory;

        public GitFileSystemController(RepositoryFactory repositoryFactory)
        {
            this._repositoryFactory = repositoryFactory;
        }

        [HttpGet("blob/{*path}")]
        [PreFoundRoot]
        public IActionResult GetBlob(GitTree root, string path)
        {
            if (string.IsNullOrEmpty(path))
            {
                return StatusCode(StatusCodes.Status404NotFound);
            }

            string[] pathNodes = path.Split('/', StringSplitOptions.RemoveEmptyEntries);
            int folderNodesCount = pathNodes.Count() - 1;

            GitTree currentTree = root;
            for (int i = 0; i < folderNodesCount; i++)
            {
                if (currentTree.ExistTree(pathNodes[i]))
                {
                    currentTree = currentTree.Open<GitTree>(pathNodes[i]);
                }
                else
                {
                    return StatusCode(StatusCodes.Status404NotFound);
                }
            }

            string fileName = pathNodes[folderNodesCount];
            if (currentTree.ExistBlob(fileName))
            {
                return Json(currentTree.Open<GitBlob>(fileName).TextContent);
            }
            else
            {
                return StatusCode(StatusCodes.Status404NotFound);
            }
        }

        [HttpGet("tree/{*path}")]
        [PreFoundRoot]
        public IActionResult GetFolder(GitTree root, string path)
        {
            GitTree currentTree = root;

            if (string.IsNullOrEmpty(path) == false)
            {
                foreach(string folderName in path.Split('/', StringSplitOptions.RemoveEmptyEntries))
                {
                    if (currentTree.ExistTree(folderName))
                    {
                        currentTree = currentTree.Open<GitTree>(folderName);
                    }
                    else
                    {
                        return StatusCode(StatusCodes.Status404NotFound);
                    }
                }

            }
            return Json(currentTree.TreeContent
                .Select(file => new
                {
                    Name = file.Key,
                    Type = file.Value.Type
                })
            );

        }
        private sealed class PreFoundRootAttribute : ActionFilterAttribute
        {
            public override void OnActionExecuting(ActionExecutingContext context)
            {
                Repository repository = this.GetRepository(context);

                if (repository == null)
                {
                    return ;
                }

                this.DistinguishType(repository, context);
            }

            public void DistinguishCommitType(
                Repository repository,
                string commitSignture,
                ActionExecutingContext context)
                => context.ActionArguments["root"] = repository
                    .Entity<GitCommit>(commitSignture)
                    .GetTree();

            public void DistinguishBranchType(
                Repository repository,
                string branchName,
                ActionExecutingContext context)
                => context.ActionArguments["root"] = repository
                    .Branches[branchName]
                    .GetNewestCommit()
                    .GetTree();

            private void DistinguishType(Repository repository, ActionExecutingContext context)
            {
                string flag = context.RouteData.Values["flag"].ToString();
                
                if (repository.ExistBranch(flag))
                {
                    this.DistinguishBranchType(repository, flag, context);
                }
                else if (flag.Length == 40 && repository.ExistEntity(flag))
                {
                    this.DistinguishCommitType(repository, flag, context);
                }
                else
                {
                    context.Result = new StatusCodeResult(StatusCodes.Status404NotFound);
                }
            }

            private Repository GetRepository(ActionExecutingContext context)
            {
                GitFileSystemController currentController = context.Controller as GitFileSystemController;

                string repositoryName = context.RouteData.Values["repository"].ToString();
                if (currentController._repositoryFactory.Exist(repositoryName) == false)
                {
                    context.Result = new StatusCodeResult(StatusCodes.Status404NotFound);
                    return null;
                }

                return currentController._repositoryFactory.Get(repositoryName);
            }
        }
    }
}