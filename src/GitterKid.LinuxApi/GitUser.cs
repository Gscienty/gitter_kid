using System;

namespace GitterKid.LinuxApi
{
    public class GitUser
    {
        public string LoginName { get; set; }
        public int UserId  { get; set; }
        public int GroupId { get; set; }
        public string UserName { get; set; }
        public string HomeDirectory { get; set; }
        public string Shell { get; set; }
    }
}