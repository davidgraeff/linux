/*
 *
 * Copyright (C) 2013 David Gräff <david.graeff@web.de>
 * Copyright (C) 2002-2003 Romain Lievin <roms@tilp.info>
 * Released under the terms of the GNU GPL v2.0.
 *
 */

#include "gconf_treeview_model.h"
#define IMAGES_TREEVIEW
#include "../images.c"

static GtkTreeIter *parents[256];
static gint indent;


void init_left_tree(void)
{
	GtkTreeView *view = GTK_TREE_VIEW(gconfwindow.tree1_w);
	GtkCellRenderer *renderer;
	GtkTreeSelection *sel;
	GtkTreeViewColumn *column;

	GtkTreeStore *tree1;
	GtkTreeModel *model1;
	tree1 = gtk_tree_store_new(COL_NUMBER,
				G_TYPE_STRING, G_TYPE_STRING, //Option, Name
				G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, // COL_NO, COL_MOD, COL_YES
				G_TYPE_STRING, // COL_VALUE
				G_TYPE_POINTER, GDK_TYPE_COLOR, G_TYPE_BOOLEAN, // COL_MENU, COL_COLOR, COL_EDIT
				GDK_TYPE_PIXBUF, G_TYPE_BOOLEAN, // COL_PIXBUF, COL_PIXVIS
				G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, //COL_BTNVISIBLE, COL_BTNACT
				G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, // COL_BTNINC, COL_BTNRAD,
				G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN // COL_NO_EN, COL_MOD_EN, COL_YES_EN
				);
	model1 = GTK_TREE_MODEL(tree1);
	
	gtk_tree_view_set_model(view, model1);
	gtk_tree_view_set_headers_visible(view, TRUE);
	gtk_tree_view_set_rules_hint(view, TRUE);

	column = gtk_tree_view_column_new();
	gtk_tree_view_append_column(view, column);
	gtk_tree_view_column_set_title(column, _("Options"));

	renderer = gtk_cell_renderer_toggle_new();
	g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK
		(on_treeview1_checkbox_toggle), NULL);
	gtk_tree_view_column_pack_start(GTK_TREE_VIEW_COLUMN(column),
					renderer, FALSE);
	gtk_tree_view_column_set_attributes(GTK_TREE_VIEW_COLUMN(column),
					    renderer,
					    "active", COL_BTNACT,
					    "inconsistent", COL_BTNINC,
					    "visible", COL_BTNVISIBLE,
					    "radio", COL_BTNRAD, NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(GTK_TREE_VIEW_COLUMN(column),
					renderer, FALSE);
	gtk_tree_view_column_set_attributes(GTK_TREE_VIEW_COLUMN(column),
					    renderer,
					    "text", COL_OPTION,
					    "foreground-gdk",
					    COL_COLOR, NULL);

	sel = gtk_tree_view_get_selection(view);
	gtk_tree_selection_set_mode(sel, GTK_SELECTION_SINGLE);
	gtk_widget_realize(gconfwindow.tree1_w);
}

