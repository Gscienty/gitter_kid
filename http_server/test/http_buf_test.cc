#include "http_buf.h"
#include <iostream>

int main() {
    http_server::http_buf buf(1);
    uint8_t arr[4] = { 0 };
    buf.pubsetbuf(arr, 3);

    buf.sputc('a');
    buf.sputc('a');
    buf.sputc('a');
    buf.sputc('b');
    buf.sputc('b');
    buf.sputc('b');
    buf.sputc('b');
    buf.sputn(reinterpret_cast<const uint8_t *>("Hello"), 5);
    buf.pubsync();

    return 0;
}
