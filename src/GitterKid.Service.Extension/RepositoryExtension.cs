using System.Collections.Generic;
using System.Linq;

namespace GitterKid.Service.Extension
{
    public static class RepositoryExtension
    {
        public static GitBranch GetDefaultBranch(this Repository repository)
        {
            if (repository.Branches.ContainsKey("master"))
            {
                return repository.Branches["master"];
            }
            else
            {
                foreach(var pair in repository.Branches)
                {
                    return pair.Value;
                }
            }

            return null;
        }
        
        public static Dictionary<string, IEnumerable<GitBranch.BranchLog>> GetAllCommitLogs(this Repository repository)
        {
            Dictionary<string, IEnumerable<GitBranch.BranchLog>> result = new Dictionary<string, IEnumerable<GitBranch.BranchLog>>();

            foreach (var branch in repository.Branches)
            {
                List<GitBranch.BranchLog> logs = new List<GitBranch.BranchLog>();

                foreach (var log in branch.Value.BranchLogs)
                {
                    logs.Add(log);
                }

                result.Add(branch.Key, logs);
            }

            return result;
        }

        public static IEnumerable<GitCommitter> GetAllCommitters(this Repository repository)
        {
            Dictionary<string, string> committerSet = new Dictionary<string, string>();


            foreach (var branch in repository.Branches)
            {
                foreach(var log in branch.Value.BranchLogs)
                {
                    if (committerSet.ContainsKey(log.CommitEvent.Mail) == false)
                    {
                        committerSet.Add(log.CommitEvent.Mail, log.CommitEvent.Name);
                    }
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