void init_right_tree(void)
{
	GtkTreeView *view = GTK_TREE_VIEW(gconfwindow.tree2_w);
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	
	GtkTreeStore *tree2;
	GtkTreeModel *model2;
	tree2 = gtk_tree_store_new(COL_NUMBER,
				G_TYPE_STRING, G_TYPE_STRING, //Option, Name
				G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, // COL_NO, COL_MOD, COL_YES
				G_TYPE_STRING, // COL_VALUE
				G_TYPE_POINTER, GDK_TYPE_COLOR, G_TYPE_BOOLEAN, // COL_MENU, COL_COLOR, COL_EDIT
				GDK_TYPE_PIXBUF, G_TYPE_BOOLEAN, // COL_PIXBUF, COL_PIXVIS
				G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, //COL_BTNVISIBLE, COL_BTNACT
				G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, // COL_BTNINC, COL_BTNRAD,
				G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN // COL_NO_EN, COL_MOD_EN, COL_YES_EN
				);
	model2 = GTK_TREE_MODEL(tree2);

	gtk_tree_view_set_model(view, model2);
	gtk_tree_view_set_headers_visible(view, TRUE);
	gtk_tree_view_set_rules_hint(view, TRUE);

	// The option name column with the expand icon and enabled checkbox
	column = gtk_tree_view_column_new();
	gtk_tree_view_append_column(view, column);
	gtk_tree_view_column_set_title(column, _("Options"));

	// The expand icon
	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(GTK_TREE_VIEW_COLUMN(column),
					renderer, FALSE);
	gtk_tree_view_column_set_attributes(GTK_TREE_VIEW_COLUMN(column),
					    renderer,
					    "pixbuf", COL_PIXBUF,
					    "visible", COL_PIXVIS, NULL);
	// The enabled/module/disabled checkbox
	renderer = gtk_cell_renderer_toggle_new();
	gtk_tree_view_column_pack_start(GTK_TREE_VIEW_COLUMN(column),
					renderer, FALSE);
	gtk_tree_view_column_set_attributes(GTK_TREE_VIEW_COLUMN(column),
					    renderer,
					    "active", COL_BTNACT,
					    "inconsistent", COL_BTNINC,
					    "visible", COL_BTNVISIBLE,
					    "radio", COL_BTNRAD, NULL);
	g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK
		(on_treeview2_toggleEnable_clicked), NULL);
	// The options name
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(GTK_TREE_VIEW_COLUMN(column),
					renderer, FALSE);
	gtk_tree_view_column_set_attributes(GTK_TREE_VIEW_COLUMN(column),
					    renderer,
					    "text", COL_OPTION,
					    "foreground-gdk",
					    COL_COLOR, NULL);

	// The internal name column
	// The internal name
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(view, -1,
						    _("Name"), renderer,
						    "text", COL_NAME,
						    "foreground-gdk",
						    COL_COLOR, NULL);
	column = gtk_tree_view_get_column(view, COL_NAME);
	gtk_tree_view_column_set_visible(column, gconfwindow.show_name);
	
	// Range column (Disable)
	renderer = gtk_cell_renderer_toggle_new();
	gtk_tree_view_insert_column_with_attributes(view, -1,
						    "N", renderer,
							"active", COL_NO,
							"visible", COL_NO_EN,
							NULL);
	column = gtk_tree_view_get_column(view, COL_NO);
	gtk_tree_view_column_set_visible(column, gconfwindow.show_range);
	gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer), TRUE);
	g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK
		(on_treeview2_makeDisable_clicked), NULL);
	
	// Range column (Module)
	renderer = gtk_cell_renderer_toggle_new();
	gtk_tree_view_insert_column_with_attributes(view, -1,
						    "M", renderer,
							"active", COL_MOD,
							"visible", COL_MOD_EN,
							NULL);
	column = gtk_tree_view_get_column(view, COL_MOD);
	gtk_tree_view_column_set_visible(column, gconfwindow.show_range);
	gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer), TRUE);
	g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK
		(on_treeview2_makeModule_clicked), NULL);
	
	// Range column (Enable)
	renderer = gtk_cell_renderer_toggle_new();
	gtk_tree_view_insert_column_with_attributes(view, -1,
						    "Y", renderer,
							"active", COL_YES,
							"visible", COL_YES_EN,
							NULL);
	column = gtk_tree_view_get_column(view, COL_YES);
	gtk_tree_view_column_set_visible(column, gconfwindow.show_range);
	gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer), TRUE);
	g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK
		(on_treeview2_makeEnable_clicked), NULL);
	
	// Current value column
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(view, -1,
						    _("Value"), renderer,
						    "text", COL_VALUE,
						    "editable",
						    COL_EDIT,
						    "foreground-gdk",
						    COL_COLOR, NULL);
	column = gtk_tree_view_get_column(view, COL_VALUE);
	gtk_tree_view_column_set_visible(column, gconfwindow.show_value);
	g_signal_connect(G_OBJECT(renderer), "edited",
			 G_CALLBACK(renderer_edited), NULL);

	if (gconfwindow.resizeable) {
		for (int i = 0; i < COL_VALUE; i++) {
			column = gtk_tree_view_get_column(view, i);
			gtk_tree_view_column_set_resizable(column, TRUE);
		}
	}

	GtkTreeSelection *sel = gtk_tree_view_get_selection(view);
	gtk_tree_selection_set_mode(sel, GTK_SELECTION_SINGLE);
}

/* CTree Callbacks */

