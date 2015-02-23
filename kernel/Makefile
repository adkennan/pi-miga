# Makefile - build script */
 
# build environment
PREFIX ?= /home/andrew/dev/pi/tc/arm-2008q3
ARMGNU ?= $(PREFIX)/bin/arm-none-eabi
 
# source files
SOURCES_ASM := $(wildcard src/*.S)
SOURCES_C   := $(wildcard src/*.c)
 
# object files
OBJS        := $(patsubst %.S,%.o,$(SOURCES_ASM))
OBJS        += $(patsubst %.c,%.o,$(SOURCES_C))
DEPS		:= $(patsubst %.S,%.d,$(SOURCES_ASM))
DEPS		+= $(patsubst %.c,%.d,$(SOURCES_C))

# Build flags
DEPENDFLAGS := -MD -MP
INCLUDES    := -I include -I../include
BASEFLAGS   := -O2 -fpic -pedantic -pedantic-errors -nostdlib
BASEFLAGS   += -nostartfiles -ffreestanding -nodefaultlibs
BASEFLAGS   += -fno-builtin -fomit-frame-pointer -mcpu=arm1176jzf-s
WARNFLAGS   := -Wall 
#-Wextra -Wshadow -Wcast-align -Wwrite-strings
#WARNFLAGS   += -Wredundant-decls -Winline
#WARNFLAGS   += -Wno-attributes -Wno-deprecated-declarations
#WARNFLAGS   += -Wno-div-by-zero -Wno-endif-labels -Wfloat-equal
#WARNFLAGS   += -Wformat=2 -Wno-format-extra-args -Winit-self
#WARNFLAGS   += -Winvalid-pch -Wmissing-format-attribute
#WARNFLAGS   += -Wmissing-include-dirs -Wno-multichar
#WARNFLAGS   += -Wredundant-decls -Wshadow
#WARNFLAGS   += -Wno-sign-compare -Wswitch -Wsystem-headers -Wundef
#WARNFLAGS   += -Wno-pragmas 
#WARNFLAGS   += -Wwrite-strings -Wdisabled-optimization -Wpointer-arith
#WARNFLAGS   += -Werror
ASFLAGS     := $(INCLUDES) $(DEPENDFLAGS) -D__ASSEMBLY__
CFLAGS      := $(INCLUDES) $(DEPENDFLAGS) $(BASEFLAGS) $(WARNFLAGS)
CFLAGS      += -std=c99 -D__have_long64 -D__int_fast64_t_defined
 
# build rules
all: kernel.img
 
include $(wildcard src/*.d)
 
kernel.elf: $(OBJS) link-arm-eabi.ld
	$(ARMGNU)-gcc $(OBJS) -Tlink-arm-eabi.ld -o $@
 
kernel.img: kernel.elf
	$(ARMGNU)-objcopy kernel.elf -O binary kernel.img
 
clean:
	$(RM) -f $(OBJS) $(DEPS) kernel.elf kernel.img
 
dist-clean: clean
	$(RM) -f *.d
 
# C.
%.o: %.c Makefile
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@
 
# AS.
%.o: %.S Makefile
	$(ARMGNU)-g++ $(ASFLAGS) -c $< -o $@