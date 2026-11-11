# libxmhost: what majestic gives the Xiongmai ISP libraries, for any other
# program that runs them -- the open MPI (../libmpi), the MIPI receiver,
# the 3A library's hooks and attribute store, and libispapp's camera_*
# calls. One shared library, so libispapp_XM530, libisp_XM530 and the
# sensor libraries find all of it at load, in the global scope, as they
# find majestic's own.
CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
CFLAGS += -I../isp/include/isp -I../isp/include/xm -I../include \
	  -D_GNU_SOURCE -DCHIPID_XM530 -Werror -Wall -std=c99 -fPIC -O2 \
	  -fvisibility=hidden

MPI = ../libmpi/mpi_sys.c ../libmpi/mpi_vi.c ../libmpi/mpi_region.c
OBJECTS = mipi.o alg.o camera.o $(MPI:../libmpi/%.c=mpi_%.o)

all: libxmhost.so

# The open MPI exports everything, as its archive does.
mpi_%.o: ../libmpi/%.c
	$(CC) -I../include -Werror -Wall -std=c99 -fPIC -O2 -c -o $@ $<

libxmhost.so: $(OBJECTS)
	$(CC) -shared -Wl,-soname,libxmhost.so -o $@ $^
	$(STRIP) $@

clean:
	-rm -f *.o *.so
