// Paula Espiritu (mespirit@ucsc.edu)
// Spenser Estrada (spmestra@ucsc.edu)

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
#include <bitset>
using namespace std;

#include "auxlib.h"

struct location {
   size_t filenr;
   size_t linenr;
   size_t offset;
};

enum { ATTR_void, ATTR_char, ATTR_int, ATTR_null,
  ATTR_string, ATTR_struct, ATTR_array, ATTR_function,
  ATTR_variable, ATTR_field, ATTR_typeid, ATTR_param,
  ATTR_lval, ATTR_const, ATTR_vreg, ATTR_vaddr,
  ATTR_bitset_size,
};

using attr_bitset = bitset<ATTR_bitset_size>;

struct astree {

   // Fields.
   int symbol;               // token code
   location lloc;            // source location
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node
   attr_bitset attr;

   // Functions.
   astree (int symbol, const location&, const char* lexinfo);
   ~astree();
   astree* adopt (astree* child1, astree* child2 = nullptr);
   astree* adopt_sym (astree* child, int symbol);
   void dump_node (FILE*);
   void peek_node (FILE*);
   void dump_tree (FILE*, int depth = 0);
   static void dump (FILE* outfile, astree* tree);
   static void print (FILE* outfile, astree* tree, int depth = 0);
};

void destroy (astree* tree1, astree* tree2 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif

