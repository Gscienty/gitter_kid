using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using GitterKid.Service;

namespace GitterKid.Service.Extension
{
    public class RepositoryFactory
    {
        private readonly string _baseRepositoryPath;

        internal RepositoryFactory(string baseRepositoryPath)
        {
            this._baseRepositoryPath = baseRepositoryPath;
        }

        public Repository Get(string repositoryName)
            => new Repository($"{this._baseRepositoryPath}/{repositoryName}");

        public bool Exist(string repositoryName)
            => Repository.ExistRepository($"{this._baseRepositoryPath}/{repositoryName}");

        public IEnumerable<string> GetRepositoriesName()
            => this.GetRepositoriesPath()
                .Select(path =>
                {
                    string middlePath = path.Replace('\\', '/');
                    return middlePath.Substring(middlePath.LastIndexOf('/') + 1);
                });

        private IEnumerable<string> GetRepositoriesPath()
            => Directory.GetDirectories(this._baseRepositoryPath)
                .Select(path => path.Replace('\\', '/'));

        public IEnumerable<Repository> GetRepositories()
            => this.GetRepositoriesPath()
                .Select(path => new Repository(path))
                .Where(repository => repository.IsRepository);

        public IEnumerable<Repository> GetRepositories(IEnumerable<string> repositoriesName)
            => repositoriesName.Select(
                    repositoryName => new Repository($"{this._baseRepositoryPath}/{repositoryName}")
                )
                .Where(repository => repository.IsRepository);
    }
}