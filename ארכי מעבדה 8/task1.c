#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <elf.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <inttypes.h>

#define MaxSize 100

typedef struct {
    char *name;
    void (*fun)();
} fun_desc;

int check_valid_file();

int _inDebug = 0;
Elf32_Phdr * _elfProg;
int Currentfd=-1;
unsigned char * _mappedFile;
char _fileName[MaxSize];


/***********************void Fun()******************************/
void toggleDebug(){
    if(!_inDebug)
        printf("Debug flag now on\n");
    else
        printf("Debug flag now off\n");

    _inDebug=(_inDebug+1)%2;
}
Elf32_Ehdr * getEhdr(){
	return (Elf32_Ehdr *)_mappedFile;
}
Elf32_Shdr * getShdr(){
	return ((Elf32_Shdr *)(_mappedFile+getEhdr()->e_shoff));
}
Elf32_Shdr * getSectionByIndex(int i){
	return &getShdr()[i];
}
char * elf_str_table() {
    return (char *)getEhdr() + getSectionByIndex(getEhdr()->e_shstrndx)->sh_offset;
}
char * elf_lookup(int offset) {
    char *strtab = elf_str_table();
    return strtab + offset;
}
char * getNameOfSection(int k){
    int shnum = getEhdr()->e_shnum;
    Elf32_Shdr *shdr = (Elf32_Shdr *)(_mappedFile + getEhdr()->e_shoff);

    Elf32_Shdr *sh_strtab = &shdr[getEhdr()->e_shstrndx];
    char *const sh_strtab_p = _mappedFile + sh_strtab->sh_offset;

    for (int i = 0; i < shnum; ++i) {
        if(i == k){
            return sh_strtab_p + shdr[i].sh_name;
        }
    }
    return NULL;
}
char * getNameOfSymbol(int offset){
        if (Currentfd==-1){
	printf("Invalide move, Be nice!\n");
	return (char *) NULL;
    }


    Elf32_Ehdr* _Ehdr = (Elf32_Ehdr *) _mappedFile;
    int i,cont = 1;
    Elf32_Shdr* _Shdr = (Elf32_Shdr*)(_Ehdr->e_shoff + _mappedFile);
    Elf32_Sym* symtab = NULL;
    for (i = 0; cont && i < getEhdr()->e_shnum; i++){
        if (_Shdr[i].sh_type == 0xb) {
            symtab = (Elf32_Sym *)(_mappedFile + _Shdr[i].sh_offset);
            Elf32_Shdr *strtab = _Shdr + _Shdr[i].sh_link;
            char *name = (char *)_mappedFile + strtab->sh_offset + symtab[offset].st_name;
            return name;
        }
    }
    return (char *)NULL;
}
unsigned int getValueOfSymbol(int offset){
        if (Currentfd==-1){
	printf("Invalide move, Be nice!\n");
	return 0;
    }


    Elf32_Ehdr* _Ehdr = (Elf32_Ehdr *) _mappedFile;
    int i,cont = 1;
    Elf32_Shdr* _Shdr = (Elf32_Shdr*)(_Ehdr->e_shoff + _mappedFile);
    Elf32_Sym* symtab = NULL;
    for (i = 0; cont && i < getEhdr()->e_shnum; i++){
        if (_Shdr[i].sh_type == SHT_DYNSYM) {
            symtab = (Elf32_Sym *)(_mappedFile + _Shdr[i].sh_offset);
            unsigned int value = symtab[offset].st_value;
            return value;
        }
    }
    return 0;
}
void printRaw(){
        int shnum = getEhdr()->e_shnum;
    	Elf32_Shdr *shdr = (Elf32_Shdr *)(_mappedFile + getEhdr()->e_shoff);
 
	unsigned int i, idx;
	// Iterate over section headers
	for(i = 0; i < shnum; i++) {
		Elf32_Shdr *section = &shdr[i];
 
		// If this is a relocation section
		if(section->sh_type == SHT_REL) {
			// Process each entry in the table
                        printf("Offset\t\t\tInfo\n");
			for(idx = 0; idx < section->sh_size / section->sh_entsize; idx++) {
				Elf32_Rel *reltab = &((Elf32_Rel *)((int)getEhdr() + section->sh_offset))[idx];
                                printf("%08x\t\t", reltab->r_offset);
                                printf("%08x\n", reltab->r_info);
			}
		}
	}
	return;
}

