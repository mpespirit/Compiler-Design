// $Id: emitter.h,v 1.1 2015-07-09 14:08:38-07 - - $

#ifndef __EMIT_H__
#define __EMIT_H__

#include "astree.h"

extern FILE* fOil;

void emit_global(astree* tree);
void emit_stringcon (astree* tree);
void emit_prolog ();
void emit_sm_code ( astree* node);

#endif

