# Paula Espiritu (mespirit@ucsc.edu)
# Spenser Estrada (spmestra@ucsc.edu)

DEPSFILE  = Makefile.deps
NOINCLUDE = ci clean spotless
NEEDINCL  = ${filter ${NOINCLUDE}, ${MAKECMDGOALS}}
CPP       = g++ -g -O0 -Wall -Wextra -std=gnu++14
MKDEPS    = g++ -MM -std=gnu++14
GRIND     = valgrind --leak-check=full --show-reachable=yes
FLEX      = flex --outfile=${LEXCPP}
BISON     = bison --defines=${PARSEHDR} --output=${PARSECPP} --xml
XML2HTML  = xsltproc /usr/share/bison/xslt/xml2xhtml.xsl

MODULES   = astree lyutils string_set auxlib symbol_table
HDRSRC    = ${MODULES:=.h}
CPPSRC    = ${MODULES:=.cpp} main.cpp
FLEXSRC   = scanner.l
BISONSRC  = parser.y
PARSEHDR  = yyparse.h
LEXCPP    = yylex.cpp
PARSECPP  = yyparse.cpp
CGENS     = ${LEXCPP} ${PARSECPP}
ALLGENS   = ${PARSEHDR} ${CGENS}
EXECBIN   = oc
ALLCSRC   = ${CPPSRC} ${CGENS}
OBJECTS   = ${ALLCSRC:.cpp=.o}
LEXOUT    = yylex.output
PARSEOUT  = yyparse.output
REPORTS   = ${LEXOUT} ${PARSEOUT}
MODSRC    = ${foreach MOD, ${MODULES}, ${MOD}.h ${MOD}.cpp}
MISCSRC   = ${filter-out ${MODSRC}, ${HDRSRC} ${CPPSRC}}
EXTRA     = README Makefile PARTNER
ALLSRC    = ${FLEXSRC} ${BISONSRC} ${MODSRC} ${MISCSRC} ${EXTRA}
TESTINS   = ${wildcard test*.in}
EXECTEST  = ${EXECBIN} -ly
LISTSRC   = ${ALLSRC} ${DEPSFILE} ${PARSEHDR}
SUBMIT    = ${HDRSRC} ${CPPSRC} ${EXTRA} parser.y scanner.l

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${CPP} -o${EXECBIN} ${OBJECTS}

yylex.o : yylex.cpp
	@ # Suppress warning message from flex compilation.
	${CPP} -Wno-sign-compare -c $<

%.o : %.cpp
	${CPP} -c $<

${LEXCPP} : ${FLEXSRC}
	${FLEX} ${FLEXSRC}

${PARSECPP} ${PARSEHDR} : ${BISONSRC}
	${BISON} ${BISONSRC}
	${XML2HTML} yyparse.xml >yyparse.html

ci : ${ALLSRC} ${TESTINS}
	- checksource ${ALLSRC}
	- cpplint.py.perl ${CPPSRC}
	cid + ${ALLSRC} ${TESTINS} test?.inh

lis : ${LISTSRC} tests
	mkpspdf List.source.ps ${LISTSRC}
	mkpspdf List.output.ps ${REPORTS} \
		${foreach test, ${TESTINS:.in=}, \
		${patsubst %, ${test}.%, in out err log}}

clean :
	- rm ${OBJECTS} ${ALLGENS} ${REPORTS} ${DEPSFILE}
	- rm ${foreach test, ${TESTINS:.in=}, \
		${patsubst %, ${test}.%, out err log}}

spotless : clean
	- rm ${EXECBIN}

rmtests : spotless
	- rm *.oc *.oc.err *.oc.out *.str *.tok *.ast oclib.oh 
	- rm checksource.log gmake.log yyparse.* *.lexyacctrace
	- rm *.log 

deps : ${ALLCSRC}
	@ echo "# ${DEPSFILE} created `date` by ${MAKE}" >${DEPSFILE}
	${MKDEPS} ${ALLCSRC} >>${DEPSFILE}

${DEPSFILE} :
	@ touch ${DEPSFILE}
	${MAKE} --no-print-directory deps

tests : ${EXECBIN}
	touch ${TESTINS}
	make --no-print-directory ${TESTINS:.in=.out}

%.out %.err : %.in
	${GRIND} --log-file=$*.log ${EXECTEST} $< 1>$*.out 2>$*.err; \
	echo EXIT STATUS = $$? >>$*.log

again :
	gmake --no-print-directory spotless deps ci all lis
	
submit : 
	submit cmps104a-wm.s17 late3 ${SUBMIT}
	
check : 
	${GRIND} oc 11-numbers.oc

ifeq "${NEEDINCL}" ""
include ${DEPSFILE}
endif

