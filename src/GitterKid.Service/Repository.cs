using System.IO;
using System.Collections.Generic;

namespace GitterKid.Service
{
    public class Repository
    {
        public RepositoryConfig Config { get; private set; }

        public string Path { get; private set; }
        public bool ExistRepository => Directory.Exists(this.Path);

        public Repository(string path)
        {
            this.Path = path;

            this.Config = new RepositoryConfig(path);
        }
    }
}