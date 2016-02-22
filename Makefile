TARGETS = $(wildcard Week*)

.PHONY : all
all : $(addprefix d,$(TARGETS))

dWeek% :
	$(MAKE) -C $(subst d,,$@)

.PHONY : clean
clean : $(addprefix c,$(TARGETS))

cWeek% :
	$(MAKE) -C $(subst c,,$@) clean

.PHONY : latex
latex : $(addprefix l,$(TARGETS))

lWeek% :
	$(MAKE) -C $(subst l,,$@) latex
