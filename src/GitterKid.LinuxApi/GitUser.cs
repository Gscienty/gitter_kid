using System.Runtime.InteropServices;

namespace GitterKid.LinuxApi
{
    public class GitUser
    {
        public string Name { get; private set; }

        private GitUser(string name)
        {
            this.Name = name;
        }

        public static int Create(string username)
        {
        }
    }
}