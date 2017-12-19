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
            Repository repository = new Repository("./test");

            Assert.True(repository.RepositoryInit(""));
        }
    }
}
