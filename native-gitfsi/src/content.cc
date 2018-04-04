#include "content.h"
#include <vector>
#include <string>
#include <algorithm>

namespace gitterKid {
    namespace fsi {
        commitPersonLog content::parsePersonLog(
            std::vector<byte>::iterator begin,
            std::vector<byte>::iterator end) {

            std::string fields[4];
            std::vector<byte>::iterator ch = begin;
            for (int i = 0; ch != end && i < 4; i++) {
                std::vector<byte>::iterator itemEnd = std::find(ch, end, (byte) ' ');
                fields[i].insert(fields[i].begin(), ch, itemEnd);
                if (itemEnd != end) {
                    ch = itemEnd + 1;
                }
                else {
                    ch = end;
                }
            }

            return commitPersonLog(fields[0], fields[1], std::stol(fields[2]), fields[3]);
        }
    }
}