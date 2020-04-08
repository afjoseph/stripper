#include <elf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#define LOG_TAG "stripper"
#define FREE(x)                                                                \
  {                                                                            \
    if (x != NULL) {                                                           \
      free(x);                                                                 \
      x = 0;                                                                   \
    }                                                                          \
  }

#define LOGE(fmt, ...)                                                         \
  {                                                                            \
    fprintf(stderr, "[!] ");                                                   \
    fprintf(stderr, "[%s] ", LOG_TAG);                                         \
    fprintf(stderr, fmt, ##__VA_ARGS__);                                       \
    fprintf(stderr, "\n");                                                     \
  }

#define LOGD(fmt, ...)                                                         \
  {                                                                            \
    printf("[+] ");                                                            \
    printf("[%s] ", LOG_TAG);                                                  \
    printf(fmt, ##__VA_ARGS__);                                                \
    printf("\n");                                                              \
  }

#pragma clang diagnostic pop

#define ELF_SHDR Elf64_Shdr
#define ELF_EHDR Elf64_Ehdr
#define ELF_OFF Elf64_Off
#define ELF_WORD Elf64_Word
