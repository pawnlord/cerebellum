#ifndef COFF_FILE_H
#define COFF_FILE_H
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include "coff_utils.h"


class CoffFile {
    public:
    CoffFile();
    CoffFile(std::string file_name);
    void add_section(std::string name_str, int32_t flags, RelocationTable& rt, std::vector<unsigned char> data);
    void add_symbol(std::string name, unsigned long value, short scnum, 
                        unsigned short type, unsigned char sclass, 
                        unsigned char e_numaux, std::string aux);
    void add_section_symbol(std::string name, unsigned long value, short scnum, 
                        unsigned short type, unsigned char sclass, 
                        unsigned char e_numaux, std::string aux);
    // Fill a relocation table with its future relocations
    // In order to not have relocations that are made invalid by future symbols
    void fill_reloc_table(RelocationTable& t);
    void compile();
    std::string get_compiled();
    relocation get_relocation(std::string symname, int vaddr, int type);
    private:
    symbol create_symbol(std::string name, unsigned long value, short scnum, 
                        unsigned short type, unsigned char sclass, 
                        unsigned char e_numaux, std::string aux);
    void coff_pbn(int32_t number, char size);// size: 1,2, or 4
    header_t head;
    std::vector<section_header_t> sections;
    std::vector<symbol> symbols;
    std::vector<RelocationTable> rts;
    std::vector<char> data;
    std::vector<char> string_table;
    std::vector<int> bss_sections;
    int string_table_sz;
};

#endif