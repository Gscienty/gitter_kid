using System;
using System.Linq;
using System.Collections.Generic;
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
            Assert.True(groups.Any(g => g.GroupId == 1));
            Assert.False(groups.Any(g => g.GroupId == 0));
        }

        [Fact]
        public void GroupUserViewTest()
        {
            GitGroups groups = new GitGroups();
            string ask = "";
            foreach (var group in groups)
            {
                ask += "[" + group.GroupName + "]";
                foreach (var member in group.Members)
                {
                    ask += " " + member;
                }
            }
            Assert.Equal(ask, "123333333333333333333333333333333333333333333");
        }
    }
}