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
        [InlineData("core", "bare", "false")]
        [InlineData("core", "symlinks", "false")]
        [InlineData("core", "ignorecase", "true")]
        [InlineData("remote \"origin\"", "url", "git@github.com:Gscienty/GitterKid.git")]
        [InlineData("remote \"origin\"", "fetch", "+refs/heads/*:refs/remotes/origin/*")]
        [InlineData("branch \"master\"", "remote", "origin")]
        [InlineData("branch \"master\"", "merge", "refs/heads/master")]
        public void ContentTest(string segment, string key, string value)
        {
            Repository repository = new Repository(@"../../../../../.git/");

            Assert.Equal(repository.Configure.Segments[segment][key], value);
        }

        [Theory]
        [InlineData("virtualSegment")]
        public void IsNotExistSegment(string segment)
        {
            Repository repository = new Repository(@"../../../../../.git/");

            Assert.False(repository.Configure.Segments.ContainsKey(segment));
        }

        [Theory]
        [InlineData("core")]
        public void ExistSegment(string segment)
        {
            Repository repository = new Repository(@"../../../../../.git/");

            Assert.True(repository.Configure.Segments.ContainsKey(segment));
        }
    }
}
