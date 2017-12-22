using System;
using System.Linq;
using Xunit;
using GitterKid.LinuxApi;

namespace GitterKid.LinuxApi.Test
{
    public class UserManagerTest
    {
        [Fact]
        public void UserViewTest()
        {
            GitUsers users = new GitUsers();

            Assert.True(users.Any(u => u.UserId == 1000));
            Assert.False(users.Any(u => u.UserId == 1001));
        }

        [Fact]
        public void GroupViewTest()
        {
            GitGroups groups = new GitGroups();

            Assert.True(groups.Any(g => g.GroupId == 1000));
        }
    }
}