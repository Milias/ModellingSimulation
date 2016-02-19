TARGETS = $(wildcard Week\ *)

all: $(TARGETS)
	make -C @^
