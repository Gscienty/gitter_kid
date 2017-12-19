using System;
using Xunit;
using GitterKid.LinuxApi;

namespace GitterKid.LinuxApi.Test
{
    public class UserManagerTest
    {
        [Fact]
        public void CreateUserTest()
        {
            Assert.Equal(GitUser.Create("test"), 0);
        }
    }
}