/* Change hex/int/string value in the cell */
void renderer_edited(GtkCellRendererText * cell,
			    const gchar * path_string,
			    const gchar * new_text, gpointer user_data)
{
	GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
	GtkTreeIter iter;
	const char *old_def, *new_def;
	struct menu *menu;
	struct symbol *sym;
	
	GtkTreeModel* model2 = gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w));
	GtkTreeStore* tree2 = GTK_TREE_STORE(model2);

	if (!gtk_tree_model_get_iter(model2, &iter, path))
		return;

	gtk_tree_model_get(model2, &iter, COL_MENU, &menu, -1);
	sym = menu->sym;

	gtk_tree_model_get(model2, &iter, COL_VALUE, &old_def, -1);
	new_def = new_text;

	sym_set_string_value(sym, new_def);

	update_tree(&rootmenu, NULL, tree2);

	gtk_tree_path_free(path);
}

struct menu *currentMenu = 0; // currentMenu node

struct menu *getCurrentMenu(void) {
	return currentMenu;
}

/* Change the value of a symbol and update the tree */
void change_sym_value(struct menu *menu, gint col, GtkTreeStore* tree)
{
	struct symbol *sym = menu->sym;
	tristate newval;

	if (!sym)
		return;

	if (col == COL_NO)
		newval = no;
	else if (col == COL_MOD)
		newval = mod;
	else if (col == COL_YES)
		newval = yes;
	else
		return;

	switch (sym_get_type(sym)) {
	case S_BOOLEAN:
	case S_TRISTATE:
		if (!sym_tristate_within_range(sym, newval))
			newval = yes;
		sym_set_tristate_value(sym, newval);
		if (view_mode == FULL_VIEW)
			update_tree(&rootmenu, NULL, tree);
		else if (view_mode == SPLIT_VIEW) {
			update_tree(currentMenu, NULL, tree);
			display_list();
		}
		else if (view_mode == SINGLE_VIEW)
			display_tree_part(currentMenu);	//fixme: keep exp/coll
		break;
	case S_INT:
	case S_HEX:
	case S_STRING:
	default:
		break;
	}
}

void toggle_sym_value(struct menu *menu, GtkTreeStore* tree)
{
	if (!menu->sym)
		return;

	sym_toggle_tristate_value(menu->sym);
	if (view_mode == FULL_VIEW)
		update_tree(&rootmenu, NULL, tree);
	else if (view_mode == SPLIT_VIEW) {
		update_tree(currentMenu, NULL, tree);
		display_list();
	}
	else if (view_mode == SINGLE_VIEW)
		display_tree_part(currentMenu);	//fixme: keep exp/coll
}


