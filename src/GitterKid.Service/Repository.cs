using System;
using System.IO;
using System.Collections.Generic;

namespace GitterKid.Service
{
    public class Repository
    {
        private Dictionary<string, GitBranch> _branches;
        public RepositoryConfig Configure { get; private set; }
        public string Path { get; private set; }
        public IReadOnlyDictionary<string, GitBranch> Branches => this._branches;

        public Repository(string path)
        {
            this.Path = path;
            if (Repository.ExistRepository(path))
            {
                this.Initialize();
            }
            else
            {
                this.CreateRepository();
            }
        }

        public T Entity<T>(string signture) where T : GitEntity, new()
        {
            return GitEntity.Load<T>(this.Path, signture);
        }

        public static bool ExistRepository(string path) => Directory.Exists(this.Path);

        public bool ExistBranch(string name) => this._branches.ContainsKey(name);

        private void CreateRepository()
        {
            throw new NotImplementedException();
        }

        private void Initialize()
        {
            this.InitializeConfigure();
            this.InitializeBranch();
        }

        private void InitializeConfigure()
            => this.Configure = new RepositoryConfig(this.Path);
        private void InitializeBranch()
        {
            this._branches = new Dictionary<string, GitBranch>();

            foreach(string branchName in GitBranch.GetBranches(this.Path))
            {
                this._branches.Add(branchName, new GitBranch(this.Path, branchName));
            }
        }
    }
}