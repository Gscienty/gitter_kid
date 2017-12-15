using System.IO;
using System.Runtime.InteropServices;

namespace GitterKid.LinuxApi
{
    public class Folder
    {
        private DirectoryInfo _directoryInformation;
        public string Path { get; private set; }

        public Folder(string path)
        {
            this.Path = path;

            if (Directory.Exists(path) == false)
            {
                Directory.CreateDirectory(path);
            }
        }
    }
}