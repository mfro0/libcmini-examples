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

INCLUDE=-I$(LIBCMINI)/include \
	-I/usr/m68k-atari-mint/include

LIBS=-lgem -lm -lcmini -nostdlib -lgcc
CC=$(PREFIX)/bin/gcc

CC=$(CROSSBINDIR)gcc
STRIP=$(CROSSBINDIR)strip
STACK=$(CROSSBINDIR)stack
OBJCOPY=$(CROSSBINDIR)objcopy

NATIVECC=gcc

APP=cpxconf.cp
TEST_APP=$(APP)

# CHARSET_FLAGS= -finput-charset=ISO-8859-1 \
#               -fexec-charset=ATARIST

CFLAGS= \
	-O2 \
	-nostdinc \
	-I/usr/lib/gcc/m68k-atari-mint/4.6.4/include \
	-I/usr/m68k-atari-mint/include \
	-DFORCE_GEMLIB_UDEF \
	-Wall \
	-fomit-frame-pointer \
	-fno-merge-constants \
	$(CHARSET_FLAGS)

SRCDIR=sources
INCDIR=include
RSCDIR=rsc
INCLUDE+=-I$(INCDIR) -I$(RSCDIR)

CSRCS=\
	$(SRCDIR)/cpx_preinit.c \
	$(SRCDIR)/cpxconf.c \

ASRCS=\
	$(SRCDIR)/cpxstart.S
HSRCS=\
	$(SRCDIR)/cpxhead.c

COBJS=$(patsubst $(SRCDIR)/%.o,%.o,$(patsubst %.c,%.o,$(CSRCS)))
AOBJS=$(patsubst $(SRCDIR)/%.o,%.o,$(patsubst %.S,%.o,$(ASRCS)))
OBJS=$(COBJS)
HOBJS=$(patsubst $(SRCDIR)/%.o,%.o,$(patsubst %.c,%.o,$(HSRCS)))

CPX=$(patsubst %.cp,%.cpx,$(APP))

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

all: $(DEPEND) $(patsubst %,%/$(APP),$(TRGTDIRS)) $(patsubst %,%/$(CPX),$(TRGTDIRS))

$(DEPEND): $(ASRCS) $(CSRCS)
	-rm -f $(DEPEND)
	$(Q)for d in $(TRGTDIRS);\
	do \
		if [ ! -d $$d/objs ]; then mkdir -p $$d/objs; fi; \
		$(CC) $(CFLAGS) $(INCLUDE) -M $(ASRCS) $(CSRCS) | sed -e "s#^\(.*\).o:#$$d/objs/\1.o:#" >> $(DEPEND); \
	done

#
# generate pattern rules for multilib object files.
#
define CC_TEMPLATE
$(1)/objs/%.o:$(SRCDIR)/%.c
	$(Q)echo "CC $$@"
	$(Q)$(CC) $$(CFLAGS) $(INCLUDE) -c $$< -o $$@

$(1)/objs/%.o:$(SRCDIR)/%.S
	$(Q)echo "CC $$@"
	$(Q)$(CC) $$(CFLAGS) $(INCLUDE) -c $$< -o $$@

$(1)_OBJS=$(patsubst %,$(1)/objs/%,$(OBJS))
$(1)_HOBJS=$(patsubst %,$(1)/objs/%,$(HOBJS))

$(1)/$(APP): $$($(1)_OBJS)  $(1)/objs/$(AOBJS) $$($(1)_HOBJS)
	$(Q)echo "CC $$@"
	$(Q)$(CC) $$(CFLAGS) --traditional -o $$@ $(1)/objs/cpxstart.o $$($(1)_OBJS) -L$(LIBCMINI_LIB)/$(1) $(LIBS) -Wl,-Map -Wl,$(1)/mapfile
	$(Q)$(STRIP) $$@

$(1)/$(CPX): $$($(1)_HOBJS) $(1)/$(APP)
	$(Q)echo "objcopy $(1)/cpxhead"
	$(Q)$(OBJCOPY) -O binary $$($(1)_HOBJS) $(1)/cpxhead
	$(Q)echo "GEN $$@"
	$(Q)cat $(1)/cpxhead $(1)/$(APP) > $(1)/$(CPX)
endef
$(foreach DIR,$(TRGTDIRS),$(eval $(call CC_TEMPLATE,$(DIR))))

clean:
	@rm -f $(patsubst %,%/objs/*.o,$(TRGTDIRS)) $(patsubst %,%/$(APP),$(TRGTDIRS)) $(patsubst %,%/$(CPX),$(TRGTDIRS))
	@-rm -f $(DEPEND) $(foreach DIR,$(TRGTDIRS), $(DIR)/mapfile)
	@-rm -f $(DEPEND) $(foreach DIR,$(TRGTDIRS), $(DIR)/cpxhead)

.PHONY: printvars
printvars:
	@$(foreach V,$(.VARIABLES), $(if $(filter-out environment% default automatic, $(origin $V)),$(warning $V=$($V))))

.phony: $(DEPEND)

ifneq (clean,$(MAKECMDGOALS))
-include $(DEPEND)
endif
