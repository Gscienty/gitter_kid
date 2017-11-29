using System;
using Xunit;
using GitterKid.Service;

namespace GitterKid.Service.Test
{
    public class ConfigTest
    {
        [Theory]
        [InlineData("core", "repositoryformatversion", "0")]
        [InlineData("core", "filemode", "false")]
        [InlineData("core", "bare", "true")]
        [InlineData("core", "symlinks", "false")]
        [InlineData("core", "ignorecase", "true")]
        public void ContentTest(string segment, string key, string value)
        {
            Repository repository = new Repository(@"F:\auto\a\repository\");

            Assert.Equal(repository.Configure.Segments[segment][key], value);
        }

        [Theory]
        [InlineData("virtualSegment")]
        public void IsNotExistSegment(string segment)
        {
            Repository repository = new Repository(@"F:\auto\a\repository\");

            Assert.False(repository.Configure.Segments.ContainsKey(segment));
        }

        [Theory]
        [InlineData("core")]
        public void ExistSegment(string segment)
        {
            Repository repository = new Repository(@"F:\auto\a\repository\");

            Assert.True(repository.Configure.Segments.ContainsKey(segment));
        }

        [Theory]
        [InlineData("core", "newkey", "newvalue")]
        [InlineData("core", "newkey", "newvalue")]
        [InlineData("newCore", "newkey", "newvalue")]
        [InlineData("newCore", "newkey2", "haha")]
        [InlineData("newCore", "newkey2", "newvalue2")]
        public void AddConfig(string segment, string key, string value)
        {
            Repository repository = new Repository(@"F:\auto\a\repository\");
            repository.Configure.Upgrade(segment, key, value);
            Assert.Equal(repository.Configure.Segments[segment][key], value);
        }
    }
}
