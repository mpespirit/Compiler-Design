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
#include "yyparse.h"

// Instead of writing "std::string", we can now just write "string"
using namespace std;
FILE* fSym;
bool new_block = false;

symbol_table* global;// = new symbol_table;

symbol* new_symbol (astree* node){
   symbol* sym = new symbol();
   sym->lloc = node->lloc;
   sym->block_nr = 0;
   sym->attr = node->attr;
   sym->parameters = nullptr;
   return sym;
}

void pre_order (astree* node);
void post_order(astree* node);

/*void insert_sym (symbol_table* st,  astree* node){
   symbol* s = new_symbol(node);
   const string* key = const_cast<string*>(node->lexinfo);
   symbol_entry e = symbol_entry(key, s);
   if ( st->count(e.first) ) st->erase(e.first);
   st->insert(e);
}*/

const string* make_key ( astree* node ){
   return const_cast<string*>(node->lexinfo);
}

symbol_entry make_entry(astree* node){
   symbol* s = new_symbol(node);
   const string* key = make_key(node);
   return symbol_entry(key, s);
}

void insert_sym ( symbol_table* st,  symbol_entry e ){
   if ( st->count(e.first) ) st->erase(e.first);
   st->insert(e);
   fprintf(stderr, "Symbol is in global");
}

void semantic_analysis (astree* node){
   //pre-order actions
   pre_order(node);
   for (astree* child: node->children)
      semantic_analysis(child);
   post_order(node);
}

void print_global ( FILE* file ){
   if ( global != nullptr ){
      for ( auto e = global->begin(); e != global->end(); e++ ){
         fprintf(file, "%s \n", e->first->c_str() ); 
      }
   }
} 

void insert_struct(astree* node){
   if ( global==nullptr ) global = new symbol_table;
   symbol* s = new_symbol( node );
   fprintf(stderr, "Made struct symbol \n");
   if ( node->children.size() > 1){
      size_t i;
      for (i=1; i < node->children.size(); i++){
         insert_sym( s->fields, 
                     make_entry( node->children[i] ) );
      } 
   }
   const string* key = make_key( node->children[0]);
   fprintf(stderr, "Key is \"%s\" \n", key->c_str() ); 
   symbol_entry e = symbol_entry( key, s );
   fprintf(stderr, "Made entry\n");
   insert_sym( global, e);
}

void insert_vardecl(astree* node){
   symbol* s = new_symbol( node );
   const string* key;
   if ( node->children[0]->symbol==TOK_ARRAY )
      key = make_key(node->children[0]->children[1]);
   else
      key = make_key(node->children[0]->children[0]);
   s->block_nr = block_stack.back();
   fprintf(stderr, "%s  %zu  \n", key->c_str(), s->block_nr);
}

bool has_var( astree* node){
   bool has = false;
   if ( !node->children.empty() ){
      size_t i;
      for ( i=0; i < node->children.size(); i++){
         if ( node->children[i]->symbol==TOK_VARDECL )
             has = true;
      }
   }
   return has;
}

void enter_block( astree* node ){
   if ( has_var(node) ){
      block_stack.push_back( next_block );
      next_block++;
      symbol_stack.push_back( new symbol_table );
   }
}

void leave_block ( astree* node ){
   if ( has_var(node) ){
      block_stack.pop_back();
      symbol_stack.pop_back();
   }
}

void pre_order (astree* node){
   //fprintf(stderr, "Pre-Order Action:\n");
   switch( node->symbol ){
      case TOK_STRUCT:
         fprintf(stderr, "Found struct");
         insert_struct( node );
         print_global( stdout );
         break;
      case TOK_BLOCK:
         enter_block( node );
         break;
      case TOK_VARDECL:
         insert_vardecl( node ); 
         break;
      default: break;
   }
}

void post_order(astree* node){
   switch (node->symbol){
      case TOK_BLOCK:
         leave_block( node );
      default: break;
   }
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
