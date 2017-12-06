using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Filters;
using Microsoft.AspNetCore.Http;
using GitterKid.Service;
using GitterKid.Service.Extension;

namespace GitterKid.WebApi
{
    [Route("api/repository")]
    public class GitRepositoryController : Controller
    {
        private readonly RepositoryFactory _repositoryFactory;

        public GitRepositoryController(RepositoryFactory repositoryFactory)
        {
            this._repositoryFactory = repositoryFactory;
        }

        [HttpGet("{repositoryName}/branch")]
        public IActionResult Branch([FromRoute(Name = "repositoryName")] string repositoryName)
        {
            if (this._repositoryFactory.Exist(repositoryName) == false)
            {
                return StatusCode(StatusCodes.Status404NotFound);
            }

            Repository repository = this._repositoryFactory.Get(repositoryName);

            return Json(repository.Branches.Select(branch => branch.Key));
        }

        [HttpGet("query")]
        public IActionResult Query([FromQuery(Name = "keyword")] string keyword)
        {

            IEnumerable<Repository> repositories;
            if (string.IsNullOrEmpty(keyword))
            {
                repositories = this._repositoryFactory.GetRepositories();
            }
            else
            {
                repositories = this._repositoryFactory
                    .GetRepositories(
                        this._repositoryFactory.GetRepositoriesName()
                            .Where(name => name.Contains(keyword))
                    )
                    .Where(repository => repository.IsRepository);
            }

            return Json(repositories.Select(repository => new
            {
                Name = repository.Name
            }));
        }
    }
}