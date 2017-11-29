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

        public T Entity<T>(string signture) where T : GitEntity
        {
            return Activator.CreateInstance(typeof(T), new object[] { this.Path, signture }) as T;
        }
    }
}