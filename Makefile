CC = gcc
DEBUG =	-g -ggdb -DINFO
CFLAGS =  -c -std=c99 -W -Wall -Werror -fPIC  $(DEBUG)

LIB_OBJS = \
	./engine/db.o\
	./engine/sst.o\
	./engine/log.o\
	./engine/util.o\
	./engine/meta.o\
	./engine/debug.o\
	./engine/index.o\
	./engine/bloom.o\
	./engine/buffer.o\
	./engine/skiplist.o

LIBRARY = libnessdb.so

all: $(LIBRARY)

clean:
	-rm -f db-bench
	-rm -f bench/db-bench.o
	-rm -f $(LIBRARY)  
	-rm -f $(LIB_OBJS)

cleandb:
	-rm -rf ndbs
	-rm -rf *.event

$(LIBRARY): $(LIB_OBJS)
	$(CC) -pthread  -fPIC -shared $(LIB_OBJS) -o libnessdb.so

db-bench: bench/db-bench.o $(LIB_OBJS)
	$(CC) -pthread  bench/db-bench.o $(LIB_OBJS) -o $@
