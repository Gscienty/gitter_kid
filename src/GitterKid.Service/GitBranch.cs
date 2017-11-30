using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace GitterKid.Service
{
    public class GitBranch
    {
        private string _repositoryPath;
        private List<BranchLog> _branchLogs;

        public string Name { get; private set; }
        public string NewestCommitSignture { get; private set; }
        public IReadOnlyList<BranchLog> BranchLogs => this._branchLogs;

        internal GitBranch(string repositoryPath, string name)
        {
            this._repositoryPath = repositoryPath;

            this.Name = name;
            this.NewestCommitSignture = File.ReadAllText($"{this._repositoryPath}/refs/heads/{name}").Trim();

            this.InitializeLogs();
        }

        public GitCommit GetNewestCommit() => 
            GitEntity.Load<GitCommit>(this._repositoryPath, this.NewestCommitSignture);

        internal static IEnumerable<string> GetBranches(string repositoryPath) => 
            Directory.GetFiles($"{repositoryPath}/refs/heads/")
                .Select(originName => originName.Substring(originName.LastIndexOf('/') + 1));
        
        private void InitializeLogs()
        {
            this._branchLogs = new List<BranchLog>();

            string logsPath = $"{this._repositoryPath}/logs/refs/heads/{this.Name}";

            if (File.Exists(logsPath) == false)
            {
                return;
            }

            foreach(string line in File.ReadAllLines(logsPath))
            {
                string[] pair = line.Split('\t');

                string[] firstPair = pair[0].Split(' ');
                int cutPosition = pair[1].IndexOf(':');
                string originActive = pair[1].Substring(0, cutPosition);
                string message = pair[1].Substring(cutPosition + 2);

                BranchLogActive active = BranchLogActive.Unknow;

                if (originActive.Equals("branch"))
                {
                    active = BranchLogActive.Branch;
                }
                else if (originActive.Equals("commit"))
                {
                    active = BranchLogActive.Commit;
                }
                else if (originActive.Equals("commit (initial)"))
                {
                    active = BranchLogActive.InitialCommit;
                }
                else if (originActive.Equals("merge branch"))
                {
                    active = BranchLogActive.MergeBranch;
                }

                this._branchLogs.Add(new BranchLog
                {
                    Signture = firstPair[1],
                    Active = active,
                    Committer = new GitCommit.CommitEventLog
                    {
                        Name = firstPair[2],
                        Mail = firstPair[3].Substring(1, firstPair[3].Length - 2),
                        Timestamp = long.Parse(firstPair[4]),
                        TimeZone = firstPair[5]
                    },
                    Message = message
                });
            }
        }

        public class BranchLog
        {
            public string Signture { get; internal set; }
            public BranchLogActive Active { get; internal set; }
            public GitCommit.CommitEventLog Committer { get; internal set; }
            public string Message { get; internal set; }

            internal BranchLog() { }
        }

        public enum BranchLogActive
        {
            Unknow,
            InitialCommit,
            Commit,
            MergeBranch,
            Branch
        }
    }
}