#include "code.h"
#include "io.c"
#include "io.h"
#include "word.c"
#include "word.h"

#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAGIC 0x8badbeef

// checks bit length of value
uint16_t bit_length(int value) {
  int random = value;
  if (random == 0) {
    return 1;
  }
  // edge case for bit length
  else {
    random = log2(random) + 1;
    return random;
  }
}

int main(int argc, char **argv) {

  int infile = STDIN_FILENO;
  int outfile = STDOUT_FILENO;
  int c;
  bool flag = false;

  while ((c = getopt(argc, argv, "vi:o:")) != -1) {
    switch (c) {
    case ('v'):
      flag = true;
      break;
    case ('i'):
      infile = open(optarg, O_RDONLY | O_CREAT);
      break;
    case ('o'):
      outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC);
      break;
    }
  }

  FileHeader my_head = {0, 0};
  read_header(infile, &my_head);
  fchmod(outfile, my_head.protection); // read header proteciton into output
  if (my_head.magic !=
      MAGIC) { // check if file has header magic. if not breaks and exits/
    printf("Error! Magic number doesn't match");
    close(infile);
    close(outfile);
    exit(2);
  }
  WordTable *table = wt_create();
  uint8_t curr_sym = 0;
  uint16_t curr_code = 0;
  uint16_t next_code = START_CODE;

  while (read_pair(infile, &curr_code, &curr_sym, bit_length(next_code)) ==
         true) {
    table[next_code] = word_append_sym(table[curr_code], curr_sym);
    buffer_word(outfile, table[next_code]);
    next_code++;
    if (next_code == MAX_CODE) {
      wt_reset(table);
      next_code = START_CODE;
    }
  }
  flush_words(outfile);
  wt_delete(table);

  if (flag == true) {
    if ((total_bits % 8) != 0) {
      total_bits = (total_bits / 8) + 1;
    } else {
      total_bits = total_bits / 8;
    }

    if ((total_syms % 8) != 0) {
      total_syms = (total_syms / 8) + 1;
    } else {
      total_syms = total_syms / 8;
    }

    float compression_ratio = 100 * (1 - (total_syms / total_bits));

    printf("Compressed file size: %llu bytes\n", total_syms);
    printf("Uncompressed file size: %llu bytes\n", total_bits);
    printf("Compression ratio: %f%%\n", compression_ratio);
  }

  close(infile);
  close(outfile);

  return 0;
}
