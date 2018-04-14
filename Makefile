
# project directory
ROOT = $(CURDIR)

XLCALL_DIR = $(ROOT)/sdk_xlcall


CXXSTD = -std=c++1y
#CXXSTD = -xc -std=c9x

CPPWINFLAGS = -municode 
#CPPWINFLAGS += -D_cdecl=__attribute__\(\(__cdecl__\)\)
CPPWINFLAGS += -D_cdecl=cdecl

CPPFLAGS += -DNDEBUG
dbgCPPFLAGS = $(CPPFLAGS) -UNDEBUG -D_DEBUG

IFLAGS = -I '$(XLCALL_DIR)/include' -I '$(XLCALL_DIR)/src'

CXXFLAGS = -O2
dbgCXXFLAGS = $(CXXFLAGS) -g

LDFLAGS = -static-libgcc -static-libstdc++

# common sources
#SOURCES =


export


# no default goal
# (define it with recursively expanded assignment from any empty variable)
.DEFAULT_GOAL = $@


# project goals and goal specific variables
.PHONY: .phony


sdk_sample/%/: .phony
	$(MAKE) -C 'sdk_sample/$*' $*


sdk_sample/framewrk/%: .phony
	$(MAKE) -C 'sdk_sample/framewrk' $*


sdk_sample/generic/%: .phony
	$(MAKE) -C 'sdk_sample/generic' $*

sdk_sample/generic/: sdk_sample/framewrk/
sdk_sample/generic/generic: sdk_sample/framewrk/
