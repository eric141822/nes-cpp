#ifndef ROM_H
#define ROM_H

#include <cstdint>
#include <vector>
#include <iostream>

const uint8_t NES_TAG[4] = {0x4E, 0x45, 0x53, 0x1A};
const size_t PRG_ROM_PAGE_SIZE = 16384; // 16KB
const size_t CHR_ROM_PAGE_SIZE = 8192; // 8KB

enum Mirroring {
    VERTICAL,
    HORIZONTAL,
    FOUR_SCREEN,
};

struct Rom {
    std::vector<uint8_t> prg_rom;
    std::vector<uint8_t> chr_rom;
    uint8_t mapper;
    Mirroring screen_mirroring;

    Rom() {};
    explicit Rom(std::vector<uint8_t> raw);
};

#endif // !ROM_H