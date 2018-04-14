
include $(ROOT)/Makefile.d/framework.defs

# additional settings, sources, and prerequisites for particular targets

SOURCES = $(wildcard *.c *.cpp)
#HEADERS = $(wildcard *.h *.hpp)

IFLAGS += -I .

CXXFLAGS += -fPIC


export


# no default goal
# (define it with recursively expanded assignment from any empty variable)
.DEFAULT_GOAL = $@


.PHONY: clean
clean:
	$(MAKE) -f '$(ROOT)/Makefile.d/clean.makecmdgoals' $@


framewrk:
	$(info $(\) @@@ ($@: $^))
	$(MAKE) -f '$(ROOT)/Makefile.d/compile.makecmdgoals' $@
	$(MAKE) -f '$(ROOT)/Makefile.d/ar2.makecmdgoals' $@

