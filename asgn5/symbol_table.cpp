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

#include <stdio.h>
#include <string.h>

#include "symbol_table.h"
#include "yyparse.h"

// Instead of writing "std::string", we can now just write "string"
using namespace std;
FILE* fSym;
bool new_block = false;
size_t depth = 0;

// this is where to put struct symbols and their fields
symbol_table* global;

// a symbol has most of the same fields as AST node
// Also has block number and parameters (i.e., of a function) 
symbol* new_symbol (astree* node){
   symbol* sym = new symbol();
   sym->lloc = node->lloc;
   sym->block_nr = 0;
   sym->attr = node->attr;
   sym->parameters = nullptr;
   return sym;
}

// just moving these up so later funcctions can see them
void pre_order (astree* node);
void post_order(astree* node);
void print_table(FILE* file, symbol_table* st);

// makes a constant string "key" from AST nodes lexigraphic info
const string* make_key ( astree* node ){
   return const_cast<string*>(node->lexinfo);
}

// makes a symbol table entry from AST node
// Not mandatory to make an entry thus, but is often convenient
symbol_entry make_entry(astree* node){
   symbol* s = new_symbol(node);
   const string* key = make_key(node);
   return symbol_entry(key, s);
}

// preferred and safe way to insert a symbol into table
void insert_sym ( symbol_table* st,  symbol_entry e ){
   if ( st->count(e.first) ) st->erase(e.first);
   st->insert(e);
}

// depth-first traversal of AST
// This function is the main purpose for this file
void semantic_analysis (astree* node){
   //pre-order actions
   pre_order(node);
   for (astree* child: node->children){
      semantic_analysis(child);
   }
   post_order(node);
}

// Maps the entries of the attribute bitset to a string 
// Supports print_attr() below
string attr_tostring(size_t i){
   switch(i){
      case 0: return "void";
      case 1: return "int";
      case 2: return "null";
      case 3: return "string";
      case 4: return "";
      case 5: return "array";
      case 6: return "function";
      case 7: return "variable";
      case 9: return "typeid";
      case 10: return "param";
      case 11: return "lval";
      case 12: return "const";
      case 13: return "vreg";
      case 14: return "vaddr";
      case 15: return "bitset_size";
   }
   return "invalid";
}

// Runs through attribute bitset in symbol or AST node
// Prints appropriate string for "true" attributes
void print_attr(FILE* file, attr_bitset a){
   for(size_t i=0; i<16; ++i){
      if(a[i]){
         fprintf(file, " %s", attr_tostring(i).c_str() );
      }
   }
}

// Turns a symbol table entry into a string for printing
void print_entry(FILE* file, symbol_entry e){
   symbol* s = e.second;
   size_t filenr = s->lloc.filenr;
   size_t linenr = s->lloc.linenr;
   size_t offset = s->lloc.offset;
   size_t block = s->block_nr;
   fprintf(file, "%*s", ((int)depth)*3, "");
   fprintf(file, "%s (%zu.%zu.%zu) {%zu}",
           e.first->c_str(), filenr, linenr,
           offset, block);
   if ( s->attr[4] )
      fprintf(file, " struct \"%s\"", s->struct_ID->c_str());
   print_attr(file, s->attr);
   fprintf(file, "\n");
}

// Function for insertion of struct to global symbol table
// Inserts fields as appropriate
void insert_struct(astree* node){
   // need table to exist 
   if ( global==nullptr ) global = new symbol_table;
   // node gets struct attribute
   node->attr[ATTR_struct]=1;
   symbol* s = new_symbol( node );
   // structs are all in block zero
   s->block_nr=0;
   const string* key = make_key( node->children[0]);
   // often need to print appropriate struct name
   s->struct_ID=key;
   // prints newly created entry in specified format
   fprintf(fSym, "%s (%zu.%zu.%zu) {%zu}",
                    key->c_str(), s->lloc.filenr,
                    s->lloc.linenr, s->lloc.offset,
                    s->block_nr);
         fprintf(fSym, " struct \"%s\" \n",
                 key->c_str() );
   // need to insert fields if present
   if ( node->children.size() > 1){
      size_t i;
      // inserts/prints fields basically same way as struct itself
      for (i=1; i < node->children.size(); i++){
         const string* kex = make_key(node->children[i]);
         symbol* t = new_symbol(node->children[i]);
         fprintf(fSym, "%s (%zu.%zu.%zu) ",
                    kex->c_str(), t->lloc.filenr,
                    t->lloc.linenr, t->lloc.offset);
         fprintf(fSym, " field {%s} \n",
                 key->c_str() );
         insert_sym( s->fields, 
                     symbol_entry( kex, t ) );
      } 
   }
   // struct w/ fields inserted into global table
   symbol_entry e = symbol_entry( key, s );
   insert_sym( global, e);
}

