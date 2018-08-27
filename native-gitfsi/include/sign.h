#ifndef _GIT_FSI_SIGN_
#define _GIT_FSI_SIGN_

#include "content.h"
#include <string>
#include <sstream>

namespace gitter_kid {
namespace fsi {

template <char _T_Base_Chr,
         char _T_Base_Starter,
         char _T_Left_Mover,
         typename _T_Chr_Type>
_T_Chr_Type __hex_byte(_T_Chr_Type chr) {
    return (chr - _T_Base_Chr + _T_Base_Starter) << _T_Left_Mover;
}

template <typename _T_Base_Ptr> byte __to_byte(_T_Base_Ptr &base) {
    byte result = 0x00;
    if ('0' <= *base && *base <= '9') {
        result ^= __hex_byte<'0', 0, 4>(*base);
    }
    else {
        result ^= __hex_byte<'a', 10, 4>(*base);
    }

    if ('0' <= *(base + 1) && *(base + 1) <= '9') {
        result ^= __hex_byte<'0', 0, 0>(*(base + 1));
    }
    else {
        result ^= __hex_byte<'a', 10, 0>(*(base + 1));
    }

    return result;
}

class sign_t {
private:
    std::string _sign_str;
    std::basic_string<byte> _sign_bytes;
public:
    sign_t();
    sign_t(std::string sign);
    sign_t(char *sign);
    template<typename _T_Iter> void str_assign(_T_Iter begin, _T_Iter end) {
        this->_sign_str.assign(begin, end);
        auto itr = begin;
        while (itr != end) {
            this->_sign_bytes.push_back(__to_byte(itr));
            itr += 2;
        }
    }
    template<typename _T_Iter> void bytes_assign(_T_Iter begin, _T_Iter end) {
        this->_sign_bytes.assign(begin, end);
        this->_sign_str.resize(40);

        std::stringstream sign_stream;
        sign_stream.rdbuf()->pubsetbuf(const_cast<char *>(this->_sign_str.data()), 40);
        for (auto itr = begin; itr != end; itr++) {
            sign_stream << std::hex << uint16_t((*itr & 0xF0) >> 4) << uint16_t((*itr & 0x0F));
        }
    }
    std::string &str();
    std::basic_string<byte> &bytes();

    bool operator< (const sign_t &other_sign) const;
};

}
}

#endif