/* Fill a row of strings */
gchar **fill_row(struct menu *menu)
{
	static gchar *row[COL_NUMBER];
	struct symbol *sym = menu->sym;
	const char *def;
	int stype;
	tristate val;
	enum prop_type ptype;

	g_free(row[COL_OPTION]);
	g_free(row[COL_NAME]);
	g_free(row[COL_VALUE]);
	g_free(row[COL_MENU]);
	g_free(row[COL_COLOR]);
	
	bzero(row, sizeof(row));

	row[COL_OPTION] =
	    g_strdup_printf("%s %s", _(menu_get_prompt(menu)),
			    sym && !sym_has_value(sym) ? "(NEW)" : "");

	if (gconfwindow.opt_mode == OPT_ALL && !menu_is_visible(menu))
		row[COL_COLOR] = g_strdup("DarkGray");
	else if (gconfwindow.opt_mode == OPT_PROMPT &&
			menu_has_prompt(menu) && !menu_is_visible(menu))
		row[COL_COLOR] = g_strdup("DarkGray");
	else
		row[COL_COLOR] = g_strdup("Black");

	ptype = menu->prompt ? menu->prompt->type : P_UNKNOWN;
	switch (ptype) {
	case P_MENU:
		row[COL_PIXBUF] = (gchar *) xpm_menu;
		if (view_mode == SINGLE_VIEW)
			row[COL_PIXVIS] = GINT_TO_POINTER(TRUE);
		row[COL_BTNVISIBLE] = GINT_TO_POINTER(FALSE);
		break;
	case P_COMMENT:
		row[COL_PIXBUF] = (gchar *) xpm_void;
		row[COL_PIXVIS] = GINT_TO_POINTER(FALSE);
		row[COL_BTNVISIBLE] = GINT_TO_POINTER(FALSE);
		break;
	default:
		row[COL_PIXBUF] = (gchar *) xpm_void;
		row[COL_PIXVIS] = GINT_TO_POINTER(FALSE);
		row[COL_BTNVISIBLE] = GINT_TO_POINTER(TRUE);
		break;
	}

	if (!sym)
		return row;
	row[COL_NAME] = g_strdup(sym->name);

	sym_calc_value(sym);
	sym->flags &= ~SYMBOL_CHANGED;

	if (sym_is_choice(sym)) {	// parse childs for getting final value
		struct menu *child;
		struct symbol *def_sym = sym_get_choice_value(sym);
		struct menu *def_menu = NULL;

		row[COL_BTNVISIBLE] = GINT_TO_POINTER(FALSE);

		for (child = menu->list; child; child = child->next) {
			if (menu_is_visible(child)
			    && child->sym == def_sym)
				def_menu = child;
		}

		if (def_menu)
			row[COL_VALUE] =
			    g_strdup(_(menu_get_prompt(def_menu)));
	}
	if (sym->flags & SYMBOL_CHOICEVAL)
		row[COL_BTNRAD] = GINT_TO_POINTER(TRUE);

	stype = sym_get_type(sym);
	switch (stype) {
	case S_BOOLEAN:
		if (GPOINTER_TO_INT(row[COL_PIXVIS]) == FALSE)
			row[COL_BTNVISIBLE] = GINT_TO_POINTER(TRUE);
		if (sym_is_choice(sym))
			break;
		/* fall through */
	case S_TRISTATE:
		val = sym_get_tristate_value(sym);
		switch (val) {
		case no:
			row[COL_NO] = GINT_TO_POINTER(TRUE);
			row[COL_VALUE] = g_strdup("[No]");
			row[COL_BTNACT] = GINT_TO_POINTER(FALSE);
			row[COL_BTNINC] = GINT_TO_POINTER(FALSE);
			break;
		case mod:
			row[COL_MOD] = GINT_TO_POINTER(TRUE);
			row[COL_VALUE] = g_strdup("[Module]");
			row[COL_BTNINC] = GINT_TO_POINTER(TRUE);
			break;
		case yes:
			row[COL_YES] = GINT_TO_POINTER(TRUE);
			row[COL_VALUE] = g_strdup("[Yes]");
			row[COL_BTNACT] = GINT_TO_POINTER(TRUE);
			row[COL_BTNINC] = GINT_TO_POINTER(FALSE);
			break;
		}

		row[COL_NO_EN] = GINT_TO_POINTER(sym_tristate_within_range(sym, no)); // No is allowed
		row[COL_MOD_EN] = GINT_TO_POINTER(sym_tristate_within_range(sym, mod)); // Mod is allowed
		row[COL_YES_EN] = GINT_TO_POINTER(sym_tristate_within_range(sym, yes)); // Yes is allowed
		break;
	case S_INT:
	case S_HEX:
	case S_STRING:
		def = sym_get_string_value(sym);
		row[COL_VALUE] = g_strdup(def);
		row[COL_EDIT] = GINT_TO_POINTER(TRUE);
		row[COL_BTNVISIBLE] = GINT_TO_POINTER(FALSE);
		break;
	}

	return row;
}


/* Set the node content with a row of strings */
void set_node(GtkTreeIter * node, struct menu *menu, gchar ** row, GtkTreeStore* tree)
{
	GdkColor color;
	GdkPixbuf *pix;

	pix = gdk_pixbuf_new_from_xpm_data((const char **)
					   row[COL_PIXBUF]);

 	gdk_color_parse(row[COL_COLOR], &color);
// 	gdk_colormap_alloc_colors(gdk_colormap_get_system(), &color, 1,
// 				  FALSE, FALSE, &success);

	gtk_tree_store_set(tree, node,
			   COL_OPTION, row[COL_OPTION],
			   COL_NAME, row[COL_NAME],
			   COL_NO, row[COL_NO],
			   COL_MOD, row[COL_MOD],
			   COL_YES, row[COL_YES],
			   COL_VALUE, row[COL_VALUE],
			   COL_MENU, (gpointer) menu,
			   COL_COLOR, &color,
			   COL_EDIT, GPOINTER_TO_INT(row[COL_EDIT]),
			   COL_PIXBUF, pix,
			   COL_PIXVIS, GPOINTER_TO_INT(row[COL_PIXVIS]),
			   COL_BTNVISIBLE, GPOINTER_TO_INT(row[COL_BTNVISIBLE]),
			   COL_BTNACT, GPOINTER_TO_INT(row[COL_BTNACT]),
			   COL_BTNINC, GPOINTER_TO_INT(row[COL_BTNINC]),
			   COL_BTNRAD, GPOINTER_TO_INT(row[COL_BTNRAD]),
			   COL_NO_EN, GPOINTER_TO_INT(row[COL_NO_EN]),
			   COL_MOD_EN, GPOINTER_TO_INT(row[COL_MOD_EN]),
			   COL_YES_EN, GPOINTER_TO_INT(row[COL_YES_EN]),
			   -1);

	g_object_unref(pix);
}

