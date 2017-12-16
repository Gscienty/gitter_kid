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
            Assert.Equal(Folder.Readable("/home/ant"), 0);
        }
    }
}
