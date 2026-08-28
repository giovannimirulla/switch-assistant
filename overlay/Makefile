TARGET := switch-ha-overlay
BUILD := build
SOURCES := source
INCLUDES := include
NO_ICON := 1
NO_NACP := 1

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

ARCH := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS := -g -Wall -O2 -ffunction-sections $(ARCH)
CFLAGS += -D__SWITCH__
# Temporary compatibility shim for the notification overlay. The current
# implementation issues raw IManagerDisplayService command 6000 calls to add
# the layer to multiple VI stacks. On newer HOS versions this appears to be
# unsafe and can crash am. Force-include a tiny shim that neutralizes only the
# raw serviceDispatchIn call used by vi_add_to_layer_stack() in main.c.
# Use TOPDIR because the recursive build runs with CURDIR=overlay/build.
CFLAGS += -include $(TOPDIR)/include/vi_compat.h
# Diagnostic shim: trace the first framebufferBegin/eventWait/framebufferEnd
# calls so we can identify the exact first-frame operation preceding the HOS
# 22.5.0 am crash. Logs to sdmc:/switch/switch-ha/overlay-frame.log.
CFLAGS += -include $(TOPDIR)/include/frame_diag.h
# Diagnostic shim: bypass only the hard-coded 25 second overlay graphics boot
# delay. Other sleeps remain untouched so post-init behavior stays unchanged.
CFLAGS += -include $(TOPDIR)/include/boot_delay_compat.h
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := -g $(ARCH)
LDFLAGS := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

PORTLIBS := $(DEVKITPRO)/portlibs/switch
LIBNX := $(DEVKITPRO)/libnx
LIBDIRS := $(PORTLIBS) $(LIBNX)
LIBS := -lnx

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR
export DEPSDIR := $(CURDIR)/$(BUILD)

export VPATH := $(CURDIR)/source
export OFILES := main.o

export INCLUDE := -I$(CURDIR)/include \
	$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
	-I$(CURDIR)/$(BUILD)

export CPPFLAGS := $(INCLUDE)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
export LD := $(CC)
export ARCH CPPFLAGS CFLAGS CXXFLAGS ASFLAGS LDFLAGS LIBS LD

.PHONY: all clean

all:
	@mkdir -p $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).ovl $(TARGET).nro $(TARGET).elf $(TARGET).lst

else

include $(DEVKITPRO)/libnx/switch_rules

DEPENDS := $(OFILES:.o=.d)

all: $(OUTPUT).ovl

$(OUTPUT).ovl: $(OUTPUT).elf
	@elf2nro $< $@
	@echo "built ... $(notdir $(OUTPUT).ovl)"

$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

endif