void init_node_parents(void)
{
	/* Prepare GtkTreeIter */
	gint i;
	for (parents[0] = NULL, i = 1; i < 256; i++)
		parents[i] = (GtkTreeIter *) g_malloc(sizeof(GtkTreeIter));
	
}

/* Add a node to the tree */
void place_node(struct menu *menu, char **row, GtkTreeStore* tree)
{
	GtkTreeIter *parent = parents[indent - 1];
	GtkTreeIter *node = parents[indent];

	gtk_tree_store_append(tree, node, parent);
	set_node(node, menu, row, tree);
}

/*
 * Find a menu in the GtkTree starting at parent.
 */
GtkTreeIter *gtktree_iter_find_node(GtkTreeIter * parent,
				    struct menu *tofind)
{
	static GtkTreeIter found;
	GtkTreeIter iter;
	GtkTreeIter *child = &iter;
	gboolean valid;
	GtkTreeIter *ret;

	GtkTreeModel* model2 = gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w));
	valid = gtk_tree_model_iter_children(model2, child, parent);
	while (valid) {
		struct menu *menu;

		gtk_tree_model_get(model2, child, 6, &menu, -1);

		if (menu == tofind) {
			memcpy(&found, child, sizeof(GtkTreeIter));
			return &found;
		}

		ret = gtktree_iter_find_node(child, tofind);
		if (ret)
			return ret;

		valid = gtk_tree_model_iter_next(model2, child);
	}

	return NULL;
}


/*
 * Update the tree by adding/removing entries
 * Does not change other nodes
 */
void update_tree(struct menu *src, GtkTreeIter * dst, GtkTreeStore* tree)
{
	struct menu *child1;
	GtkTreeIter iter, tmp;
	GtkTreeIter *child2 = &iter;
	gboolean valid;
	GtkTreeIter *sibling;
	struct symbol *sym;
	struct menu *menu1, *menu2;

	if (src == &rootmenu)
		indent = 1;

	GtkTreeModel* model2 = gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w));
	valid = gtk_tree_model_iter_children(model2, child2, dst);
	for (child1 = src->list; child1; child1 = child1->next) {

		sym = child1->sym;

	      reparse:
		menu1 = child1;
		if (valid)
			gtk_tree_model_get(model2, child2, COL_MENU,
					   &menu2, -1);
		else
			menu2 = NULL;	// force adding of a first child

#ifdef DEBUG
		printf("%*c%s | %s\n", indent, ' ',
		       menu1 ? menu_get_prompt(menu1) : "nil",
		       menu2 ? menu_get_prompt(menu2) : "nil");
#endif

		if ((gconfwindow.opt_mode == OPT_NORMAL && !menu_is_visible(child1)) ||
		    (gconfwindow.opt_mode == OPT_PROMPT && !menu_has_prompt(child1)) ||
		    (gconfwindow.opt_mode == OPT_ALL    && !menu_get_prompt(child1))) {

			/* remove node */
			if (gtktree_iter_find_node(dst, menu1) != NULL) {
				memcpy(&tmp, child2, sizeof(GtkTreeIter));
				valid = gtk_tree_model_iter_next(model2,
								 child2);
				gtk_tree_store_remove(tree, &tmp);
				if (!valid)
					return;		/* next parent */
				else
					goto reparse;	/* next child */
			} else
				continue;
		}

		if (menu1 != menu2) {
			if (gtktree_iter_find_node(dst, menu1) == NULL) {	// add node
				if (!valid && !menu2)
					sibling = NULL;
				else
					sibling = child2;
				gtk_tree_store_insert_before(tree,
							     child2,
							     dst, sibling);
				set_node(child2, menu1, fill_row(menu1), tree);
				if (menu2 == NULL)
					valid = TRUE;
			} else {	// remove node
				memcpy(&tmp, child2, sizeof(GtkTreeIter));
				valid = gtk_tree_model_iter_next(model2,
								 child2);
				gtk_tree_store_remove(tree, &tmp);
				if (!valid)
					return;	// next parent
				else
					goto reparse;	// next child
			}
		} else if (sym && (sym->flags & SYMBOL_CHANGED)) {
			set_node(child2, menu1, fill_row(menu1), tree);
		}

		indent++;
		update_tree(child1, child2, tree);
		indent--;

		valid = gtk_tree_model_iter_next(model2, child2);
	}
}


