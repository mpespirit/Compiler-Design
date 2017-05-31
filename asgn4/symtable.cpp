#include "symtable.h"
#include "astree.h"

symbol_table global;
FILE* fSym;

symbol* new_symbol(astree* node){
   symbol* sym = new symbol();
   sym->lloc = node->lloc;
   sym->block_nr = 0
   sym->attr = node->attr;
   sym->parameters = nullptr;
   return sym;
}

void pre_order(astree* node);
void post_order(astree* node);

void semantic_analysis(astree* node){
   // pre-order actions
   pre_order(node);
   for (astree* child: node->children)
      semantic_analysis(child);
   // post-order actions
   post_order(node);
}

void pre_order(astree* node){
   switch (node->symbol){
      case TOK_STRUCT:
         
         break;
      default:
         break;

   }
}
