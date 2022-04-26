VERBOSE=yes
ifneq (yes,$(VERBOSE))
    Q=@
else
    Q=
endif

CROSS=Y

CROSSBINDIR_IS_Y=m68k-atari-mint-
CROSSBINDIR_IS_N=

CROSSBINDIR=$(CROSSBINDIR_IS_$(CROSS))

UNAME := $(shell uname)
ifeq ($(CROSS), Y)
ifeq ($(UNAME),Linux)
PREFIX=m68k-atari-mint
HATARI=hatari
else
PREFIX=m68k-atari-mint
HATARI=/usr/bin/hatari
endif
else
PREFIX=/usr
endif

DEPEND=depend

LIBCMINI=../../libcmini
LIBCMINI_LIB=$(LIBCMINI)/build

INCLUDE=-I$(LIBCMINI)/include -I/usr/m68k-atari-mint/include -nostdlib
LIBS=-lgem -lpng16 -lz -lm -lcmini -nostdlib -lgcc
CC=$(PREFIX)/bin/gcc

CC=$(CROSSBINDIR)gcc
STRIP=$(CROSSBINDIR)strip
STACK=$(CROSSBINDIR)stack
NATIVECC=gcc

APP=bench.app
TEST_APP=$(APP)

# CHARSET_FLAGS= -finput-charset=ISO-8859-1 \
#               -fexec-charset=ATARIST

CFLAGS= \
	-O2 \
	-g \
	-nostdinc \
	-I/usr/lib/gcc/m68k-atari-mint/4.6.4/include \
	-DFORCE_GEMLIB_UDEF \
	-Wl,-Map,mapfile \
	-std=c99 \
	-Wall \
	$(CHARSET_FLAGS)

SRCDIR=sources
INCDIR=include
INCLUDE+=-I$(INCDIR)

CSRCS=\
	$(SRCDIR)/bench.c \
	$(SRCDIR)/menu.c \
	$(SRCDIR)/window.c \
	$(SRCDIR)/global.c \
	$(SRCDIR)/util.c \
	$(SRCDIR)/intmath.c \
	$(SRCDIR)/dialog.c \
	$(SRCDIR)/rsrc.c \
	$(SRCDIR)/grafwindow.c \
	$(SRCDIR)/offscreen.c \
	$(SRCDIR)/testwindow.c \
	$(SRCDIR)/complexwindow.c \
	$(SRCDIR)/bezierwindow.c \
	$(SRCDIR)/clockwindow.c \
	$(SRCDIR)/rasterwindow.c \
	$(SRCDIR)/rcircles_bb_window.c \
	$(SRCDIR)/rcircles_vdi_window.c \
	$(SRCDIR)/vdiwindow.c \
	$(SRCDIR)/cubewindow.c \
	$(SRCDIR)/fontwindow.c \
	$(SRCDIR)/imgrotwindow.c \
	$(SRCDIR)/terrainwindow.c \
	\
	$(SRCDIR)/natfeats.c

ASRCS=\
	$(SRCDIR)/nkcc-gc.S \
	$(SRCDIR)/nf_asm.S

COBJS=$(patsubst $(SRCDIR)/%.o,%.o,$(patsubst %.c,%.o,$(CSRCS)))
AOBJS=$(patsubst $(SRCDIR)/%.o,%.o,$(patsubst %.S,%.o,$(ASRCS)))
OBJS=$(COBJS) $(AOBJS)

TRGTDIRS=. ./m68020-60 ./m5475 ./mshort ./m68020-60/mshort ./m5475/mshort
OBJDIRS=$(patsubst %,%/objs,$(TRGTDIRS))

#
# multilib flags. These must match m68k-atari-mint-gcc -print-multi-lib output
#
m68020-60/$(APP):CFLAGS += -m68020-60
m5475/$(APP):CFLAGS += -mcpu=5475
mshort/$(APP):CFLAGS += -mshort
m68020-60/mshort/$(APP): CFLAGS += -mcpu=68030 -mshort
m5475/mshort/$(APP): CFLAGS += -mcpu=5475 -mshort

ctest: $(TEST_APP)
all: $(patsubst %,%/$(APP),$(TRGTDIRS))

$(DEPEND): $(ASRCS) $(CSRCS) include/patterns.h
	-rm -f $(DEPEND)
	$(Q)for d in $(TRGTDIRS);\
		do $(CC) $(CFLAGS) $(INCLUDE) -M $(ASRCS) $(CSRCS) | sed -e "s#^\(.*\).o:#$$d/objs/\1.o:#" >> $(DEPEND); \
	done


#
# generate pattern rules for multilib object files.
#
define CC_TEMPLATE
$(1)/objs/rasterwindow.o: $(SRCDIR)/rasterwindow.c include/rasterdraw.h
$(1)/objs/rcircles_bb_window.o: $(SRCDIR)/rcircles_bb_window.c include/rasterdraw.h
$(1)/objs/%.o:$(SRCDIR)/%.c
	$(Q)echo "CC $$<"
	$(Q)$(CC) $$(CFLAGS) $(INCLUDE) -c $$< -o $$@

$(1)/objs/%.o:$(SRCDIR)/%.S
	$(Q)echo "CC $$<"
	$(Q)$(CC) $$(CFLAGS) $(INCLUDE) -c $$< -o $$@

$(1)_OBJS=$(patsubst %,$(1)/objs/%,$(OBJS))
$(1)/$(APP): $$($(1)_OBJS)
	$(Q)echo "CC $$<"
	$(Q)$(CC) $$(CFLAGS) -Wl,-Map -Wl,$(1)/mapfile -o $$@ $(LIBCMINI_LIB)/crt0.o $$($(1)_OBJS) -L$(LIBCMINI_LIB)/$(1) $(LIBS)
	#$(Q)$(STRIP) $$@
endef
$(foreach DIR,$(TRGTDIRS),$(eval $(call CC_TEMPLATE,$(DIR))))

clean:
	@rm -f $(patsubst %,%/objs/*.o,$(TRGTDIRS)) $(patsubst %,%/$(APP),$(TRGTDIRS))
	@rm -f $(DEPEND) mapfile include/patterns.h

.PHONY: printvars
printvars:
	@$(foreach V,$(.VARIABLES), $(if $(filter-out environment% default automatic, $(origin $V)),$(warning $V=$($V))))

.phony: $(DEPEND)

ifneq (clean,$(MAKECMDGOALS))
-include $(DEPEND)
endif

test: $(TEST_APP)
	$(HATARI) --grab -w --tos /usr/share/hatari/etos512k.img \
	--machine falcon -s 14 --cpuclock 32 --cpulevel 3 \
	-d . $(APP)

ftest: $(TEST_APP)
	$(HATARI) --grab -w --tos /usr/share/hatari/tos404.img \
	--machine falcon --cpuclock 32 --cpulevel 3 \
	-d . $(APP)

sttest: $(TEST_APP)
	$(HATARI) --grab -w --tos "/usr/share/hatari/tos106de.img" \
	--machine st --cpuclock 32 --cpulevel 3  --vdi true --vdi-planes 4 \
	--vdi-width 640 --vdi-height 480 \
	-d . $(APP)
