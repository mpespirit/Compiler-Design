#include <vector>
#include <iostream>
#include <unordered_map>
#include <string>
#include <bitset>
#include "astree.h"

extern size_t next_block;
extern FILE* fSym;
//extern symbol_table* global;// = new symbol_table

struct symbol;

using symbol_table = unordered_map<const string*,symbol*>;
extern vector<symbol_table*> symbol_stack;
extern vector<size_t> block_stack;

using symbol_entry = pair<const string*, symbol*>;

struct symbol{
   attr_bitset attr;
   symbol_table* fields;
   location lloc;
   size_t block_nr;
   vector<symbol*>* parameters;
};

symbol* new_symbol(astree* node);
void semantic_analysis(astree* node);
