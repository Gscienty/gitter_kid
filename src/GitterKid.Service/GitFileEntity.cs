using System;
using System.Text;

namespace GitterKid.Service
{
    public class GitFileEntity : GitEntity
    {
        public GitFileEntity(string repositoryPath, string signture) : base(repositoryPath, signture) { }

        public string TextContent() => Encoding.UTF8.GetString(this.Body);
    }
}