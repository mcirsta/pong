#include "utils.hpp"

#include <cstdint>

bool checkXZ(FILE *fp)
{
    uint8_t magicCh;
    const uint8_t XZ_HEADER_MAGIC[6] = { 0xFD, '7', 'z', 'X', 'Z', 0x00 };
    int sig_len = 6;
    for(int i=0;i<sig_len;i++) {
        fscanf(fp, "%c", &magicCh);
        if(magicCh != XZ_HEADER_MAGIC[i]) {
            return false;
        }
    }
    return true;
}
