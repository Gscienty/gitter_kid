#ifndef _GIT_FSI_CONTENT_
#define _GIT_FSI_CONTENT_

#include "define.h"
#include <vector>
#include <string>

namespace gitterKid {
    namespace fsi {

        enum objectType { unknowType, blobType, commitType, treeType, tagType, ofsDeltaType, refDeltaType };

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
        
        class content {
            protected:
                commitPersonLog parsePersonLog(std::vector<byte>::iterator begin, std::vector<byte>::iterator end);
            public:
                virtual objectType getType() const = 0;
        };
    }
}

#endif