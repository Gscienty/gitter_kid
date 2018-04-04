#ifndef _GIT_FSI_COMMIT_
#define _GIT_FSI_COMMIT_

#include "content.h"

namespace gitterKid {
    namespace fsi {
        class commitPersonLog {
            private:
                std::string name;
                std::string mail;
                long timestamp;
                std::string timeZone;
            public:
                commitPersonLog() { }
                commitPersonLog(std::string name, std::string mail, long timestamp, std::string timeZone)
                    : name(name), mail(mail), timestamp(timestamp), timeZone(timeZone) { }
                
                std::string getName() const { return this->name; }
                std::string getMail() const { return this->mail; }
                long getTimestamp() const { return this->timestamp; }
                std::string getTimeZone() const { return this->timeZone; }
        };

        class commitBody {
            private:
                std::vector<std::string> parents;
                std::string treeSign;
                std::string message;
                commitPersonLog author;
                commitPersonLog committer;
            public:
                commitBody() { }

                std::vector<std::string> getParents() const { return this->parents; }
                std::string getTreeSignture() const { return this->treeSign; }
                std::string getMessage() const { return this->message; }
                commitPersonLog getAuthor() const { return this->author; }
                commitPersonLog getCommitter() const { return this->committer; }

                void addParent(std::string parent) { this->parents.push_back(parent); }
                void setTreeSignture(std::string treeSign) { this->treeSign = treeSign; }
                void setMessage(std::string message) { this->message = message; }
                void setAuthor(commitPersonLog author) { this->author = author; }
                void setCommitter(commitPersonLog committer) { this->committer = committer; }

        };

        class commit : public content {
            private:
                commitBody &body;

                commitPersonLog parsePersonLog(std::vector<byte>::iterator begin, std::vector<byte>::iterator end);
            public:
                objectType getType() const { return objectType::commitType; }
                commit(commitBody &body, std::vector<byte>::iterator spliter, std::vector<byte>::iterator end);
                commit(const content &origin);
                commitBody get() const { return this->body; }
        };
    }
}

#endif