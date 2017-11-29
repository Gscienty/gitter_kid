using System;
using Xunit;
using GitterKid.Service;

namespace GitterKid.Service.Test
{
    public class FileEntityTest
    {
        [Theory]
        [InlineData("56a6051ca2b02b04ef92d5150c9ef600403cb1de", "1")]
        public void FileContentTest(string signture, string value)
        {
            Repository repository = new Repository(@"F:\auto\a\repository\");
            Assert.Equal(repository.Entity<GitFileEntity>(signture).TextContent(), value);
        }
    }
}
