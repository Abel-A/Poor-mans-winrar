#include "trie.h"
#include "code.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stddef.h>

TrieNode *trie_node_create(uint16_t index) {

  if (!index) {
    return (void *)0;
  }

  TrieNode *node = (TrieNode *)calloc(1, sizeof(TrieNode));
  if (node) {
    node->code = index;
    // initalize trie node
  }

  for (int i = 0; i < ALPHABET; i++) {
    if (node) {
      node->children[i] = NULL;
    }
  }

  // initalizes all childeren of node to be null
  // stores index in code

  return node;
}

void trie_node_delete(TrieNode *n) {
  if (n) {
    free(n);
  }
  // frees space allocated to node;
  // keeps childeren.
}

TrieNode *trie_create(void) {

  TrieNode *node = (TrieNode *)calloc(1, sizeof(TrieNode));
  // constructor for root node;

  for (int i = 0; i < ALPHABET; i++) {
    if (node) {
      node->children[i] = NULL;
    }
  }
  // initalize of all childeren of root to NULL
  if (node) {
    node->code = EMPTY_CODE;
  }
  // initalize trie node start to empty_code

  return node;
}

void trie_reset(TrieNode *root) {
  for (int i = 0; i < ALPHABET; i++) {
    if (root->children[i] != NULL) {
      // set each value to NULL
      trie_reset(root->children[i]);
      // recursively go into each root of every child.
      free(root->children[i]);
      // made a change
    }
  }
}
// delete trinode.

void trie_delete(TrieNode *n) {
  if (n) {
    // checks if input is NULL or not.
    for (int i = 0; i < ALPHABET; i++) {
      if (n->children[i] != NULL) {
        // set each value to NULL
        trie_delete(n->children[i]);
        // recursively go into each root of every child.
        free(n->children[i]);
      }
    }
    trie_node_delete(n);
    n = NULL;
    // free(n);
  }
}

TrieNode *trie_step(TrieNode *n, uint8_t sym) {
  if (!n) {
    return NULL;
    // if node does not exisist return void *
  }
  if (n->children[sym]) {
    return n->children[sym];
  } else {
    return NULL;
  }

  // can not find the value of sym in any of the code of childeren of input n.
}
