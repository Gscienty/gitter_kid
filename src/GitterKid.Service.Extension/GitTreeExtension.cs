using GitterKid.Service;

namespace GitterKid.Service.Extension
{
    public static class GitTreeExtension
    {
        public static bool ExistTree(this GitTree tree, string name)
        {
            if (tree.Exist(name) == false)
            {
                return false;
            }

            return tree.GetFileType(name) == 40000;
        }

        public static bool ExistBlob(this GitTree tree, string name)
        {
            if (tree.Exist(name) == false)
            {
                return false;
            }

            return tree.GetFileType(name) == 10064;
        }
    }
}