/* Display the whole tree (single/split/full view) */
void display_tree(struct menu *menu, GtkTreeStore* tree)
{
	struct symbol *sym;
	struct property *prop;
	struct menu *child;
	enum prop_type ptype;
	GtkTreeStore* tree1 = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree1_w)));
	GtkTreeStore* tree2 = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w)));

	if (menu == &rootmenu) {
		indent = 1;
		currentMenu = &rootmenu;
	}

	for (child = menu->list; child; child = child->next) {
		prop = child->prompt;
		sym = child->sym;
		ptype = prop ? prop->type : P_UNKNOWN;

		if (sym)
			sym->flags &= ~SYMBOL_CHANGED;

		if ((view_mode == SPLIT_VIEW)
		    && !(child->flags & MENU_ROOT) && (tree == tree1))
			continue;

		if ((view_mode == SPLIT_VIEW) && (child->flags & MENU_ROOT)
		    && (tree == tree2))
			continue;

		if ((gconfwindow.opt_mode == OPT_NORMAL && menu_is_visible(child)) ||
		    (gconfwindow.opt_mode == OPT_PROMPT && menu_has_prompt(child)) ||
		    (gconfwindow.opt_mode == OPT_ALL    && menu_get_prompt(child)))
			place_node(child, fill_row(child), tree);
#ifdef DEBUG
		printf("%*c%s: ", indent, ' ', menu_get_prompt(child));
		printf("%s", child->flags & MENU_ROOT ? "rootmenu | " : "");
		printf("%s", prop_get_type_name(ptype));
		printf(" | ");
		if (sym) {
			printf("%s", sym_type_name(sym->type));
			printf(" | ");
			printf("%s", dbg_sym_flags(sym->flags));
			printf("\n");
		} else
			printf("\n");
#endif
		/* Do not show P_MENU entries in the right view if not in full view mode */
		if ((view_mode != FULL_VIEW) && (ptype == P_MENU)
		    && (tree == tree2))
			continue;

		if (((view_mode == SINGLE_VIEW) && (menu->flags & MENU_ROOT))
		    || (view_mode == FULL_VIEW)
		    || (view_mode == SPLIT_VIEW)) {
			indent++;
			display_tree(child, tree);
			indent--;
		}
	}
}

/* Display a part of the tree starting at current node (single/split view) */
void display_tree_part(struct menu *menu)
{
	GtkTreeStore* tree2 = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w)));
	gtk_tree_store_clear(tree2);
	
	currentMenu = menu;

	display_tree(menu, tree2);
	gtk_tree_view_expand_all(GTK_TREE_VIEW(gconfwindow.tree2_w));
}

/* Display the list in the left frame (split view) */
void display_list(void)
{
	GtkTreeStore* tree1 = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree1_w)));
	gtk_tree_store_clear(tree1);

	display_tree(&rootmenu, tree1);
	gtk_tree_view_expand_all(GTK_TREE_VIEW(gconfwindow.tree1_w));
}

void fixup_rootmenu(struct menu *menu)
{
	struct menu *child;
	static int menu_cnt = 0;

	menu->flags |= MENU_ROOT;
	for (child = menu->list; child; child = child->next) {
		if (child->prompt && child->prompt->type == P_MENU) {
			menu_cnt++;
			fixup_rootmenu(child);
			menu_cnt--;
		} else if (!menu_cnt)
			fixup_rootmenu(child);
	}
}
