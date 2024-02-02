//
// Created by fufeng on 2024/2/2.
//
#include <stdio.h>
#include <stdlib.h>
#include <libelf.h>
#include <gelf.h>

/*
sudo apt-get install libelf-dev
gcc -o readelfsections readelfsections.c -lelf
./readelfsections your_elf_file
 */

void print_section_info(Elf *elf) {
    Elf_Scn *section = NULL;
    Elf32_Shdr *shdr32;
    Elf64_Shdr *shdr64;
    GElf_Shdr shdr;

    while ((section = elf_nextscn(elf, section)) != NULL) {
        gelf_getshdr(section, &shdr);

        printf("Section Name: %s\n", elf_strptr(elf, shdr.sh_link, shdr.sh_name));
        printf("Section Size: %lu bytes\n", (unsigned long)shdr.sh_size);
        printf("Section Offset: 0x%lx\n", (unsigned long)shdr.sh_offset);
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf_filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *elf_filename = argv[1];

    // Initialize ELF library
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ELF library initialization failed: %s\n", elf_errmsg(-1));
        exit(EXIT_FAILURE);
    }

    // Open ELF file
    Elf *elf = elf_begin(fileno(fopen(elf_filename, "r")), ELF_C_READ, NULL);
    if (elf == NULL) {
        fprintf(stderr, "Failed to open ELF file: %s\n", elf_errmsg(-1));
        exit(EXIT_FAILURE);
    }

    // Check ELF type
    if (elf_kind(elf) != ELF_K_ELF) {
        fprintf(stderr, "Not an ELF file\n");
        elf_end(elf);
        exit(EXIT_FAILURE);
    }

    // Print section information
    print_section_info(elf);

    // Cleanup
    elf_end(elf);

    return 0;
}
