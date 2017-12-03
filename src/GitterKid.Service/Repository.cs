using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;

namespace GitterKid.Service
{
    public class Repository
    {
        private Dictionary<string, GitBranch> _branches;
        public RepositoryConfig Configure { get; private set; }
        public string Path { get; private set; }
        public bool IsRepository { get; private set; }
        public string Name { get; private set; }
        public IReadOnlyDictionary<string, GitBranch> Branches => this._branches;

        public Repository(string path)
        {
            this.Path = path.Replace('\\', '/');
            this.Name = this.Path.Substring(this.Path.LastIndexOf('/') + 1);
            if (Repository.ExistRepository(this.Path))
            {
                try
                {
                    this.Initialize();
                    this.IsRepository = true;
                }
                catch
                {
                    this.IsRepository = false;
                }
            }
            else
            {
                this.IsRepository = false;
            }
        }

        public bool ExistEntity(string signture)
            => GitEntity.StaticExistEntity(this.Path, signture);

        public T Entity<T>(string signture) where T : GitEntity, new()
        {
            return GitEntity.Load<T>(this.Path, signture);
        }

        public static bool ExistRepository(string path) => Directory.Exists(path);

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