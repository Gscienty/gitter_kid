using System.Collections.Generic;
using System.Linq;

namespace GitterKid.Service.Extension
{
    public static class GitBranchExtension
    {
        public static GitBranch.BranchLog GetLastestCommit(this GitBranch branch)
        {
            if (branch.BranchLogs.Count() == 0)
            {
                return null;
            }
            else
            {
                return branch.BranchLogs.Last();
            }
        }

        public static IEnumerable<GitCommitter> GetAllCommitters(this GitBranch branch)
        {
            Dictionary<string, string> committerSet = new Dictionary<string, string>();

            foreach (var log in branch.BranchLogs)
            {
                if (committerSet.ContainsKey(log.CommitEvent.Mail) == false)
                {
                    committerSet.Add(log.CommitEvent.Mail, log.CommitEvent.Name);
                }
            }

            return committerSet.Select(pair => new GitCommitter
            {
                Mail = pair.Key,
                Name = pair.Value
            });
        }
    }
}