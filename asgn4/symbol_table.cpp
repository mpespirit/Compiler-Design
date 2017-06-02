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
void print_table(FILE* file, symbol_table* st);

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
}

void semantic_analysis (astree* node){
   //pre-order actions
   pre_order(node);
   for (astree* child: node->children){
      semantic_analysis(child);
   }
   post_order(node);
}

string attr_tostring(size_t i){
   switch(i){
      case 0: return "void";
      case 1: return "char";
      case 2: return "int";
      case 3: return "null";
      case 4: return "string";
      case 6: return "array";
      case 7: return "function";
      case 8: return "variable";
      case 10: return "typeid";
      case 11: return "param";
      case 12: return "lval";
      case 13: return "const";
      case 14: return "vreg";
      case 15: return "vaddr";
      case 16: return "bitset_size";
   }
   return "invalid";
}

void print_attr(FILE* file, attr_bitset a){
   for(size_t i=0; i<16; ++i){
      if(a[i]){
         fprintf(file, " %s", attr_tostring(i).c_str() );
      }
   }
}

/*
void print_stack(FILE* file){
   int i=0;
   for(;i<symbol_stack.size(); i++){
      fprintf(file, "%*s", i*3, "");
      //print_table(file, st);
      print_table(file, symbol_stack[i]);
      i++;
   }
}
*/

void print_global ( FILE* file, astree* node ){
   if ( global != nullptr ){
      for ( auto e = global->begin(); e != global->end(); e++ ){
         symbol* s = e->second;
         fprintf(file, "%s (%zu.%zu.%zu) {%zu}",
                    e->first->c_str(), node->lloc.filenr, 
                    node->lloc.linenr, node->lloc.offset,
                    e->second->block_nr);
         fprintf(file, " struct \"%s\" \n", 
                 e->second->struct_ID->c_str() );
         if( s->fields != nullptr ){
          for ( auto f = s->fields->begin(); f!=s->fields->end(); f++){
            symbol* t = f->second;
            fprintf(file, "%s (%zu.%zu.%zu) ",
                    f->first->c_str(), t->lloc.filenr,
                    t->lloc.linenr, t->lloc.offset);
            fprintf(file, " field {%s} \n",
                 e->first->c_str() );
          } 
         }
         //if(strcmp(node->lexinfo->c_str(), "struct") == 0){
         //   fprintf(file, "{%s} %s \"%s\" ", "block",
         //           node->lexinfo->c_str(), "meh" );
         }
         fprintf(file, " \n");
      }
   }


void print_table( FILE* file, symbol_table* st ){
      for ( auto e = st->begin(); e != st->end(); e++ ){
         symbol* s = e->second;
         size_t filenr = s->lloc.filenr;
         size_t linenr = s->lloc.linenr;
         size_t offset = s->lloc.offset;
         size_t block = s->block_nr;   
         fprintf(file, "%s (%zu.%zu.%zu) {%zu}",
                    e->first->c_str(), filenr, linenr,
                    offset, block);
         fprintf(file, "\n");
      }
   }


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
   print_attr(file, s->attr);
   fprintf(file, "\n");
}

void insert_struct(astree* node){
   if ( global==nullptr ) global = new symbol_table;
   node->attr[ATTR_struct]=1;
   symbol* s = new_symbol( node );
   s->block_nr=0;
   const string* key = make_key( node->children[0]);
   s->struct_ID=key;
   fprintf(fSym, "%s (%zu.%zu.%zu) {%zu}",
                    key->c_str(), s->lloc.filenr,
                    s->lloc.linenr, s->lloc.offset,
                    s->block_nr);
         fprintf(fSym, " struct \"%s\" \n",
                 key->c_str() );
   if ( node->children.size() > 1){
      size_t i;
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
   symbol_entry e = symbol_entry( key, s );
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
   //fprintf(stderr, "Pre-Order Action:\n");
   switch( node->symbol ){
      case TOK_STRUCT:
         fprintf(stderr, "Found struct\n");
         insert_struct( node );
         print_global( fSym, node );
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
      case TOK_PARAMLIST:
         insert_params( node );
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

