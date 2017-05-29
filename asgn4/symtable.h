#include <vector>
#include <unordered_map>
#include <string>
#include <bitset>
#include "astree.h"

extern size_t next_block;
//vector<symbol_table*> stack;

struct symbol;

//using symbol_table = unordered_map<string*,symbol*>;
//using symbol_entry = pair<const string*, symbol*>;

struct symbol{
   attr_bitset attr;
//   symbol_table* fields;
   location lloc;
   size_t block_nr;
   vector<symbol*>* parameters;
};

symbol* new_symbol(astree* node);
