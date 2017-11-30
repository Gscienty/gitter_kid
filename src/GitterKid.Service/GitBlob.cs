using System;
using System.Text;

namespace GitterKid.Service
{
    public class GitBlob : GitEntity
    {
        public string TextContent { get; private set; }
        public byte[] BlobContent => this.Body;

        internal override void PackageBody()
        {
            this.TextContent = Encoding.UTF8.GetString(this.Body);
        }
    }
}