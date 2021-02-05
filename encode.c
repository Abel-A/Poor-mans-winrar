#include "code.h"
#include "io.c"
#include "io.h"
#include "trie.c"
#include "trie.h"
#include "word.h"

#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

uint32_t bit_length(int value) {
  int random = value;
  if (random == 0) {
    return 1;
  } else {
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

  struct stat foo;
  FileHeader header = {0, 0};
  header.magic = 0x8badbeef;
  fstat(infile, &foo);
  header.protection = foo.st_mode;
  fchmod(outfile, foo.st_mode);
  write_header(outfile, &header);

  TrieNode *root = trie_create();
  TrieNode *curr_node = root;
  TrieNode *prev_node = NULL;
  uint8_t curr_sym = 0;

  uint8_t prev_sym = 0;
  TrieNode *next_node = NULL;

  uint16_t next_code = START_CODE;
  while (read_sym(infile, &curr_sym) == true) {

    next_node = trie_step(curr_node, curr_sym);
    if (next_node != NULL) {
      prev_node = curr_node;
      curr_node = next_node;
    } else {
      buffer_pair(outfile, curr_node->code, curr_sym, bit_length(next_code));
      curr_node->children[curr_sym] = trie_node_create(next_code);
      curr_node = root;
      next_code = next_code + 1;
    }
    if (next_code == MAX_CODE) {
      trie_reset(root);
      curr_node = root;
      next_code = START_CODE;
    }
    prev_sym = curr_sym;
  }
  if (curr_node != root) {
    buffer_pair(outfile, prev_node->code, prev_sym, bit_length(next_code));
    next_code = (next_code + 1) % MAX_CODE;
  }

  buffer_pair(outfile, STOP_CODE, 0, bit_length(next_code));
  flush_pairs(outfile);
  trie_delete(root);

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

    printf("Compressed file size: %llu bytes", total_syms);
    printf("Uncompressed file size: %llu bytes", total_bits);
    printf("Compression ratio: %f%%", compression_ratio);
  }

  close(infile);
  close(outfile);

  return 0;
}
