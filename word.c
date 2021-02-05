#include "word.h"
#include "stdio.h"
#include "string.h"

#include "code.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

Word *word_create(uint8_t *syms, uint64_t len) {

  Word *word_table = (Word *)calloc(1, sizeof(Word));
  /// constructor for word table.
  if (word_table) {
    word_table->syms = (uint8_t *)malloc(sizeof(uint8_t) * len);
    // constructor for array of symbols
  }

  if ((syms) && (word_table) && (word_table->syms)) {
    memcpy(word_table->syms, syms, len);
  }
  if (word_table) {
    word_table->len = len;
  }

  return word_table;
}

Word *word_append_sym(Word *w, uint8_t sym) {

  Word *word_table = (Word *)calloc(1, sizeof(Word));
  if (word_table) {
    word_table->len = (w->len) + 1;
  }
  if (word_table) {
    word_table->syms = (uint8_t *)malloc(sizeof(uint8_t) * (word_table->len));
  }
  // constructor for new (Word*)calloc(1, sizeof(Word));word table
  // constructor for new word table.
  // copys Word *w into new word_table.
  if ((w->syms != NULL) && (word_table != NULL) && (word_table->syms != NULL)) {
    memcpy(word_table->syms, w->syms, w->len);
  }

  if (word_table && word_table->syms) {
    word_table->syms[w->len] = sym;
  }

  return word_table;
}

void word_delete(Word *w) {
  if (w) {
    for (int i = 0; i < 8; i++) {
      w->syms[i] = 0;
      // sets all value of syms to 0, just incase of any wierd bugs
    }

    free(w->syms);
    free(w);
    // frees word.

    w = NULL;
    // sets w to null
  }
}

WordTable *wt_create(void) {
  WordTable *word_table = (WordTable *)calloc(MAX_CODE, sizeof(WordTable));
  // change somehting a bit.
  // creates an empty word table
  // has a size of uint16 -1
  // word represents size of empty word

  // word_table[0] = word_create(0, 0);
  // sets memory location of word_table to be empty word.
  // not dynamic memory location. need to
  if (word_table) {
    word_table[EMPTY_CODE] = word_create(0, 0);
  }
  return word_table;
}

void wt_reset(WordTable *wt) {
  for (uint64_t i = START_CODE; i < MAX_CODE; i++) {
    word_delete(wt[i]);
    wt[i] = NULL;
    // iterates through word table and deletes indvidual elements except first
    // empty string.
  }
}

void wt_delete(WordTable *wt) {
  for (uint64_t i = 0; i < MAX_CODE; i++) {
    if (wt[i] != NULL) {
      word_delete(wt[i]);
    }
  }
  free(wt);
}

/*int main(){

    //uint8_t word[] = { 'h', 'e', 'l', 'l', 'o' };
    //uint8_t word2[] = {'g'};

    //Word *testword = word_create(word, 5);
    //Word *test2 = word_append_sym(testword, 'a' );
    //printf("%c", test2->syms[3]); works

    //WordTable *table = wt_create(); works
    //printf("%c", table[0]->syms[0]); works

    //word_delete(testword);
    //printf("%c", testword->syms[3]);

    //WordTable *table = wt_create();
    //wt_delete(table);
    //printf("%c", table[0]->syms[0]);

    return 0;
}*/
