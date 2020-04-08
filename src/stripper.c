#include "stripper.h"

bool
read_file(char* filename,
          size_t filename_len,
          FILE** out_file,
          size_t* out_file_len,
          char** out_buffer)
{
  FILE* file = 0;
  char* file_buffer = 0;
  size_t file_len = 0;

  filename[filename_len] = 0;

  // TODO: Use mmap, msync and munmap to process large files
  if (!(file = fopen(filename, "rb"))) {
    LOGE("Couldn't open file");
    return false;
  }

  fseek(file, 0, SEEK_END);
  file_len = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (file_len < sizeof(Elf32_Ehdr)) {
    LOGE("File size is too small");
    fclose(file);
    file = 0;
    return false;
  }

  file_buffer = calloc(file_len + 1, sizeof(char));
  if (!file_buffer) {
    LOGE("Couldn't allocate file_buffer");
    fclose(file);
    file = 0;
    return false;
  }

  fread(file_buffer, 1, file_len, file);
  file_buffer[file_len] = 0;
  fclose(file);
  file = 0;

  *out_buffer = file_buffer;
  *out_file = file;
  *out_file_len = file_len;

  return true;
}

bool
write_to_file(char* filename,
              size_t filename_len,
              char* new_file_buffer,
              size_t new_file_buffer_len)
{
  FILE* file = 0;

  filename[filename_len] = 0;

  if (!(file = fopen(filename, "w"))) {
    LOGE("Couldn't open file");
    return false;
  }

  fwrite(new_file_buffer, 1, new_file_buffer_len, file);
  fclose(file);
  file = 0;

  return true;
}

bool
find_sections(const char* file_buffer,
              int* out_section_count,
              int* out_str_index,
              ELF_SHDR** out_sections)
{
  int section_count = 0;
  int str_index = 0;
  ELF_OFF section_offset = 0;
  ELF_EHDR* ehdr = 0;
  ELF_SHDR* sections = 0;

  if (file_buffer[0] != ELFMAG0 || file_buffer[1] != ELFMAG1 ||
      file_buffer[2] != ELFMAG2 || file_buffer[3] != ELFMAG3) {
    LOGE("File's magic is not ELF");
    return false;
  }

  ehdr = (ELF_EHDR*)&file_buffer[0];

  section_offset = ehdr->e_shoff;
  ehdr->e_shoff = 0;

  section_count = ehdr->e_shnum;
  ehdr->e_shnum = 0;

  str_index = ehdr->e_shstrndx;
  ehdr->e_shstrndx = 0;

  sections = (ELF_SHDR*)(&file_buffer[section_offset]);
  if (sections == NULL) {
    LOGE("Could not assign sections pointer");
    return false;
  }

  LOGD("Nullified section references successfully");
  LOGD("Section offset: %lu", section_offset);
  LOGD("Section count: %u", section_count);
  LOGD("String Index: %u", str_index);

  *out_section_count = section_count;
  *out_str_index = str_index;
  *out_sections = sections;

  return true;
}

bool
remove_headers(char* file_buffer,
               ELF_SHDR* sections,
               int section_count,
               int str_index)
{
  ELF_SHDR* iter = sections;
  for (int i = 0; i < section_count; i++, iter++) {
    if (iter->sh_link == (ELF_WORD)str_index) {
      LOGE("A section is still linked to the str index: %u", iter->sh_link);
      return false;
    }

    if (i == str_index) {
      LOGD("Removing headers with index %d...", i);
      memset(&file_buffer[iter->sh_offset], 0, iter->sh_size);
    }
  }

  LOGD("Removing section header completely...");
  memset(sections, 0, section_count * sizeof(ELF_SHDR));

  return true;
}

int
main(int p_argc, char** p_argv)
{
  FILE* file = 0;
  char* file_buffer = 0;
  size_t file_len = 0;

  if (p_argc != 2) {
    printf("Usage: ./%s <file_path>\n", LOG_TAG);
    return 1;
  }

  if (!read_file(
        p_argv[1], strlen(p_argv[1]), &file, &file_len, &file_buffer)) {
    LOGE("Failed to read file");
    FREE(file_buffer);
    return 1;
  }

  if (file_buffer[EI_DATA] != ELFDATA2LSB) {
    LOGE("File is not little endian");
    FREE(file_buffer);
    return 1;
  }

  if (file_buffer[EI_CLASS] == ELFCLASS32) {
    LOGD("File is 32-bit");
#undef ELF_SHDR
#define ELF_SHDR Elf32_Shdr
#undef ELF_EHDR
#define ELF_EHDR Elf32_Ehdr
#undef ELF_OFF
#define ELF_OFF Elf32_Off
#undef ELF_WORD
#define ELF_WORD Elf32_Word
  } else if (file_buffer[EI_CLASS] == ELFCLASS64) {
    LOGD("File is 64-bit");
#undef ELF_SHDR
#define ELF_SHDR Elf64_Shdr
#undef ELF_EHDR
#define ELF_EHDR Elf64_Ehdr
#undef ELF_OFF
#define ELF_OFF Elf64_Off
#undef ELF_WORD
#define ELF_WORD Elf64_Word
  } else {
    LOGE("File does not have a valid 32/64 bit EI_CLASS");
    FREE(file_buffer);
    return 1;
  }

  int section_count = 0;
  int str_index = 0;
  ELF_SHDR* sections = 0;
  if (!find_sections(file_buffer, &section_count, &str_index, &sections)) {
    return 1;
  }

  if (!remove_headers(file_buffer, sections, section_count, str_index)) {
    return 1;
  }

  if (!write_to_file(p_argv[1], strlen(p_argv[1]), file_buffer, file_len)) {
    return 1;
  }

  FREE(file_buffer);
  return 0;
}
