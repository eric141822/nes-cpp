#include "rom.h"

Rom::Rom(std::vector<uint8_t> raw)
{
    for (size_t i = 0; i < 4; i++)
    {
        if (raw[i] != NES_TAG[i])
        {
            std::cerr << "Invalid ROM file, not in NES format.\n";
            exit(1);
        }
    }

    uint8_t mapper = (raw[7] & 0b1111'0000) | (raw[6] >> 4);

    uint8_t ines_version = (raw[7] >> 2) & 0b0000'0011;
    if (ines_version != 0)
    {
        std::cerr << "NES 2.0 format not supported.\n";
        exit(1);
    }

    bool four_screen = (raw[6] & 0b1000) != 0;
    bool vertical_mirrowig = (raw[6] & 0b1) != 0;
    Mirroring screen_mirroring = four_screen ? Mirroring::FOUR_SCREEN : vertical_mirrowig ? Mirroring::VERTICAL
                                                                                          : Mirroring::HORIZONTAL;
    size_t prg_rom_size = static_cast<size_t>(raw[4]) * PRG_ROM_PAGE_SIZE;
    size_t chr_rom_size = static_cast<size_t>(raw[5]) * CHR_ROM_PAGE_SIZE;

    bool skip_trainer = (raw[6] & 0b100) != 0;

    uint16_t prg_rom_start = 16 + (skip_trainer ? 512 : 0);
    uint16_t chr_rom_start = prg_rom_start + prg_rom_size;

    this->prg_rom = std::vector<uint8_t>(raw.begin() + prg_rom_start, raw.begin() + prg_rom_start + prg_rom_size);
    this->chr_rom = std::vector<uint8_t>(raw.begin() + chr_rom_start, raw.begin() + chr_rom_start + chr_rom_size);
    this->screen_mirroring = screen_mirroring;
    this->mapper = mapper;
}