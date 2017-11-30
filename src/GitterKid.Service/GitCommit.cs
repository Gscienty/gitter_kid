using System;
using System.Text;
using System.Collections.Generic;

namespace GitterKid.Service
{
    public class GitCommit : GitEntity
    {
        private List<string> _parentSignture;

        public string TreeSignture { get; private set; }
        public string Message { get; private set; }
        public IEnumerable<string> ParentSignture => this._parentSignture;
        public CommitEventLog Author { get; private set; }
        public CommitEventLog Committer { get; private set; }

        public GitTree GetTree() => GitEntity.Load<GitTree>(this._repositoryPath, this.TreeSignture);

        public GitCommit GetParent(int position) => GitEntity.Load<GitCommit>(
            this._repositoryPath,
            this._parentSignture[position]
        );

        internal override void PackageBody()
        {
            string[] commitLines = Encoding.UTF8.GetString(this.Body).Split('\n');
            this._parentSignture = new List<string>();
            bool isMessage = false;
            foreach(string line in commitLines)
            {
                if (isMessage)
                {
                    this.Message += $"{line}\n";
                }
                else if (line == "")
                {
                    isMessage = true;
                }
                else if (line.StartsWith("tree"))
                {
                    this.TreeSignture = line.Split(' ')[1];
                }
                else if (line.StartsWith("parent"))
                {
                    this._parentSignture.Add(line.Split(' ')[1]);
                }
                else if (line.StartsWith("author"))
                {
                    string[] units = line.Split(' ');
                    this.Author = new CommitEventLog
                    {
                        Name = units[1],
                        Mail = units[2].Substring(1, units[2].Length - 2),
                        Timestamp = long.Parse(units[3]),
                        TimeZone = units[4]
                    };
                }
                else if (line.StartsWith("commiter"))
                {
                    string[] units = line.Split(' ');
                    this.Committer = new CommitEventLog
                    {
                        Name = units[1],
                        Mail = units[2].Substring(1, units[2].Length - 2),
                        Timestamp = long.Parse(units[3]),
                        TimeZone = units[4]
                    };
                }
            }
        }

        public sealed class CommitEventLog
        {
            public string Name { get; internal set; }
            public string Mail { get; internal set; }
            public long Timestamp { get; internal set; }
            public string TimeZone { get; internal set; }

            internal CommitEventLog() { }
        }
    }
}