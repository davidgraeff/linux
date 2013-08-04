# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile
# This specific makefile only contains special targets and targets for
# the line-oriented program. Other interface flavours are included at the end.

PHONY += oldconfig config silentoldconfig update-po-config \
	localmodconfig localyesconfig

ifdef KBUILD_KCONFIG
Kconfig := $(KBUILD_KCONFIG)
else
Kconfig := Kconfig
endif

# We need this, in case the user has it in its environment
unexport CONFIG_

config: $(obj)/conf
	$< --oldaskconfig $(Kconfig)

oldconfig: $(obj)/conf
	$< --$@ $(Kconfig)

silentoldconfig: $(obj)/conf
	$(Q)mkdir -p include/config include/generated
	$< --$@ $(Kconfig)

localyesconfig localmodconfig: $(obj)/streamline_config.pl $(obj)/conf
	$(Q)mkdir -p include/config include/generated
	$(Q)perl $< --$@ $(srctree) $(Kconfig) > .tmp.config
	$(Q)if [ -f .config ]; then 					\
			cmp -s .tmp.config .config ||			\
			(mv -f .config .config.old.1;			\
			 mv -f .tmp.config .config;			\
			 $(obj)/conf --silentoldconfig $(Kconfig);	\
			 mv -f .config.old.1 .config.old)		\
	else								\
			mv -f .tmp.config .config;			\
			$(obj)/conf --silentoldconfig $(Kconfig);	\
	fi
	$(Q)rm -f .tmp.config

# Create new linux.pot file
# Adjust charset to UTF-8 in .po file to accept UTF-8 in Kconfig files
update-po-config: $(obj)/kxgettext $(obj)/gkonfig/gconf.glade.h
	$(Q)echo "  GEN     config.pot"
	$(Q)xgettext --default-domain=linux                         \
	    --add-comments --keyword=_ --keyword=N_                 \
	    --from-code=UTF-8                                       \
	    --files-from=$(srctree)/scripts/kconfig/POTFILES.in     \
	    --directory=$(srctree) --directory=$(objtree)           \
	    --output $(obj)/config.pot
	$(Q)sed -i s/CHARSET/UTF-8/ $(obj)/config.pot
	$(Q)(for i in `ls $(srctree)/arch/*/Kconfig      \
	    $(srctree)/arch/*/um/Kconfig`;               \
	    do                                           \
		echo "  GEN     $$i";                    \
		$(obj)/kxgettext $$i                     \
		     >> $(obj)/config.pot;               \
	    done )
	$(Q)echo "  GEN     linux.pot"
	$(Q)msguniq --sort-by-file --to-code=UTF-8 $(obj)/config.pot \
	    --output $(obj)/linux.pot
	$(Q)rm -f $(obj)/config.pot

PHONY += allnoconfig allyesconfig allmodconfig alldefconfig randconfig

allnoconfig allyesconfig allmodconfig alldefconfig randconfig: $(obj)/conf
	$< --$@ $(Kconfig)

PHONY += listnewconfig olddefconfig oldnoconfig savedefconfig defconfig

listnewconfig olddefconfig: $(obj)/conf
	$< --$@ $(Kconfig)

# oldnoconfig is an alias of olddefconfig, because people already are dependent
# on its behavior(sets new symbols to their default value but not 'n') with the
# counter-intuitive name.
oldnoconfig: $(obj)/conf
	$< --olddefconfig $(Kconfig)

savedefconfig: $(obj)/conf
	$< --$@=defconfig $(Kconfig)

defconfig: $(obj)/conf
ifeq ($(KBUILD_DEFCONFIG),)
	$< --defconfig $(Kconfig)
else
	@echo "*** Default configuration is based on '$(KBUILD_DEFCONFIG)'"
	$(Q)$< --defconfig=arch/$(SRCARCH)/configs/$(KBUILD_DEFCONFIG) $(Kconfig)
endif

%_defconfig: $(obj)/conf
	$(Q)$< --defconfig=arch/$(SRCARCH)/configs/$@ $(Kconfig)

# Help text used by make help
help:
	@echo  '  config	  - Update current config utilising a line-oriented program'
	@echo  '  nconfig         - Update current config utilising a ncurses menu based program'
	@echo  '  menuconfig	  - Update current config utilising a menu based program'
	@echo  '  xconfig	  - Update current config utilising a QT based front-end'
	@echo  '  qconfig	  - Update current config utilising a QT based front-end'
	@echo  '  gconfig	  - Update current config utilising a GTK based front-end'
	@echo  '  oldconfig	  - Update current config utilising a provided .config as base'
	@echo  '  localmodconfig  - Update current config disabling modules not loaded'
	@echo  '  localyesconfig  - Update current config converting local mods to core'
	@echo  '  silentoldconfig - Same as oldconfig, but quietly, additionally update deps'
	@echo  '  defconfig	  - New config with default from ARCH supplied defconfig'
	@echo  '  savedefconfig   - Save current config as ./defconfig (minimal config)'
	@echo  '  allnoconfig	  - New config where all options are answered with no'
	@echo  '  allyesconfig	  - New config where all options are accepted with yes'
	@echo  '  allmodconfig	  - New config selecting modules when possible'
	@echo  '  alldefconfig    - New config with all symbols set to default'
	@echo  '  randconfig	  - New config with random answer to all options'
	@echo  '  listnewconfig   - List new options'
	@echo  '  olddefconfig	  - Same as silentoldconfig but sets new symbols to their default value'

# Use recursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
HOST_EXTRACFLAGS += $(shell $(CONFIG_SHELL) $(check-lxdialog) -ccflags) \
                    -DLOCALE

# ===========================================================================
# Shared Makefile for some of the kconfig executables:
# conf:	  Used for defconfig, oldconfig and related targets
# object files used by all kconfig flavours

conf-objs	:= conf.o  zconf.tab.o
kxgettext-objs	:= kxgettext.o zconf.tab.o

hostprogs-y := conf

ifeq ($(MAKECMDGOALS),update-po-config)
	hostprogs-y += kxgettext
endif

clean-files	:= zconf.tab.c zconf.lex.c zconf.hash.c
clean-files     += config.pot linux.pot

# Add environment specific flags
HOST_EXTRACFLAGS += $(shell $(CONFIG_SHELL) $(srctree)/$(src)/check.sh $(HOSTCC) $(HOSTCFLAGS))

# generated files seem to need this to find local include files
HOSTCFLAGS_zconf.lex.o	:= -I$(src)
HOSTCFLAGS_zconf.tab.o	:= -I$(src)

LEX_PREFIX_zconf	:= zconf
YACC_PREFIX_zconf	:= zconf

$(obj)/zconf.tab.o: $(obj)/zconf.lex.c $(obj)/zconf.hash.c

# NCurses/Dialog Interface
include $(obj)/menuconfig/Makefile

# Qt Interface
include $(obj)/qconfig/Makefile

# NCurses Interface
include $(obj)/nconfig/Makefile

# Gtk Interface
include $(obj)/gconfig/Makefile