# this is due to the clean-tempfiles needing to happen after prng-xorshift
.NOTPARALLEL: all

CSTD   := c11
CPPSTD := c++11

ifeq "$(CXX)" "g++"
	GCCVERSIONLT48 := $(shell expr `gcc -dumpversion` \< 4.8)
	ifeq "$(GCCVERSIONLT48)" "1"
		CSTD := c99
		CPPSTD := c++0x
	endif
endif

CFLAGS   := -pedantic -std=$(CSTD) -Wall -Werror -O3
CPPFLAGS := -pedantic -std=$(CPPSTD) -Wall -Werror -O3
LIBFLAGS  := 

all : prng-xorshift clean-tempfiles

prng-xorshift : prng-xorshift.o
	$(CXX) $(CPPFLAGS) -o prng-xorshift prng-xorshift.o $(LIBFLAGS)

%.o : %.cc
	$(CXX) $(CPPFLAGS) -MD -c $*.cc

test : all
	./prng-xorshift | head -c 4K | hd

merge-master: test
	git checkout master
	git merge dev
	git push
	git checkout dev

install:
	./install.sh

uninstall:
	./uninstall.sh

clean-tempfiles :
	rm -f *.d *.o

clean : clean-tempfiles
	rm -f prng-xorshift

-include *.d
