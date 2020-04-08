Stripper: A better stripper

## Why?
[strip](https://en.wikipedia.org/wiki/Strip_(Unix)) doesn't remove the section headers table from an ELF binary, it just takes care of the `.symtab` of a binary:

    % cat test.c
    #include "stdio.h"

    int
    main()
    {
      printf("hello world\n");
      return 0;
    }

    % gcc test.c -o test
    % strip --strip-all test

    % readelf --sections test
    There are 29 section headers, starting at offset 0x3958:

    Section Headers:
      [Nr] Name              Type             Address           Offset
           Size              EntSize          Flags  Link  Info  Align
      [ 0]                   NULL             0000000000000000  00000000
           0000000000000000  0000000000000000           0     0     0
      [ 1] .interp           PROGBITS         00000000000002a8  000002a8
           000000000000001c  0000000000000000   A       0     0     1
      ...
      ...
      [27] .strtab           STRTAB           0000000000000000  00003648
           0000000000000209  0000000000000000           0     0     1
      [28] .shstrtab         STRTAB           0000000000000000  00003851
           0000000000000103  0000000000000000           0     0     1

Depending on your usecase, you might want to actually remove all debug elements of a binary, especially when compiling with `--ffunction-sections`. What **Stripper** does is completely zero-out the section headers:

    % cat test.c
    #include "stdio.h"

    int
    main()
    {
      printf("hello world\n");
      return 0;
    }

    % gcc test.c -o test
    % /home/vagrant/dev/own/stripper/build/stripper test
    [+] [stripper] File is 64-bit
    [+] [stripper] Nullified section references successfully
    [+] [stripper] Section offset: 14680
    [+] [stripper] Section count: 29
    [+] [stripper] String Index: 28
    [+] [stripper] Removing headers with index 28...
    [+] [stripper] Removing section header completely...
    % readelf --sections test

    There are no sections in this file.

## Building
Build system is [Rake](https://github.com/ruby/rake)

    rake build

## Testing
This will run the test suite:

    rake test

## Usage
    ./stripper <file_path>
