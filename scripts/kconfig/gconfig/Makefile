# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile

PHONY += gconfig

gconfig: $(obj)/gconf
	$< $(Kconfig)

# ===========================================================================
# Shared Makefile for some of the kconfig executables:
# gconf:  Used for the gconfig target
#         Based on GTK which needs to be installed to compile it
# object files used by all kconfig flavours listed above

gconf-objs	:= gconfig/gconf.o zconf.tab.o

ifeq ($(MAKECMDGOALS),gconfig)
	gconf-target := 1
endif

ifeq ($(gconf-target),1)
	hostprogs-y += gconf
endif

clean-files	+= gconfig/gconf.glade.h gconf


HOSTLOADLIBES_gconf	= `pkg-config --libs gtk+-2.0 gmodule-2.0 libglade-2.0`
HOSTCFLAGS_gconf.o	= `pkg-config --cflags gtk+-2.0 gmodule-2.0 libglade-2.0` \
                          -Wno-missing-prototypes

$(obj)/gconfig/gconf.o: $(obj)/gconfig/.tmp_gtkcheck

ifeq ($(gconf-target),1)
-include $(obj)/.tmp_gtkcheck

# GTK needs some extra effort, too...
$(obj)/gconfig/.tmp_gtkcheck:
	@if `pkg-config --exists gtk+-2.0 gmodule-2.0 libglade-2.0`; then		\
		if `pkg-config --atleast-version=2.0.0 gtk+-2.0`; then			\
			touch $@;								\
		else									\
			echo >&2 "*"; 							\
			echo >&2 "* GTK+ is present but version >= 2.0.0 is required.";	\
			echo >&2 "*";							\
			false;								\
		fi									\
	else										\
		echo >&2 "*"; 								\
		echo >&2 "* Unable to find the GTK+ installation. Please make sure that"; 	\
		echo >&2 "* the GTK+ 2.0 development package is correctly installed..."; 	\
		echo >&2 "* You need gtk+-2.0, glib-2.0 and libglade-2.0."; 		\
		echo >&2 "*"; 								\
		false;									\
	fi
endif

# Extract gconf menu items for I18N support
$(obj)/gconfig/gconf.glade.h: $(obj)/gconfig/gconf.glade
	$(Q)intltool-extract --type=gettext/glade --srcdir=$(srctree) \
	$(obj)/gconfig/gconf.glade
