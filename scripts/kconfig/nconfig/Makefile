# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile

PHONY += nconfig

nconfig: $(obj)/nconf
	$< $(Kconfig)

# ===========================================================================
# nconf:  Used for the nconfig target.
#         Utilizes ncurses
# object files used by all kconfig flavours listed above

nconf-objs     := nconfig/nconf.o zconf.tab.o nconfig/nconf.gui.o

ifeq ($(MAKECMDGOALS),nconfig)
	hostprogs-y += nconf
endif

clean-files     += nconf

HOSTLOADLIBES_nconf	= $(shell \
				pkg-config --libs menu panel ncurses 2>/dev/null \
				|| echo "-lmenu -lpanel -lncurses"  )

