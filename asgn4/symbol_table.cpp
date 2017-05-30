/**
 * Example for using bitsets, symbol tables, etc.
 *
 * Compile with "g++ -std=c++11"
 */

// Import the relevant STL classes
#include <bitset>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "symbol_table.h"

// Instead of writing "std::string", we can now just write "string"
using namespace std;
FILE* fSym;

symbol* new_symbol(astree* node){
   symbol* sym = new symbol();
   sym->lloc = node->lloc;
   sym->block_nr = 0;
   sym->attr = node->attr;
   sym->parameters = nullptr;
   return sym;
}

/*
int main(int argc, char** argv) {
  symbol_table global;
  string* keyA = new string("Key A");
  symbol* a = new symbol();
  // You can turn single attributes flags on by assigning true (1) to the bit
  a->attributes[ATTR_void] = true;
  // You can use the symbol table (unordered_map) like an associative array
  global[keyA] = a;
  // Accessing symbols in the symbol table and checking attributes...
  if (global[keyA]->attributes[ATTR_void]) {
    cout << "Symbol has 'void' attribute" << endl;
  }
}
*/
