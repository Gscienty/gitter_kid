#ifndef _GIT_FSI_SIGN_
#define _GIT_FSI_SIGN_

#include "content.h"
#include <string>
#include <sstream>

namespace gitter_kid {
namespace fsi {

template<int _Int_Minus, int _Int_Off, typename _T_Char> _T_Char __hex_val(_T_Char &chr) {
    return (chr - _Int_Minus) << _Int_Off;
}

byte __to_byte(std::basic_string<byte>::iterator &base);

class sign_t {
private:
    std::string _sign_str;
    std::basic_string<byte> _sign_bytes;
public:
    sign_t();
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
};

}
}

#endif
