TARGETS = $(wildcard Week*)

.PHONY : all
all : $(addprefix a,$(TARGETS))

aWeek% :
	$(MAKE) -C $(subst a,,$@)

.PHONY : clean
clean : $(addprefix c,$(TARGETS))

cWeek% :
	$(MAKE) -C Shared clean
	$(MAKE) -C $(subst c,,$@) clean

.PHONY : latex
latex : $(addprefix l,$(TARGETS))

lWeek% :
	$(MAKE) -C $(subst l,,$@) latex
