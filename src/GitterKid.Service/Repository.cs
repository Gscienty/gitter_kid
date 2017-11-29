using System;
using System.IO;
using System.Collections.Generic;

namespace GitterKid.Service
{
    public class Repository
    {
        public RepositoryConfig Configure { get; private set; }

        public string Path { get; private set; }
        public bool ExistRepository => Directory.Exists(this.Path);

        public Repository(string path)
        {
            this.Path = path;

            this.Configure = new RepositoryConfig(path);
        }

        public T Entity<T>(string signture) where T : GitEntity, new()
        {
            return GitEntity.Load<T>(this.Path, signture);
        }
    }
}