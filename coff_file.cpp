#include <coff_file.h>
#include <time.h>
#include <iomanip>

void push_back_number(std::vector<char>& ref, int32_t number, char size){
    int temp = number;
    for(int i = 0; i < size; i++){
        unsigned char byte = temp&0xff;
        ref.push_back(byte);
        temp>>=8; // remove last byte
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
}
RelocationTable::RelocationTable(bool isNull){
    this->isNull = isNull;
}

void RelocationTable::add_relocation(long r_vaddr, long r_symndx, unsigned short r_type){
    relocations.push_back({r_vaddr, r_symndx, r_type});
}

int RelocationTable::get_size(){
    return relocations.size() * 10;
}

std::vector<char> RelocationTable::get_data(){
    if(isNull){
        return std::vector<char>();
    } else {
        std::vector<char> data;
        for(int r = 0; r < relocations.size(); r++){
           push_back_number(data, relocations[r].r_vaddr, 4); 
           push_back_number(data, relocations[r].r_symndx, 4);
           push_back_number(data, relocations[r].r_type, 2);
        }
        return data;
    }
}


void CoffFile::add_section(char name[8], int32_t flags, RelocationTable rt, std::vector<unsigned char> data){
    head.f_symptr+=40;
    section_header_t sh = {name, 0, 0, data.size(), head.f_symptr, head.f_symptr+data.size(), 0, 0, 0, flags, data};
    head.f_symptr+=data.size();
    head.f_symptr+=rt.get_size();
    // After adding a new section, every section is offset by 40
    for(int i = 0; i < sections.size(); i++){
        sections[i].s_scnptr+=40;
    }
    
    sections.push_back(sh);
    head.f_nscns += 1;
    rts.push_back(rt);
}

void CoffFile::coff_pbn(int32_t number, char size){
    push_back_number(this->data, number, size);
}

void CoffFile::add_symbol(std::string name, unsigned long value, short scnum, unsigned short type, unsigned char sclass, unsigned char numaux, std::string aux){
    char full_aux[18] = {0};
    if(aux != ""){
        strcpy(full_aux, aux.c_str());
    }
    if(name.length() <= 8){
        char final_name[8];
        for(int i = 0; i < 8; i++){
            if(i < name.length()){
                final_name[i] = name[i];
            } else{
                final_name[i] = 0;
            }
        }
        symbol s = {final_name, value, scnum, type, sclass, numaux, full_aux};
        symbols.push_back(s);
    } // We need the string table otherwise
    else{
        int offset = string_table.size()-1;
        for(int i = 0; name[i] != 0; i++){
            string_table.push_back(name[i]);
        }
        string_table.push_back(0);
        struct str_offset str_pointer = {0, offset};
        char* final_name = (char*)(&str_pointer);
        symbol s = {final_name, value, scnum, type, sclass, numaux, full_aux};
        symbols.push_back(s);
    }
    head.f_nsyms += 1;
}

void CoffFile::compile(){
    data.clear();
    // compile header
    coff_pbn(head.f_magic, 2);
    coff_pbn(head.f_nscns, 2);
	coff_pbn(head.f_timdat, 4);
	coff_pbn(head.f_symptr, 4);	
	coff_pbn(head.f_nsyms, 4);
	coff_pbn(head.f_opthdr, 2);	
	coff_pbn(head.f_flags, 2);
    std::cout << "header compiled" << std::endl;

    // compile section headers
    for(int i = 0; i < sections.size(); i++){
        coff_pbn(sections[i].s_name.name_num, 8);
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
    std::cout << "section headers compiled" << std::endl;
    std::vector<char> rdata;
    for(int i = 0; i < sections.size(); i++){
        data.insert(data.end(), sections[i].data.begin(), sections[i].data.end());
        rdata = rts[i].get_data();
        data.insert(data.end(), rdata.begin(), rdata.end()); // TODO: possible error
    }
    std::cout << "sections compiled" << std::endl;
    for(int i = 0; i < symbols.size(); i++){
        coff_pbn(symbols[i].e.e.e_zeroes, 4);
        coff_pbn(symbols[i].e.e.e_offset, 4);
        coff_pbn(symbols[i].e_value, 4);
        coff_pbn(symbols[i].e_scnum, 2);
        coff_pbn(symbols[i].e_type, 2);
        coff_pbn(symbols[i].e_sclass, 1);
        coff_pbn(symbols[i].e_numaux, 1);
    }
    std::cout << "symbols compiled" << std::endl;
    
    for(int i = 0; i < string_table.size(); i++){
        if(string_table[i] == 0){
            std::cout <<"0";
        }
        std::cout << string_table[i];
    }
    
    data.insert(data.end(), string_table.begin(), string_table.end());
    std::cout << "string table added" << std::endl;


    std::cout << "Compiled. size of data: " << data.size();
    for(int i = 0; i < data.size(); i++){
        std::cout << std::hex << (int)(unsigned char)data[i]<< " ";
    }
}