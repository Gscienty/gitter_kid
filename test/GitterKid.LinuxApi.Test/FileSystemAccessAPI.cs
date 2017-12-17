using System;
using Xunit;
using GitterKid.LinuxApi;


namespace GitterKid.LinuxApi.Test
{
    public class FileSystemAccessAPI
    {
        [Fact]
        public void Test1()
        {
            Folder folder = new Folder("./test");

            Assert.True(folder.RepositoryInit(""));
        }
    }
}