char * getType(int typeidx){
char* table[39] = {
    "R_386_NONE",
    "R_386_32",
    "R_386_PC32",
    "R_386_GOT32",
    "R_386_PLT32",
    "R_386_COPY","R_386_GLOB_DAT",
    "R_386_JMP_SLOT","R_386_RELATIVE",
    "R_386_GOTOFF",
    "R_386_GOTPC",
    "R_386_32PLT",
    "R_386_TLS_TPOFF",
    "R_386_TLS_IE",
    "R_386_TLS_GOTIE",
    "R_386_TLS_LE",
    "R_386_TLS_GD",
    "R_386_TLS_LDM",
    "R_386_16",
    "R_386_PC16",
    "R_386_8","R_386_PC8",
    "R_386_TLS_GD_32",
    "R_386_TLS_GD_PUSH",
    "R_386_TLS_GD_CALL",
    "R_386_TLS_GD_POP",
    "R_386_TLS_LDM_32",
    "R_386_TLS_LDM_PUSH",
    "R_386_TLS_LDM_CALL",
    "R_386_TLS_LDM_POP",
    "R_386_TLS_LDO_32",
    "R_386_TLS_IE_32",
    "R_386_TLS_LE_32",
    "R_386_TLS_DTPMOD32",
    "R_386_TLS_DTPOFF32",
    "R_386_TLS_TPOFF32",
    "R_386_NUM"
};
return table[typeidx];
    
}
void printSemantic (){
        int shnum = getEhdr()->e_shnum;
    	Elf32_Shdr *shdr = (Elf32_Shdr *)(_mappedFile + getEhdr()->e_shoff);
 
	unsigned int i, idx;
	// Iterate over section headers
	for(i = 0; i < shnum; i++) {
		Elf32_Shdr *section = &shdr[i];
                //Relocation section '.rel.dyn' at offset 0x260 contains 1 entries:
 
		// If this is a relocation section
		if(section->sh_type == SHT_REL) {
                        int symtab_count = section->sh_size/section->sh_entsize;
                        printf("Relocation section %s at offset 0x%x contains %d entries\n", getNameOfSection(i), section->sh_offset, symtab_count);
			// Process each entry in the table
                        printf("Offset\t\t\tInfo\t\tType\t\tSym.Value\tSym.Name\n");
			for(idx = 0; idx < section->sh_size / section->sh_entsize; idx++) {
				Elf32_Rel *reltab = &((Elf32_Rel *)((int)getEhdr() + section->sh_offset))[idx];
                                printf("%08x\t\t", reltab->r_offset);
                                printf("%08x\t", reltab->r_info);
                                
                                int typeidx = (reltab->r_info & 0x000000ff);
                                printf("%s\t", getType(typeidx));
                                int nameidx = (reltab->r_info & 0x0000ff00)/0x100;
                                printf("%08x\t", getValueOfSymbol(nameidx));
                                printf ("%s", getNameOfSymbol(nameidx));
                                printf("\n");
			}
		}
	}
	return;
}
void setFileName(){
    char c;
    printf("Please enter a file name to be examine:\n");
    while ((c = getchar()) != '\n' && c != EOF) { }
    fgets(_fileName,MaxSize,stdin);
    _fileName[strlen(_fileName)-1]=0;
}
int readElf(){
	if(Currentfd!=-1)
		close(Currentfd);

	if((Currentfd=open(_fileName,O_RDONLY)) ==0){
        perror("Unable to open file");
        return 0;
    }
    struct stat st;
    stat(_fileName, &st);
    int size = st.st_size;
    
    if ((_mappedFile=mmap(0,size,PROT_READ,MAP_SHARED,Currentfd,0)) == MAP_FAILED) {
        perror("mmap");
        return 0;
    }
    return 1;
}
void examinElf(){
    setFileName();
    if(!readElf())
        return;

    check_valid_file();

    printf("Magic Numbers: ");
    printf("%c",getEhdr()->e_ident[1]);
    printf("%c",getEhdr()->e_ident[2]);
    printf("%c\t",getEhdr()->e_ident[3]);
    if(getEhdr()->e_ident[5]==1){
    	printf("Data encoding scheme:LittleEndian\n");
    }else{
    	printf("Data encoding scheme:BigEndian\n");
        }
    printf("Entry point: 0x%08x\n",getEhdr()->e_entry);
    printf("ELF Section Header Offset: %d\n",getEhdr()->e_shoff);
    printf("ELF Number of Sections: %d\n",getEhdr()->e_shnum);
    printf("\t ID \t Size\n");
    for (int i=0;i<getEhdr()->e_shnum;i++){
    	printf("\t %d \t %08x\t\t",i,(getSectionByIndex(i)->sh_size));
        printf("\n");
    }
    printf("\n");
    printf("ELF program header Offset: %d\n",getEhdr()->e_shoff);
    printf("ELF Number of program header entries: %d\n",getEhdr()->e_phnum);
    printf("\t ID \t Size\n");
    Elf32_Phdr * _elfProg =(Elf32_Phdr *)(_mappedFile + (int)getEhdr()->e_phoff);
    for (int i=0;i<getEhdr()->e_phnum;i++)
    	printf("\t %d \t %08x\n" ,i, _elfProg[i].p_filesz);
    printf("\n");    
}
void printSection(){
    if (Currentfd==-1){
		printf("Invalide move, Be nice!\n");
		return;
	}
	readElf();
	check_valid_file();


	
	printf("Index \tName           \t\tAddress \tOffset  \tsize\n");
	for (int i=0;i<getEhdr()->e_shnum;i++){
    	printf("%03d  \t%-15s \t%08X \t%08X \t%08X\n",i,getNameOfSection(i),getSectionByIndex(i)->sh_addr,getSectionByIndex(i)->sh_offset,getSectionByIndex(i)->sh_size);
    }
   	printf("\n");
                
   return;
}
void linkTo(){
    if (Currentfd==-1){
	printf("Invalide move, Be nice!\n");
	return;
    }


    Elf32_Ehdr* _Ehdr = (Elf32_Ehdr *) _mappedFile;
    int _symCount;
    int i,j,cont = 1;
    Elf32_Shdr* _Shdr = (Elf32_Shdr*)(_Ehdr->e_shoff + _mappedFile);
    Elf32_Sym* symtab = NULL;
    for (i = 0; cont && i < getEhdr()->e_shnum; i++){
        if (_Shdr[i].sh_type == SHT_SYMTAB || _Shdr[i].sh_type == 0xb) {
            _symCount = _Shdr[i].sh_size/_Shdr[i].sh_entsize;
            symtab = (Elf32_Sym *)(_mappedFile + _Shdr[i].sh_offset);
            for (j = 0; j < _symCount; j++){
                Elf32_Shdr *strtab = _Shdr + _Shdr[i].sh_link;
                const char *name = (const char *)_mappedFile + strtab->sh_offset + symtab[j].st_name;
                if (strcmp(name,"_start") == 0){
                    cont = 0;
                    printf("_start check: PASSED\n");
                }
            }
        }
    }
    if (cont)
    	printf("_start check: FAILED\n");
}
void printSymbol(){
    if (Currentfd==-1){
        return;
    }
    Elf32_Ehdr* _Ehdr = (Elf32_Ehdr *) _mappedFile;
    Elf32_Shdr* _Shdr = (Elf32_Shdr*)(_Ehdr->e_shoff + _mappedFile);
    Elf32_Sym* symtab = NULL;
    if(!check_valid_file()){
        return;
    }
    const char *const sh_strtab = _mappedFile+ (_Shdr+_Ehdr->e_shstrndx)->sh_offset;
    int symtab_count;
    int i,j;
    for (i = 0; i < _Ehdr->e_shnum; i++){
        if (_Shdr[i].sh_type == SHT_SYMTAB || _Shdr[i].sh_type == 0xb) {
            symtab_count = _Shdr[i].sh_size/_Shdr[i].sh_entsize;
            symtab = (Elf32_Sym *)(_mappedFile + _Shdr[i].sh_offset);
            printf(" IDX VALUE   SEC_IDX   SEC_NAME              \t\tSYMBOL_NAME\n");
            for (j = 0; j < symtab_count; j++){
                Elf32_Shdr *strtab = _Shdr + _Shdr[i].sh_link;
                const char *name = (const char *)_mappedFile + strtab->sh_offset + symtab[j].st_name;
                printf(" %3d %08x %6d    %-10s %s\n",j,symtab[j].st_value,symtab[j].st_shndx,sh_strtab + _Shdr[i].sh_name,name);
            }
        }
    }
}
void quitProg(){
    printf("quitting\n");
    exit(0);
}
/*********************UI****************************************/
int check_valid_file(){
	printf("1\n");
	if((getEhdr()->e_ident[1]=='E')&&(getEhdr()->e_ident[2]=='L')&&(getEhdr()->e_ident[3]=='F'))
			return 1;
	printf("2\n");
	perror("File is not ELF File\n");
	close(Currentfd);
	printf("3\n");
	Currentfd=-1;

	return 0;	
}

void promptDebugPrint(){
}

void displaymenu(){
        printf("Choose action:\n ");
        printf("0-Toggle Debug Mode\n ");
        printf("1-Examine ELF File\n ");
        printf("2-Print Section Names\n ");
        printf("3-Quit\n");
}

int main(int argc, char *argv[]) {
    int _userChoice;
    fun_desc funArray[] = {
        {"Toggle Debug Mode",toggleDebug},
        {"Examine ELF File",examinElf},
        {"Print Section Names",printSection},
        {"Quit" ,quitProg},
        {NULL ,NULL}
    };
    while(1){
        if (_inDebug)
            promptDebugPrint();

        displaymenu();
        scanf("%d" , &_userChoice);
        if (_userChoice>=0&&_userChoice<=3)
            funArray[_userChoice].fun();

    }
    return 0;
}
