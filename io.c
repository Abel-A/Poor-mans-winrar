#include "io.h"
#include "code.h"
#include "word.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BLOCK 4096

uint64_t total_syms = 0; // tot # symbols processed.
uint64_t total_bits = 0; // tot # of bits processed. divided by 8. compressed 1
                         // -

static uint8_t sym_buf[BLOCK]; // Buffer for symbols buff_pair and read_pair
static uint16_t sym_cnt = 0;   // index of sym_buff of current symbol
// static uint16_t sym_buf_end = UINT16_MAX;   //index of last pointer in buffer

static uint8_t bit_buf[BLOCK]; // Buffer for bits read_sym
static uint16_t bit_cnt = 0;   //  current buffer bit index.

// wrapper for read to read bytes and symbols to buffer.
int read_bytes(int infile, uint8_t *buf, int to_read) {
  int bytes_read = 0;
  int total_read = 0;

  do {
    bytes_read = read(infile, buf + total_read, to_read - total_read);
    total_read += bytes_read;
    total_bits += bytes_read;

  } while ((total_read != to_read) && (bytes_read != 0));

  return total_read;
}

// wrapper for write to write bytes and symbols to buffer
int write_bytes(int outfile, uint8_t *buf, int to_write) {
  int bytes_write = 0;
  int total_write = 0;

  do {
    bytes_write = write(outfile, buf + total_write, to_write - total_write);
    total_write += bytes_write;
    total_syms += bytes_write;

  } while ((total_write != to_write) && (bytes_write != 0));

  return total_write;
}

// reades header to file.
void read_header(int infile, FileHeader *header) {
  read_bytes(infile, (uint8_t *)header, sizeof(FileHeader));
  // increment total bits
  return;
}

// write header to file.
void write_header(int outfile, FileHeader *header) {
  write_bytes(outfile, (uint8_t *)header, sizeof(FileHeader));
  // increment total bits
  return;
}

// read symbol into buffer.
bool read_sym(int infile, uint8_t *byte) {
  static int end = 0;

  if (sym_cnt == 0) {
    end = read_bytes(infile, sym_buf, 4096);
  }
  *byte = sym_buf[sym_cnt++];
  if (sym_cnt == 4096) {
    sym_cnt = 0;
  }

  if (end == 4096) {
    return true;
  } else {
    if (sym_cnt == (end + 1)) {
      return false;
    } else {
      return true;
    }
  }
}

void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bit_len) {
  // uint16_t code_temp = code;
  for (int i = 0; i < bit_len; i++) {
    if ((code & ((uint16_t)1)) != 0) {
      bit_buf[bit_cnt / 8] |= (1 << (bit_cnt % 8));
    } else {
      bit_buf[bit_cnt / 8] &= ~(1 << (bit_cnt % 8));
    }
    bit_cnt = bit_cnt + 1;
    code >>= 1;
    if (bit_cnt == BLOCK * 8) {
      write_bytes(outfile, bit_buf, BLOCK);
      bit_cnt = 0;
    }
  }

  // uint8_t sym_temp = sym;
  for (int i = 0; i < 8; i++) {
    if ((sym & ((uint8_t)1)) != 0) {
      bit_buf[bit_cnt / 8] |= (1 << (bit_cnt % 8));
    } else {
      bit_buf[bit_cnt / 8] &= ~(1 << (bit_cnt % 8));
    }
    bit_cnt = bit_cnt + 1;
    sym >>= 1;
    if (bit_cnt == BLOCK * 8) {
      write_bytes(outfile, bit_buf, BLOCK);
      bit_cnt = 0;
    }
  }
}

// writes to file if byte_buff is less than whole full at the end.
void flush_pairs(int outfile) {
  int bytes = 0;
  if (bit_cnt != 0) {
    if ((bit_cnt % 8) == 0) {
      bytes = bit_cnt / 8;
    } else {
      bytes = (bit_cnt / 8) + 1;
    }
    write_bytes(outfile, bit_buf, bytes);
    bit_cnt = 0;
  }
}

// reads pairs from file to buffer.
bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bit_len) {
  *sym = 0;
  *code = 0;
  for (int i = 0; i < bit_len; i++) {
    if ((bit_cnt == 0) || (bit_cnt == BLOCK * 8)) {
      read_bytes(infile, bit_buf, 4096);
      bit_cnt = 0;
    }

    if ((bit_buf[bit_cnt / 8]) & (1 << (bit_cnt % 8))) {
      code[i / 16] |= (1 << (i % 16));
    } else {
      code[i / 16] &= ~(1 << (i % 16));
    }
    bit_cnt = bit_cnt + 1;
  }

  for (int i = 0; i < 8; i++) {
    if ((bit_cnt == 0) || (bit_cnt == BLOCK * 8)) {
      read_bytes(infile, bit_buf, 4096);
      bit_cnt = 0;
    }
    if ((bit_buf[bit_cnt / 8]) & (1 << (bit_cnt % 8))) {
      sym[i / 8] |= (1 << (i % 8));
    } else {
      sym[i / 8] &= ~(1 << (i % 8));
    }
    bit_cnt = bit_cnt + 1;
  }

  return *code != STOP_CODE;
  // return false when you read stop code.
}

// moves words from buffer to
void buffer_word(int outfile, Word *w) {
  for (uint32_t i = 0; w->len > i; i++) {
    sym_buf[sym_cnt++] = w->syms[i];
    if (sym_cnt == 4096) {
      write_bytes(outfile, sym_buf, 4096);
      sym_cnt = 0;
    }
  }
}

// flushs left words in sym_buf
void flush_words(int outfile) {
  if (sym_cnt != 0) {
    write_bytes(outfile, sym_buf, sym_cnt);
    sym_cnt = 0;
  }
}