void insert_vardecl(astree* node){
   node->attr[ATTR_lval] = 1;
   switch(node->children[0]->symbol){
      case TOK_VOID:
         node->attr[ATTR_void] = 1; break;
      case TOK_INT:
         node->attr[ATTR_int] = 1; break;
      case TOK_STRING:
         node->attr[ATTR_string] = 1; break;
      case TOK_TYPEID:
         node->attr[ATTR_struct] = 1; 
         break;
   }   
   symbol* s = new_symbol( node );
   if (s->attr[4]) s->struct_ID=node->children[0]->lexinfo;
   const string* key;
   if ( node->children[0]->symbol==TOK_ARRAY )
      key = make_key(node->children[0]->children[1]);
   else
      key = make_key(node->children[0]->children[0]);
   s->block_nr = block_stack.back();
   symbol_entry e = symbol_entry(key, s);
   insert_sym( symbol_stack.back(), e );
   print_entry(fSym, e);
}

void insert_func(astree* node){
   node->attr[ATTR_function] = 1;
   switch(node->children[0]->symbol){
      case TOK_VOID: 
         node->attr[ATTR_void] = 1; break;
      case TOK_INT:
         node->attr[ATTR_int] = 1; break;
      case TOK_STRING:
         node->attr[ATTR_string] = 1; break;
      case TOK_TYPEID:
         node->attr[ATTR_struct] = 1; break;
   }
   symbol* s = new_symbol( node );
   if( node->attr[4] ) s->struct_ID = node->children[0]->lexinfo; 
   s->block_nr=0;
   const string* key = make_key(node->children[0]->children[0]);
   symbol_entry e = symbol_entry(key, s);
   insert_sym( symbol_stack.back(), e);
   print_entry(fSym, e);
}

void insert_params(astree* node){
   if( !node->children.empty() ){
      depth++;
      astree* temp = node->children[0];
      for(; !temp->children.empty(); temp=temp->children[1] ){
         temp->attr[ATTR_variable] = 1;
         temp->attr[ATTR_lval] = 1;
         temp->attr[ATTR_param] = 1;

         symbol* s = new_symbol( temp );
         s->block_nr=next_block;
         const string* key = make_key( temp->children[0] );
         symbol_entry e = symbol_entry(key, s);
         insert_sym(symbol_stack.back(), e);
         print_entry(fSym, e); 
         if (temp->children.size()==1) break;
      }
      depth--;
   }
   //fprintf(fSym, "\n");
}

// checks if block has any variable declarations
bool has_var( astree* node){
   if ( !node->children.empty() ){
      for ( size_t i=0; i < node->children.size(); i++){
         if ( node->children[i]->symbol==TOK_VARDECL )
             return true;
      }
   }
   return false;
}
/*
bool check_sym (astree* node, int sym){
   if ( !node->children.empty()){
      for(size_t i = 0; i < node->children.size(); i++){
         if(node->children[i]->symbol==sym) 
            return false;
      }
   }
   return true;
}
*/
// checks for vardecl in block, pushes new block number to
// block_stack, pushes new symbol table to symbol stack if present
void enter_block( astree* node ){
   //print_table(fSym, symbol_stack.back());
   if ( has_var(node) ){
      //print_table(fSym, symbol_stack.back());
      block_stack.push_back( next_block );
      next_block++;
      symbol_table* st = new symbol_table;
      symbol_stack.push_back( st );
      depth++;
   }
}

// checks for vardecl in block. If present, pops block stack,
// pops symbol table stack, and prints the symbol table corresponding
// to block being left.
void leave_block ( astree* node ){
   //print_stack(fSym);
   if ( has_var(node) ){
      //if (bottom(node)) 
      //print_table(fSym, symbol_stack.back() );
      block_stack.pop_back();
      symbol_stack.pop_back();
      depth--;
   }
}

// action to be taken during *descent* into tree
void pre_order (astree* node){
   switch( node->symbol ){
      case TOK_STRUCT:
         insert_struct( node );
         break;
      case TOK_BLOCK:
         enter_block( node );
         break;
      case TOK_VARDECL:
         insert_vardecl( node ); 
         break;
      case TOK_FUNCTION:
         insert_func( node );
         break;
      case TOK_PROTOTYPE:
         insert_func( node);
         break;
      case TOK_PARAMLIST:
         insert_params( node );
         break;
      default: break;
   }
}

// actions to be taken during *ascent* through tree
void post_order(astree* node){
   switch (node->symbol){
      case TOK_BLOCK:
         leave_block( node );
      default: break;
   }
}

