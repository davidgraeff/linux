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

gconf-objs	:= gconfig/gconf.o gconfig/gconf_treeview_model.o zconf.tab.o

ifeq ($(MAKECMDGOALS),gconfig)
	gconf-target := 1
endif

ifeq ($(gconf-target),1)
	hostprogs-y += gconf
endif

clean-files	+= gconfig/gconf.glade.h gconf

HOSTLOADLIBES_gconf	= `pkg-config --libs gtk+-3.0 gmodule-2.0`
HOSTCFLAGS_gconf.o	= `pkg-config --cflags gtk+-3.0 gmodule-2.0` \
                          -Wno-missing-prototypes -DGTK_DISABLE_SINGLE_INCLUDES \
                          -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED \
                          -DGSEAL_ENABLE -std=c99
HOSTCFLAGS_gconf_treeview_model.o	= `pkg-config --cflags gtk+-3.0 gmodule-2.0` \
                          -Wno-missing-prototypes -DGTK_DISABLE_SINGLE_INCLUDES \
                          -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED \
                          -DGSEAL_ENABLE -std=c99

$(obj)/gconfig/gconf.o: $(obj)/gconfig/.tmp_gtkcheck

ifeq ($(gconf-target),1)
-include $(obj)/.tmp_gtkcheck

# GTK needs some extra effort, too...
$(obj)/gconfig/.tmp_gtkcheck:
	@if `pkg-config --exists gtk+-3.0`; then		\
		if `pkg-config --atleast-version=3.0.0 gtk+-3.0`; then			\
			touch $@;								\
		else									\
			echo >&2 "*"; 							\
			echo >&2 "* GTK+ is present but version >= 3.0.0 is required.";	\
			echo >&2 "*";							\
			false;								\
		fi									\
	else										\
		echo >&2 "*"; 								\
		echo >&2 "* Unable to find the GTK+ installation. Please make sure that"; 	\
		echo >&2 "* the GTK+ 3.0 development package is correctly installed..."; 	\
		echo >&2 "* You need gtk+-3.0 and glib-2.0."; 		\
		echo >&2 "*"; 								\
		false;									\
	fi
endif

# Extract gconf menu items for I18N support
$(obj)/gconfig/gconf.glade.h: $(obj)/gconfig/gconf.glade
	$(Q)intltool-extract --type=gettext/glade --srcdir=$(srctree) \
	$(obj)/gconfig/gconf.glade
