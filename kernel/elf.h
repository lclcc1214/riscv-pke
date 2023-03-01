#ifndef _ELF_H_
#define _ELF_H_

#include "util/types.h"
#include "process.h"

#define MAX_CMDLINE_ARGS 64

// elf header structure
typedef struct elf_header_t {
  uint32 magic;
  uint8 elf[12];
  uint16 type;      /* Object file type */
  uint16 machine;   /* Architecture */
  uint32 version;   /* Object file version */
  uint64 entry;     /* Entry point virtual address */
  uint64 phoff;     /* Program header table file offset */
  uint64 shoff;     /* Section header table file offset */
  uint32 flags;     /* Processor-specific flags */
  uint16 ehsize;    /* ELF header size in bytes */
  uint16 phentsize; /* Program header table entry size */
  uint16 phnum;     /* Program header table entry count */
  uint16 shentsize; /* Section header table entry size */
  uint16 shnum;     /* Section header table entry count */
  uint16 shstrndx;  /* Section header string table index */
} elf_header;

//add @lab1_challenge1
// section header
typedef struct {
  uint32 name;      /* Section name (string tbl index) */
  uint32 type;      /* Section type */
  uint64 flags;     /* Section flags */
  uint64 addr;      /* Section virtual addr at execution */
  uint64 offset;    /* Section file offset */
  uint64 size;      /* Section size in bytes */
  uint32 link;      /* Link to another section */
  uint32 info;      /* Additional section information */
  uint64 addralign; /* Section alignment */
  uint64 entsize;   /* Entry size if section holds table */
} elf_section_header;

// Program segment header.
typedef struct elf_prog_header_t {
  uint32 type;   /* Segment type */
  uint32 flags;  /* Segment flags */
  uint64 off;    /* Segment file offset */
  uint64 vaddr;  /* Segment virtual address */
  uint64 paddr;  /* Segment physical address */
  uint64 filesz; /* Segment size in file */
  uint64 memsz;  /* Segment size in memory */
  uint64 align;  /* Segment alignment */
} elf_prog_header;

#define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian
#define ELF_PROG_LOAD 1

//add @lab1_challenge1
#define SHT_SYMTAB 2  //symbol table
#define SHT_STRTAB 3  //string table
#define STT_FILE 4    //symbol's name is file name
#define STT_FUNC 18    //symbol's name is function name

typedef enum elf_status_t {
  EL_OK = 0,

  EL_EIO,
  EL_ENOMEM,
  EL_NOTELF,
  EL_ERR,

} elf_status;

// added @lab1_challenge1
// the symbol table record, each record stores information of a symbol
typedef struct {
// st_name is the offset in string table.
  uint32 st_name;         /* Symbol name (string tbl index) */
  unsigned char st_info;  /* Symbol type and binding */
  unsigned char st_other; /* Symbol visibility */
  uint16 st_shndx;        /* Section index */
  uint64 st_value;        /* Symbol value */
  uint64 st_size;         /* Symbol size */
} elf_symtab_record;

typedef struct elf_ctx_t {
  void *info;
  elf_header ehdr;
  
  //add @lab1_challenge1
  //string table in elf
  char strtab[4096];
  //symbol table in elf
  elf_symtab_record symtab[128];
  //the number of symbol table
  uint64 syms_num;
} elf_ctx;

elf_status elf_init(elf_ctx *ctx, void *info);
elf_status elf_load(elf_ctx *ctx);
//add @lab1_challenge1
// load the symble table and string table in elf
elf_status elf_load_symtab_and_strtab(elf_ctx *ctx);

void load_bincode_from_host_elf(process *p);

#endif
