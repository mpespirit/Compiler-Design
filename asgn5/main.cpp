// Paula Espiritu (mespirit)
// Spenser Estrada (spmestra)

#include <string>
#include <vector>
using namespace std;

#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "astree.h"
#include "auxlib.h"
#include "string_set.h"
#include "lyutils.h"
#include "symbol_table.h"
#include "emitter.h"

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;
string command;
int d_flag = 0; //-D option flag
char* d_arg; //-D option args
size_t next_block = 1;

vector<symbol_table*> symbol_stack;
vector<size_t> block_stack;

// Function Declarations
char* change_ext(char* name, auto ext);
void cpp_popen(const char* filename);
void cpp_pclose();

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

/*
* -@flags: 
*    - Call set_debugflags and use DEBUGF and DEBUGSMT
* -Dstring: 
*    - Pass option and arg to cpp. 
* -l: 
*    - yy_flex_debug = 1
* -y: 
*    - yydebug = 1
*/
void check_opts(int argc, char** argv){
   yy_flex_debug = 0;
   yydebug = 0; 

   for(int opt; (opt = getopt(argc,argv,"@:D:ly")) != -1;){
      switch(opt){
         case '@':
            set_debugflags(optarg);
            break;
         case 'D':
            d_flag = 1;
            d_arg = optarg;
            break;
         case 'l':
            yy_flex_debug = 1;
            break;
         case 'y':
            yydebug = 1;
            break;
         default: 
            errprintf ("bad option (%c)\n", optopt);
            break;
         }
   }
   if(optind > argc){
      errprintf("Usage: %s [-ly] [filename]\n",
                exec::execname.c_str());
      exit(exec::exit_status);
   }
   const char* filename = optind == argc ? "-" : argv[optind];
   cpp_popen(filename);
}

//Preprocess the file and dump contents into correct output files
void cpp_popen(const char* filename){
   if(d_flag == 1) command = CPP + " -D " + d_arg + " " + filename;
   else command = CPP + " " + filename;

   yyin = popen(command.c_str(), "r");
   char* program = strdup(filename);

   if(yyin == NULL){
      syserrprintf(command.c_str());
      exit(exec::exit_status);
   } else {
      if(yy_flex_debug){
         fprintf(stderr, "-- popen (%s), fileno(yyin) = %d\n",
                 command.c_str(), fileno(yyin));
      }
      lexer::newfilename (command);
      
      char* sym = change_ext(program, ".sym");
      fSym = fopen(sym, "w");
      
      char* tok = change_ext(program, ".tok");
      fTok = fopen(tok, "w");      
      int pVal = yyparse(); 
      if (pVal != 0) fprintf(stderr, "Parse Failure (%i)\n", pVal);
      
      //Print string set table to a specified file in the same dir
      char* str = change_ext(program, ".str");
      FILE* fStr = fopen(str, "w");
      string_set::dump(fStr);   
      
      char* ast = change_ext(program, ".ast");
      FILE* fAst = fopen(ast, "w");
      astree::print(fAst, parser::root, 0);

      //attempt to build symbol table
      block_stack.push_back(0);
      symbol_stack.push_back( new symbol_table );
      semantic_analysis(parser::root);

      fclose(fStr);
      fclose(fTok);
      fclose(fAst);
      fclose(fSym);
   }
}

//Close preprocessor
void cpp_pclose(){
   int pclose_rc = pclose(yyin);
   eprint_status(command.c_str(), pclose_rc);
   if(pclose_rc != 0) exec::exit_status = EXIT_FAILURE;
}

int main (int argc, char** argv) {
   exec::execname = basename (argv[0]);
   int exit_status = EXIT_SUCCESS;

   //Check for oc executable
   if(exec::execname == "./oc"){
      exit_status = EXIT_FAILURE;
      fprintf (stderr, "Incompatible exec file");
   }

   check_opts(argc,argv);
   cpp_pclose();

   return exit_status;
}

//Remove directory path and replace file extension
char* change_ext(char* name, auto ext){
   char* base = basename(name);
   char* dot = strrchr(base, '.');
   if(dot != NULL) *dot = '\0';
   strcat(base, ext);
   return base;
}
