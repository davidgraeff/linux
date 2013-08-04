/*
 *
 * Copyright (C) 2013 David Gräff <david.graeff@web.de>
 * Copyright (C) 2002-2003 Romain Lievin <roms@tilp.info>
 * Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef GCONF_H_
#define GCONF_H_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>

#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdk.h>

#include "../lkc.h"

//#define DEBUG

extern enum view_mode_enum {
	SINGLE_VIEW, SPLIT_VIEW, FULL_VIEW
} view_mode;

enum {
	OPT_NORMAL, OPT_ALL, OPT_PROMPT
};
enum {
	COL_OPTION, COL_NAME,
	COL_NO, COL_MOD, COL_YES,
	COL_VALUE,
	COL_MENU, COL_COLOR, COL_EDIT,
	COL_PIXBUF, COL_PIXVIS,
	COL_BTNVISIBLE, COL_BTNACT,
	COL_BTNINC, COL_BTNRAD,
	COL_NO_EN, COL_MOD_EN, COL_YES_EN,
	COL_NUMBER
};

extern struct gconfwindow_s {
	// widget pointer variables
	GtkWidget *main_wnd;
	GtkWidget *tree1_w;	// left  frame
	GtkWidget *tree2_w;	// right frame
	GtkWidget *text_w;
	GtkWidget *hpaned;
	GtkWidget *back_btn;
	GtkWidget *save_btn;
	GtkWidget *save_menu_item;
	// option variables
	gboolean show_name;
	gboolean show_range;
	gboolean show_value;
	gboolean resizeable;
	int opt_mode;

} gconfwindow;

void on_treeview2_makeDisable_clicked(GtkCellRendererToggle *cell, gchar *path_str,
               gpointer data);
void on_treeview2_makeModule_clicked(GtkCellRendererToggle *cell, gchar *path_str,
               gpointer data);
void on_treeview2_makeEnable_clicked(GtkCellRendererToggle *cell, gchar *path_str,
               gpointer data);
void on_treeview1_checkbox_toggle(GtkCellRendererToggle *cell, gchar *path_str,
               gpointer data);
void on_treeview2_toggleEnable_clicked(GtkCellRendererToggle *cell, gchar *path_str,
               gpointer data);
#endif