#include "coff_file.h"
#include <time.h>
#include <iomanip>
#include <stdlib.h>
#define BSS_FLAG 0x80


void push_aux_number(std::string& aux, int num, int size){
    for(int i = 0; i < size; i++){
        aux += (num>>(i*8))&0xff;
    }
}

CoffFile::CoffFile(){
    head.f_magic = 0x014c;
    head.f_nscns = 0;
    head.f_timdat = time(NULL);
    head.f_symptr = 20;
    head.f_nsyms = 0;
    head.f_opthdr = 0;
    head.f_flags = 0;
    for(int i = 0; i < 4; i++){
        string_table.push_back(0); // init string table size
    }
    string_table_sz = 4;
    add_symbol(".file\0", 0, -2, 0, 0x67, 1, ".\test.asm");
}
CoffFile::CoffFile(std::string file_name){
    head.f_magic = 0x014c;
    head.f_nscns = 0;
    head.f_timdat = time(NULL);
    head.f_symptr = 20;
    head.f_nsyms = 0;
    head.f_opthdr = 0;
    head.f_flags = 0;
    for(int i = 0; i < 4; i++){
        string_table.push_back(0); // init string table size
    }
    string_table_sz = 4;
    add_symbol(".file\0", 0, -2, 0, 0x67, 1, file_name);
}


void CoffFile::add_section(std::string name_str, int32_t flags, RelocationTable& rt, std::vector<unsigned char> data){
    head.f_symptr+=40;
    section_header_t sh;
    // change name into correct type
    char* name = (char*)malloc(8);
    for(int i = 0; i < 8; i++){
        name[i] = 0;
    }
    int name_length = (8>name_str.length())?name_str.length():8;
    for(int i = 0; i < name_length; i++){
        name[i] = name_str[i];
    }
    // Check for bss section
    if((flags & 0xFF) != BSS_FLAG) {
        // long ass initialization
        sh = {name, 0, 0, (long)data.size(), head.f_symptr, (long)(head.f_symptr+data.size()), 0, (unsigned short)(rt.relocations.size() + rt.future_relocations.size()), 0, flags, data};
        head.f_symptr+=data.size();
        head.f_symptr+=rt.get_size();
    } else {
        // bss starts with nothing
        sh = {name, 0, 0, 0, 0, 0, 0, 0, 0, flags, data};
        bss_sections.push_back(sections.size()+1);
    }
    
    // After adding a new section, every section is offset by section size
    for(int i = 0; i < sections.size(); i++){
        // except bss
        if((sections[i].s_flags & 0xFF) != BSS_FLAG){
            sections[i].s_scnptr+=40;
            sections[i].s_relptr+=40;
        }
    }
    
    sections.push_back(sh);
    head.f_nscns += 1;
    rts.push_back(rt);

    // create section symbol
    // only real time we need an aux
    // TODO: Find way to make it optional
    std::string aux;
    push_aux_number(aux, sh.s_size, 4);
    push_aux_number(aux, sh.s_nreloc, 2);
    add_section_symbol(std::string(name) + "\0", 0, sections.size(), 0, 3, 1, aux);
}


symbol CoffFile::create_symbol(std::string name, unsigned long value, short scnum, 
                        unsigned short type, unsigned char sclass, 
                        unsigned char numaux, std::string aux){
    symbol created_symbol = {NULL,0,0,0,0,0,NULL};
    char* full_aux;
    // needs to be bigger if we are using aux
    if(aux != ""){
        // change type
        full_aux =  (char*)malloc(18);
        for(int i = 0; i < 18; i++){
            full_aux[i] = 0;
        }
        strcpy(full_aux, aux.c_str());
    } 
    if(name.length() <= 8){ // add name normally
        char* final_name = (char*)malloc(8);
        for(int i = 0; i < 8; i++){
            if(i < name.length()){
                final_name[i] = name[i];
            } else{
                final_name[i] = 0;
            }
        }
        created_symbol = {final_name, value, scnum, type, sclass, numaux, full_aux};
    } // We need the string table otherwise
    else{
        int table_offset = string_table.size();
        for(int i = 0; name[i] != 0; i++){
            string_table.push_back(name[i]);
        }
        string_table.push_back(0);
        string_table_sz += name.size() + 1;
        struct str_offset str_pointer = {0, (unsigned long)table_offset};
        sym_name sn;
        sn.e = str_pointer;
        created_symbol = {sn, value, scnum, type, sclass, numaux, full_aux};
    }
    // if we are in bss, add to the size
    for(int i = 0; i < bss_sections.size(); i++){
        if(scnum == bss_sections[i]){
            sections[bss_sections[i]-1].s_size += 4;
        }
    }
    // add to symbol number, double if aux is present
    head.f_nsyms += 1 + (aux == ""?0:1);                
    return created_symbol;
}

