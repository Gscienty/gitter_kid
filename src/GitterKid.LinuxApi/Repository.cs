using System.IO;
using System.Runtime.InteropServices;

namespace GitterKid.LinuxApi
{
    public class Repository
    {
        public string Path { get; private set; }

        public Repository(string path)
        {
            path = path.Replace('\\', '/').TrimEnd('/');
            this.Path = path;
        }

        public bool RepositoryInit(string description)
        {
            if (Directory.Exists(this.Path) == true)
            {
                return false;
            }

            return NativeMethod.ReopsitoryInit(this.Path, description) == 0;
        }

        public bool Readable(string filepath)
        {
            return NativeMethod.Readable($"{this.Path}/{filepath}") == 0;
        }

        public bool Writable(string filepath)
        {
            return NativeMethod.Writable($"{this.Path}/{filepath}") == 0;
        }

        public bool Executable(string filepath)
        {
            return NativeMethod.Executable($"{this.Path}/{filepath}") == 0;
        } 

        internal static class NativeMethod
        {
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "access_file_readable")]
            internal extern static int Readable(string filepath);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "access_file_writable")]
            internal extern static int Writable(string filepath);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "access_file_executable")]
            internal extern static int Executable(string filepath);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "repository_init")]
            internal extern static int ReopsitoryInit(string path, string descrption);
        }
    }
}