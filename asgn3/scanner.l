%{

#include "lyutils.h"

#define YY_USER_ACTION  { lexer::advance(); }

%}

%option 8bit
%option debug
%option nodefault
%option nounput
%option noyywrap
%option warn
%option verbose

LETTER          [A-Za-z_]
DIGIT           [0-9]
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
INT             ({DIGIT}+)
CHAR            ('([^\\'\n]|\\[\\'"0nt])')
STRING          (\"([^\\"\n]|\\[\\'"0nt])*\")
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)
%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"%"             { return yylval_token ('%'); }
"!"             { return yylval_token ('!'); }
"="             { return yylval_token ('='); }
"=="            { return yylval_token (TOK_EQ); }
"!="            { return yylval_token (TOK_NE); }
"<"             { return yylval_token (TOK_LT); }
"<="            { return yylval_token (TOK_LE); }
">"             { return yylval_token (TOK_GT); }
">="            { return yylval_token (TOK_GE); }
","             { return yylval_token (','); }
"."             { return yylval_token ('.'); }
";"             { return yylval_token (';'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
"}"             { return yylval_token ('}'); }
"[]"            { return yylval_token (TOK_ARRAY); }
if              { return yylval_token (TOK_IF); }
else            { return yylval_token (TOK_ELSE); }
while           { return yylval_token (TOK_WHILE); }
return          { return yylval_token (TOK_RETURN); }
int             { return yylval_token (TOK_INT); }
string          { return yylval_token (TOK_STRING); }
struct          { return yylval_token (TOK_STRUCT); }
void            { return yylval_token (TOK_VOID); }
new             { return yylval_token (TOK_NEW); }
null            { return yylval_token (TOK_NULL); }
{INT}           { return yylval_token (TOK_INTCON); }
{CHAR}          { return yylval_token (TOK_CHARCON); }
{STRING}        { return yylval_token (TOK_STRINGCON); }
{IDENT}         { return yylval_token (TOK_IDENT); }

{NOTNUMBER}     { lexer::badtoken (yytext);
                  return yylval_token (NUMBER); }
.               { lexer::badchar (*yytext); }

%%