void CoffFile::add_section_symbol(std::string name, unsigned long value, short scnum, 
                        unsigned short type, unsigned char sclass, 
                        unsigned char numaux, std::string aux){
    symbol s = create_symbol(name, value, scnum, type, sclass, numaux, aux);
    symbols.insert(symbols.begin() + scnum, s); // add a section at the section number, assume no duplicates
                                                // TODO: assume duplicates
}

void CoffFile::coff_pbn(int32_t number, char size){
    push_back_number(this->data, number, size);
}

void CoffFile::add_symbol(std::string name, unsigned long value, short scnum, unsigned short type, unsigned char sclass, unsigned char numaux, std::string aux){
    symbol s = create_symbol(name, value, scnum, type, sclass, numaux, aux);
    symbols.push_back(s);
}

void CoffFile::fill_reloc_table(RelocationTable& t){
    for(int i = 0; i < t.future_relocations.size(); i++){
        t.add_relocation(
            get_relocation(t.future_relocations[i].r_name, 
                t.future_relocations[i].r_vaddr, 
                t.future_relocations[i].r_type
            )
        );
    }
}

void CoffFile::compile(bool verbose){
    data.clear();
    // compile header
    coff_pbn(head.f_magic, 2);
    coff_pbn(head.f_nscns, 2);
	coff_pbn(head.f_timdat, 4);
	coff_pbn(head.f_symptr, 4);	
	coff_pbn(head.f_nsyms, 4);
	coff_pbn(head.f_opthdr, 2);	
	coff_pbn(head.f_flags, 2);

    // compile section headers
    for(int i = 0; i < sections.size(); i++){
        for(int j = 0; j < 8; j++){
            coff_pbn(sections[i].s_name.name[j], 1);
        }
        coff_pbn(sections[i].s_paddr, 4);
        coff_pbn(sections[i].s_vaddr, 4);
        coff_pbn(sections[i].s_size, 4);
        coff_pbn(sections[i].s_scnptr, 4);
        coff_pbn(sections[i].s_relptr, 4);
        coff_pbn(sections[i].s_lnnoptr, 4);
        coff_pbn(sections[i].s_nreloc, 2);
        coff_pbn(sections[i].s_nlnno, 2);
        coff_pbn(sections[i].s_flags, 4);
    }
    if (verbose) { std::cout << "Section headers compiled" << std::endl; }
    std::vector<char> rdata;
    for(int i = 0; i < sections.size(); i++){
        data.insert(data.end(), sections[i].data.begin(), sections[i].data.end());
        fill_reloc_table(rts[i]);
        rdata = rts[i].get_data();
        data.insert(data.end(), rdata.begin(), rdata.end()); // TODO: possible error
    }
    if (verbose) { std::cout << "Sections compiled" << std::endl; }
    for(int i = 0; i < symbols.size(); i++){
        if(symbols[i].e.e.e_zeroes != 0){
            for(int j = 0; j < 8; j++){
                coff_pbn(symbols[i].e.e_name[j], 1);
            }
        } else {
            coff_pbn(symbols[i].e.e.e_zeroes, 4);
            coff_pbn(symbols[i].e.e.e_offset, 4);
        }
        coff_pbn(symbols[i].e_value, 4);
        coff_pbn(symbols[i].e_scnum, 2);
        coff_pbn(symbols[i].e_type, 2);
        coff_pbn(symbols[i].e_sclass, 1);
        coff_pbn(symbols[i].e_numaux, 1);
        if(symbols[i].e_numaux == 1){
            for(int j = 0; j < 18; j++){
                coff_pbn(symbols[i].aux[j], 1);
            }
        }
    }
    if (verbose) { std::cout << "Symbols compiled" << std::endl; }
    
    // set string table size.
    for(int i = 0; i < 4; i++){
        string_table[i] = (string_table_sz>>(i*8))&0xFF;
    }

    data.insert(data.end(), string_table.begin(), string_table.end());
    if (verbose) { std::cout << "String table added" << std::endl; }


    if (verbose) { std::cout << "Compiled. size of data: " << data.size() << std::endl; }
}

std::string CoffFile::get_compiled(){
    std::string compiled = "";
    for(int i = 0; i < data.size(); i++){
        compiled += data[i];
    }
    return compiled;
}


relocation CoffFile::get_relocation(std::string symname, int vaddr, int type){
    relocation reloc = {vaddr, -1, (unsigned short)type};
    int symndx = 0;
    for(int i = 0; i < symbols.size(); i++){
        if(symbols[i].e.e.e_zeroes == 0){
            std::string name = "";
            for(int stroff = symbols[i].e.e.e_offset; string_table[stroff] != 0; stroff++ ){
                name += string_table[stroff];
            }
            if(name == symname){
                reloc.r_symndx = symndx;
                return reloc;
            }
        } else {
            if(symbols[i].e.e_name == symname){
                reloc.r_symndx = symndx;
                return reloc;
            }
        }
        symndx += 1;
        if(symbols[i].e_numaux != 0){
            symndx += 1;
        }
    }
    return reloc;
}