using System.Collections.Generic;
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
    }
}