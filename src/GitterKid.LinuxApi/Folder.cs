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

        public static int Readable(string filepath)
            => NativeMethod.Readable(filepath);

        internal static class NativeMethod
        {
            [DllImport(
                "libgkid.so",
                CallingConvention = CallingConvention.Cdecl,
                EntryPoint = "access_file_readable"
            )]
            internal extern static int Readable(string filepath);

            [DllImport(
                "libgkid.so",
                CallingConvention = CallingConvention.Cdecl,
                EntryPoint = "access_file_writable"
            )]
            internal extern static int Writable(string filepath);

            [DllImport(
                "libgkid.so",
                CallingConvention = CallingConvention.Cdecl,
                EntryPoint = "access_file_executable"
            )]
            internal extern static int Executable(string filepath);
        }
    }
}