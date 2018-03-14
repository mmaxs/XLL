# vim: ft=markdown
# makecmdgoals framework: readme
# copyright (c) 2018 Mikhail Usenko <michaelus@tochka.ru>



##### PROJECT ROOT DIRECTORIES

ROOT - project root directory
note:
	shall be defined once in top-level Makefile
	e.g.: ROOT = $(CURDIR)

BUILD_ROOT - build output root directory
note:
	shall be defined once in top-level Makefile
	e.g.: BUILD_ROOT = $(ROOT)/build


##### TOOLCHAIN'S OS PLATFORM / OS API SPECIFICS

HOST_OS - toolchain's OS platform (case insensitive value): values returned by `uname -o`: Linux, Msys, Cygwin, ...
note:
	shall be defined once in top-level Makefile or be set as a command-line argument for Make run,
	e.g.: make PLATFORM=windows foo

HOST_ARCH - toolchain's OS platform architecture (case insensitive value): values returned by `uname -m`: x86_64, i686, ...

WINVER - the minimum required Windows API version
note:
	[Using the Windows Headers](https://msdn.microsoft.com/en-us/library/windows/desktop/aa383745(v=vs.85).aspx)
	[What’s the difference between WINVER, _WIN32_WINNT, _WIN32_WINDOWS, and _WIN32_IE?](https://blogs.msdn.microsoft.com/oldnewthing/20070411-00/?p=27283)
	[Modifying WINVER and _WIN32_WINNT](https://msdn.microsoft.com/en-us/library/6sehtctf.aspx)
	<sdkddkver.h>

##### TOOLCHAIN SPECIFICS

CXX - C++ compiler program
note:
	Make's predefined built-in: CXX = g++

CXXRUNFLAGS - switches controlling toolchain run mode
e.g.: -pipe	-v

cxxverbose - if defined non-empty, force printing the commands executed to run the stages of compilation
note:
	this variable is primarily intended to be used as a quick command line switch for Make run
	to let seeing the actual toolchain program commands being executed, e.g.: make cxxverbose=1 foo

CPP - C preprocessor program
note:
	Make's predefined built-in: CPP = $(CC) -E


### C PREPROCESSOR FLAGS

CPPFLAGS - (Make's built-in) extra C preprocessor flags

IFLAGS - options for preprocessor directory search
e.g.: -I ... -iquote ...

CPPWINFLAGS -



##### COMPILER FLAGS

CXXSTD - flags specifying C++ language standard and features used in program sources:
e.g.: -std=... -fconcepts

CXXFLAGS - (Make's buil-in) extra C++ compiling flags, code generating, and C/C++ ABI flags

CXXWINFLAGS - extra MS Windows specific compiler flags being automatically added to CXXFLAGS when PLATFORM is set to Windows
note: Windows additional GCC options:
	-mconsole -mwindows -municode
	-mdll -mnop-fun-dllimport
	-mwin32 -mthread
	-fno-set-stack-executable -fwritable-relocated-rdata -mpe-aligned-commons
'-mdll' is automatically used instead of '-shared' as necessary in link-so.makecmdgoals

WFLAGS - compiler warnings flags

TARGET_ARCH_FLAGS - (Make's buil-in) target binaries architecture and processor instructions subset specifics
e.g.: -mtune=... -msse4.2


##### LINKER FLAGS

LDFLAGS - (Make's buil-in)


static linking
 use LDSTATIC variable to specify static linking options

 for simple applications on Windows, we normally prefer the standard C/C++ libraries
 to have been statically linked as far as the system usually lacks the gcc's runtime libraries,
 e.g.:
  LDSTATIC = -static-libgcc -static-libstdc++
 or
  make LDSTATIC='-static-libgcc -static-libstdc++' <goal>
 or define the target-specific assignment for appropriate goals in local makefiles
  <goal>: LDSTATIC = -static-libgcc -static-libstdc++

 in any case all other the mingw32/gcc/OS libraries are forced to be always linked dynamically when
 the -static flag is not specified, which also prevents dynamic linking with shared libraries at all,
 therefore if there is a specific library that we want to link with dynamically, we should then
 switch static linking off, specify the desired library, and then switch static linking state back on again
 using LDLIBS variable, e.g.:
  LDLIBS = -Wl,-Bdynamic -L $(LIBUV) -luv -Wl,-Bstatic
 or, instead, just directly specify the desired .so/.dll file for dynamic linking with, e.g.:
  LDLIBS += $(LIBUV)/libuv.dll
 which can be more preferable in this case and is as good as specifing a particular .a/.lib file
 for static linking with some desired library, e.g.:
  LDLIBS += $(LIBUV)/libuv.a

building a shared library
 use Makefile.d/link.rules and define the target-specific assignment
 to CXXFLAGS, LDFLAGS, and LDOUT for appropriate goals, e.g.
  <goal>: CXXFLAGS += -fPIC
  <goal>: private override LDFLAGS += -shared
  <goal>: private LDOUT = lib$@.so

 alternatively, use Makefile.d/link-so.rules which basically do the above but leaving aside CXXFLAGS
 (so don't forget about specifying -fpic/-fPIC options requred for compilation stage),
 also when Makefile.d/link-so.rules is used if SONAME_VERSION variable is nonempty, LDFLAGS is added with:
  -Wl,-soname=$(notdir $(LDOUT)).$(SONAME_VERSION)
 and if both SONAME_VERSION and SOFILE_VERSION variables are nonempty, linker output is set to
  $(LDOUT).$(SONAME_VERSION).$(SOFILE_VERSION)

LDLIBS - (Make's buil-in)
note:
	by default dynamically linked versions of libraries are used for ones specified with '-l' linker options;
	if you need some particular library 'foo' from them to be linked statically, you should embrace it with
	'-Wl,-Bstatic -lfoo -Wl,-Bdynamic', or alternatively, just add the pathname of the static library 'libfoo.a'
	for linking with it instead of '-lfoo' option;
	if '-static' option is specified (e.g. in LDFLAGS), this prevents dinamic linking completly with all shared
	libraries, and solely versions of static libraries are used; similarly, you should embrace '-l' option with
	'-Wl,-Bdynamic -lfoo -Wl,-Bstatic' to force a shared version of the particular library 'foo' to be used,
	or alternatively, just add pathname of the shared library 'libfoo.so' for linking with it instead of '-lfoo' option.

LDSHARED - list of the library names that be automatically passed as argumets of '-l' option,
           and are to be linked as shared library regardless of presence of '-static' linker option

LDSTATIC - list of the library names that be automatically passed as argumets of '-l' option,
           and are to be linked statically regardless whether '-static' linker option is specified or not

LDOUT -

LDEXTRA - extra files for liking with the build result that cannot be automatically inferred as object files from the SOURCES



##### OTHER COMMON FRAMEWORK DEFINITIONS



##### SUMMARY OF THE MAKE'S BUILT-IN IMPLICIT RULES AND PREDEFINED DEFAULT VARIABLES

The catalogue for:
	GNU Make 4.2.1
	Built for x86_64-pc-msys

'override' directive GNUMAKEFLAGS := 
automatic %D = $(patsubst %/,%,$(dir $%))
automatic %F = $(notdir $%)
automatic *D = $(patsubst %/,%,$(dir $*))
automatic *F = $(notdir $*)
automatic +D = $(patsubst %/,%,$(dir $+))
automatic +F = $(notdir $+)
automatic <D = $(patsubst %/,%,$(dir $<))
automatic <F = $(notdir $<)
automatic ?D = $(patsubst %/,%,$(dir $?))
automatic ?F = $(notdir $?)
automatic @D = $(patsubst %/,%,$(dir $@))
automatic @F = $(notdir $@)
automatic ^D = $(patsubst %/,%,$(dir $^))
automatic ^F = $(notdir $^)
default .FEATURES := target-specific order-only second-expansion else-if shortest-stem undefine oneshell archives jobserver output-sync check-symlink load
default .INCLUDE_DIRS = /usr/include /usr/include
default .LIBPATTERNS = lib%.dll.a %.dll.a lib%.a %.lib lib%.dll %.dll
default .LOADED := 
default .RECIPEPREFIX := 
default .SHELLFLAGS := -c
default .VARIABLES := 
default AR = ar
default ARFLAGS = rv
default AS = as
default CC = cc
default CHECKOUT,v = +$(if $(wildcard $@),,$(CO) $(COFLAGS) $< $@)
default CO = co
default COFLAGS = 
default COMPILE.C = $(COMPILE.cc)
default COMPILE.F = $(FC) $(FFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
default COMPILE.S = $(CC) $(ASFLAGS) $(CPPFLAGS) $(TARGET_MACH) -c
default COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
default COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
default COMPILE.cpp = $(COMPILE.cc)
default COMPILE.def = $(M2C) $(M2FLAGS) $(DEFFLAGS) $(TARGET_ARCH)
default COMPILE.f = $(FC) $(FFLAGS) $(TARGET_ARCH) -c
default COMPILE.m = $(OBJC) $(OBJCFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
default COMPILE.mod = $(M2C) $(M2FLAGS) $(MODFLAGS) $(TARGET_ARCH)
default COMPILE.p = $(PC) $(PFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
default COMPILE.r = $(FC) $(FFLAGS) $(RFLAGS) $(TARGET_ARCH) -c
default COMPILE.s = $(AS) $(ASFLAGS) $(TARGET_MACH)
default CPP = $(CC) -E
default CTANGLE = ctangle
default CWEAVE = cweave
default CXX = g++
default F77 = $(FC)
default F77FLAGS = $(FFLAGS)
default FC = f77
default GET = get
default LD = ld
default LEX = lex
default LEX.l = $(LEX) $(LFLAGS) -t
default LEX.m = $(LEX) $(LFLAGS) -t
default LINK.C = $(LINK.cc)
default LINK.F = $(FC) $(FFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)
default LINK.S = $(CC) $(ASFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_MACH)
default LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)
default LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)
default LINK.cpp = $(LINK.cc)
default LINK.f = $(FC) $(FFLAGS) $(LDFLAGS) $(TARGET_ARCH)
default LINK.m = $(OBJC) $(OBJCFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)
default LINK.o = $(CC) $(LDFLAGS) $(TARGET_ARCH)
default LINK.p = $(PC) $(PFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)
default LINK.r = $(FC) $(FFLAGS) $(RFLAGS) $(LDFLAGS) $(TARGET_ARCH)
default LINK.s = $(CC) $(ASFLAGS) $(LDFLAGS) $(TARGET_MACH)
default LINT = lint
default LINT.c = $(LINT) $(LINTFLAGS) $(CPPFLAGS) $(TARGET_ARCH)
default M2C = m2c
default MAKE = $(MAKE_COMMAND)
default MAKEFILES := 
default MAKEINFO = makeinfo
default MAKE_COMMAND := make
default MAKE_HOST := x86_64-pc-msys
default MAKE_TERMERR := /dev/pty2
default MAKE_VERSION := 4.2.1
default OBJC = cc
default OUTPUT_OPTION = -o $@
default PC = pc
default PREPROCESS.F = $(FC) $(FFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -F
default PREPROCESS.S = $(CC) -E $(CPPFLAGS)
default PREPROCESS.r = $(FC) $(FFLAGS) $(RFLAGS) $(TARGET_ARCH) -F
default RM = rm -f
default SUFFIXES := .out .a .ln .o .c .cc .C .cpp .p .f .F .m .r .y .l .ym .yl .s .S .mod .sym .def .h .info .dvi .tex .texinfo .texi .txinfo .w .ch .web .sh .elc .el
default TANGLE = tangle
default TEX = tex
default TEXI2DVI = texi2dvi
default WEAVE = weave
default YACC = yacc
default YACC.m = $(YACC) $(YFLAGS)
default YACC.y = $(YACC) $(YFLAGS)
makefile .DEFAULT_GOAL := 
makefile CURDIR := /d/wroot.vba&vbs/XLL/build
makefile MAKEFILE_LIST := 
makefile MAKEFLAGS = p
makefile SHELL = /bin/sh


### Implicit rules conserning to building C/C++ programs:

# LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)
# COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
# %.cc:
# %: %.cc
#         $(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@
# %.o: %.cc
#         $(COMPILE.cc) $(OUTPUT_OPTION) $<
# .cc:
#         $(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@
# .cc.o:
#         $(COMPILE.cc) $(OUTPUT_OPTION) $<

# LINK.cpp = $(LINK.cc)
# COMPILE.cpp = $(COMPILE.cc)
# %.cpp:
# %: %.cpp
#         $(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@
# %.o: %.cpp
#         $(COMPILE.cpp) $(OUTPUT_OPTION) $<
# .cpp:
#         $(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@
# .cpp.o:
#         $(COMPILE.cpp) $(OUTPUT_OPTION) $<

# LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)
# COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
# %.c:
# %: %.c
#         $(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@
# %.o: %.c
#         $(COMPILE.c) $(OUTPUT_OPTION) $<
# .c:
#         $(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@
# .c.o:
#         $(COMPILE.c) $(OUTPUT_OPTION) $<

# LINK.o = $(CC) $(LDFLAGS) $(TARGET_ARCH)
# %.o:
# %: %.o
#         $(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
# .o:
#         $(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

# AR = ar
# ARFLAGS = rv
# %.a:
# (%): %
#         $(AR) $(ARFLAGS) $@ $<
# .a:

