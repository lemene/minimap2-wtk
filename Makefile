CFLAGS=		-g -Wall -O2 -Wc++-compat #-Wextra
CPPFLAGS=	-DHAVE_KALLOC
INCLUDES=	-I./minimap2
OBJS=		
PROG=		minimap2-wtk
LIBS=		-lm -lz -lpthread -L./minimap2

ifneq ($(aarch64),)
	arm_neon=1
endif

ifeq ($(arm_neon),) # if arm_neon is not defined
ifeq ($(sse2only),) # if sse2only is not defined
	OBJS+=ksw2_extz2_sse41.o ksw2_extd2_sse41.o ksw2_exts2_sse41.o ksw2_extz2_sse2.o ksw2_extd2_sse2.o ksw2_exts2_sse2.o ksw2_dispatch.o
else                # if sse2only is defined
	OBJS+=ksw2_extz2_sse.o ksw2_extd2_sse.o ksw2_exts2_sse.o
endif
else				# if arm_neon is defined
	OBJS+=ksw2_extz2_neon.o ksw2_extd2_neon.o ksw2_exts2_neon.o
    INCLUDES+=-Isse2neon
ifeq ($(aarch64),)	#if aarch64 is not defined
	CFLAGS+=-D_FILE_OFFSET_BITS=64 -mfpu=neon -fsigned-char
else				#if aarch64 is defined
	CFLAGS+=-D_FILE_OFFSET_BITS=64 -fsigned-char
endif
endif

ifneq ($(asan),)
	CFLAGS+=-fsanitize=address
	LIBS+=-fsanitize=address -ldl
endif

ifneq ($(tsan),)
	CFLAGS+=-fsanitize=thread
	LIBS+=-fsanitize=thread -ldl
endif

.PHONY:all clean
.SUFFIXES:.c .o

.c.o:
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $< -o $@

all:$(PROG)



minimap2-wtk:main.o sketch.o wtk.o minimap2/libminimap2.a
		#$(CC) $(CFLAGS) main.o  wtk.o  sketch.o -o $@ -L. -lminimap2 $(LIBS)
		g++ $(CFLAGS) main.o  wtk.o  sketch.o -o $@ -L. -lminimap2 $(LIBS)


minimap2/libminimap2.a:
		cd minimap2 && make

wtk.o: wtk.cpp
		g++ -c $(CPPFLAGS) -std=c++11 -DKSW_SSE2_ONLY -D__SSE2__ $(INCLUDES) $< -o $@

# other non-file targets

clean:
		rm minimap2-wtk *.o -f
		cd minimap2 && make clean

wtk.o: wtk.hpp