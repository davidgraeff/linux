# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile

PHONY += menuconfig

menuconfig: $(obj)/mconf
	$< $(Kconfig)

# lxdialog stuff
check-lxdialog  := $(srctree)/$(src)/menuconfig/check-lxdialog.sh

# Use recursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
HOST_EXTRACFLAGS += $(shell $(CONFIG_SHELL) $(check-lxdialog) -ccflags) \
                    -DLOCALE

# ===========================================================================
# Shared Makefile for some of the kconfig executables:
# mconf:  Used for the menuconfig target
#         Utilizes the lxdialog package
# object files used by all kconfig flavours listed above

lxdialog := menuconfig/checklist.o menuconfig/util.o menuconfig/inputbox.o
lxdialog += menuconfig/textbox.o menuconfig/yesno.o menuconfig/menubox.o

mconf-objs     := menuconfig/mconf.o zconf.tab.o $(lxdialog)

ifeq ($(MAKECMDGOALS),menuconfig)
	hostprogs-y += mconf
endif

clean-files     += mconf

# Check that we have the required ncurses stuff installed for lxdialog (menuconfig)
HOSTLOADLIBES_mconf   = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ldflags $(HOSTCC))
PHONY += $(obj)/dochecklxdialog
$(addprefix $(obj)/,$(lxdialog)): $(obj)/dochecklxdialog
$(obj)/dochecklxdialog:
	$(Q)$(CONFIG_SHELL) $(check-lxdialog) -check $(HOSTCC) $(HOST_EXTRACFLAGS) $(HOSTLOADLIBES_mconf)

always := dochecklxdialog
