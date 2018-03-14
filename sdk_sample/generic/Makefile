
include $(ROOT)/Makefile.d/framework.defs

# additional settings, sources, and prerequisites for particular targets

SOURCES = $(wildcard *.c *.cpp)
#HEADERS = $(wildcard *.h *.hpp)
WINDRES_SOURCES = $(wildcard *.rc *.res)
LDEXTRAS += $(wildcard *.def) $({WINDRES_OBJECTS})

IFLAGS += -I '../framewrk' -I .

CXXFLAGS += -mwindows -fPIC

LDFLAGS += -static

LDLIBS += -L '$({CURRENT_BUILD_DIR})/../framewrk'
LDSTATIC += framewrk


export


# no default goal
# (define it with recursively expanded assignment from any empty variable)
.DEFAULT_GOAL = $@


.PHONY: clean
clean:
	$(MAKE) -f '$(ROOT)/Makefile.d/clean.makecmdgoals'


windres:
	$(info $(\) @@@ ($@: $^))
	$(MAKE) -f '$(ROOT)/Makefile.d/windres.makecmdgoals' $@


generic: windres
	$(info $(\) @@@ ($@: $^))
	$(MAKE) -f '$(ROOT)/Makefile.d/compile.makecmdgoals' $@
	$(MAKE) -f '$(ROOT)/Makefile.d/link-so.makecmdgoals' $@



#####

#$(info $(call !tr!,T e S t,$(_),!))
#$(info $(call !tr!,T e S t,T S,!))
#$(info $(call !tr!,T e S t,T S,))

#$(info $(call !to-lower!,T e S t))

#$(info [$(call !join!,$(,),+,a b c, x y z 123 4 5 6)])
#$(info [$(call !join!, ,+, x y z 123 4 5 6,a b c)])
#$(info [$(call !join!, ,+,a,  )])
#$(info [$(call !join!, ,+, ,  )])

#$(info [$(call !map1!,!,firstword,a  b c x y z 123)])
#$(info [$(call !map1!,!,firstword,a)])
#$(info [$(call !map1!,!,firstword, )])

#$(foreach w,a$(\)b$(\)c$(\),$(info [$(w)]))
#t = test
#nil =
#$(info [$(if $(subst $(nil),,$(nil))$(subst $(nil),,$(nil)),t,f)])
#$(info t? $(if $(call !is-equal!,$(nil),$(nil)),t,f))
#$(info t? $(if $(call !is-equal!,,),t,f))
#$(info t? $(if $(call !is-equal!,1 2 3,1 2 3),t,f))
#$(info t? $(if $(call !is-equal!,$(_),$(_)),t,f))
#$(info f? $(if $(call !is-equal!,$(_),$(_)$(_)),t,f))
#$(info f? $(if $(call !is-equal!,, ),t,f))

#t = D:/wroot.vba&vbs/XLL/ss
#t-build = $(call !BUILD-PATHNAME!,$(t))
#$(info $(t-build))
#$(info $(call !BUILD-PATHNAME!,$(t-build)))

#!t! = $(filter $1,$2)
#$(info !t!={$(value !t!)})

#dbg{SOURCES} = test

#$(info [$({SOURCES})])
#$(info [$($(call !is-defined!,{SOURCES}))])
#$(info [$($(call !BUILD-OVERLOAD!,{SOURCES}))])

#$(info {OBJECTS}={$({OBJECTS})})
#$(info [$(call !BUILD-PATHNAME!,$({OBJECTS}))])

#$(info {OBJECT-BUILD-TARGETS}={$({OBJECT-BUILD-TARGETS})})
#$(info {OBJECT-BUILD-TARGET-PREREQUISITES}={$({OBJECT-BUILD-TARGET-PREREQUISITES})})

#$(info [$(call !uniq!,a b c a b c c d a e f a b d g c g)])

#$(info [$(call !add-prefix!,+,*,a,d e f g)])
#$(info [$(call !add-prefix!,+,*,,d e f g)])
#$(info [$(call !add-prefix!,+,*,a,)])
#$(info [$(call !add-suffix!,+,*,a,d e f g)])
#$(info [$(call !add-suffix!,+,*,,d e f g)])
#$(info [$(call !add-suffix!,+,*,a,)])

#$(info [$(call !cartesian!,+,*,a b c,d e f g)])
#$(info [$(call !cartesian!,+,*,,d e f g)])
#$(info [$(call !cartesian!,+,*,a b c,)])

#$(info [$(call !cartesian!,+,*,a b c,d)])
#$(info [$(call !cartesian!,+,*,a,d)])
#$(info [$(call !cartesian!,+,*,a,)])
#$(info [$(call !cartesian!,+,*,,d)])
#$(info [$(call !cartesian!,+,*,,)])

#$(error )

