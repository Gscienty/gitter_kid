using System;
using System.Text;

namespace GitterKid.Service
{
    public class GitTextEntity : GitEntity
    {
        public string TextContent { get; private set; }

        internal override void Initialize()
        {
            this.TextContent = Encoding.UTF8.GetString(this.Body);
        }
    }
}