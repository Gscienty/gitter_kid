using System;
using Xunit;
using GitterKid.LinuxApi;

namespace GitterKid.LinuxApi.Test
{
    public class UserManagerTest
    {
        [Fact]
        public void TestPasswdRead()
        {
            GitUsers users = new GitUsers();
            Assert.Equal(users.FirstLine(), "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        }
    }
}