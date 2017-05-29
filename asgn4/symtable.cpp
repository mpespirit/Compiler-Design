#include "symtable.h"
#include "astree.h"


symbol* new_symbol(astree* node){
   symbol* sym = new symbol();
   sym->lloc = node->lloc;
   sym->block_nr = 0;
   sym->attr = node->attr;
   sym->parameters = nullptr;
   return sym;
}

void semantic_analysis(astree* node){
   // pre-order actions
   for (astree* child: node->children)
      semantic_analysis(child);
   // post-order actions
}
