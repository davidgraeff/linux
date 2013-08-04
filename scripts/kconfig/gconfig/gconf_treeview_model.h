/*
 *
 * Copyright (C) 2013 David Gräff <david.graeff@web.de>
 * Copyright (C) 2002-2003 Romain Lievin <roms@tilp.info>
 * Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef GCONF_TREEVIEW_MODEL_H_
#define GCONF_TREEVIEW_MODEL_H_

#include "gconf.h"

// tree and model init
void init_right_tree(void);
void init_left_tree(void);

void renderer_edited(GtkCellRendererText * cell,
			    const gchar * path_string,
			    const gchar * new_text, gpointer user_data);

// model update operations
void change_sym_value(struct menu *menu, gint col, GtkTreeStore* tree);
void toggle_sym_value(struct menu *menu, GtkTreeStore* tree);

// node operations
void init_node_parents(void);
void set_node(GtkTreeIter * node, struct menu *menu, gchar ** row, GtkTreeStore* tree);
void place_node(struct menu *menu, char **row, GtkTreeStore* tree);
GtkTreeIter *gtktree_iter_find_node(GtkTreeIter * parent,
				    struct menu *tofind);
gchar **fill_row(struct menu *menu);

// tree operations
void update_tree(struct menu *src, GtkTreeIter * dst, GtkTreeStore* tree);
void display_tree(struct menu *menu, GtkTreeStore* tree);
void display_tree_part(struct menu *menu);
void display_list(void);
void fixup_rootmenu(struct menu *menu);

struct menu *getCurrentMenu(void);

#endif
