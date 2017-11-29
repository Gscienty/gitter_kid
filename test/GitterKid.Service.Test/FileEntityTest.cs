using System;
using Xunit;
using GitterKid.Service;

namespace GitterKid.Service.Test
{
    public class FileEntityTest
    {
        [Theory]
        [InlineData("bf3f8339277ac8e1996c14c89012afae40bfa9b9")]
        public void TreeEntityTest(string signture)
        {
            Repository repository = new Repository(@"../../../../../.git/");
            Assert.True(repository.Entity<GitTreeEntity>(signture).Exist(".gitignore"));
            Assert.True(repository.Entity<GitTreeEntity>(signture).Exist("src"));
            Assert.True(repository.Entity<GitTreeEntity>(signture).Exist("test"));

            Assert.False(repository.Entity<GitTreeEntity>(signture).Exist("is not exist"));

            Assert.True(repository.Entity<GitTreeEntity>(signture).Open<GitTreeEntity>("test").Exist("GitterKid.Service.Test"));
        }
    }
}
