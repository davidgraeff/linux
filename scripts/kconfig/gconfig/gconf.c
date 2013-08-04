/*
 *
 * Copyright (C) 2013 David Gräff <david.graeff@web.de>
 * Copyright (C) 2002-2003 Romain Lievin <roms@tilp.info>
 * Released under the terms of the GNU GPL v2.0.
 *
 */

#include "gconf.h"
#include "gconf_treeview_model.h"
#define IMAGES_TOOLBAR
#include "../images.c"

enum view_mode_enum view_mode = FULL_VIEW;
struct gconfwindow_s gconfwindow;
GtkTextTag *tag1, *tag2;

static void replace_button_icon(GtkBuilder * builder,
			 gchar * btn_name, const char ** xpm)
{
	GdkPixbuf * pixmap;
	GtkToolButton *button;
	GtkWidget *image;

	pixmap = gdk_pixbuf_new_from_xpm_data(xpm);

	button = GTK_TOOL_BUTTON(gtk_builder_get_object(builder, btn_name));
	image = gtk_image_new_from_pixbuf(pixmap);
	gtk_widget_show(image);
	gtk_tool_button_set_icon_widget(button, image);
}

static void conf_changed(void)
{
	bool changed = conf_get_changed();
	gtk_widget_set_sensitive(gconfwindow.save_btn, changed);
	gtk_widget_set_sensitive(gconfwindow.save_menu_item, changed);
}

/* Main Window Initialization */
static void init_main_window(const gchar * ui_file)
{
	GtkBuilder *builder;
	GtkWidget *widget;
	GtkTextBuffer *txtbuf;
	
	// init default values for options
	gconfwindow.show_name = TRUE;
	gconfwindow.show_range = TRUE;
	gconfwindow.show_value = TRUE;
	gconfwindow.resizeable = FALSE;
	gconfwindow.opt_mode = OPT_NORMAL;

	builder = gtk_builder_new();
	int result = gtk_builder_add_from_file(builder, ui_file, NULL);
	if (!result)
		g_error(_("GUI loading failed !\n"));
	gtk_builder_connect_signals (builder, NULL);

	gconfwindow.main_wnd = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
	gconfwindow.hpaned = GTK_WIDGET(gtk_builder_get_object(builder, "hpaned1"));
	gconfwindow.tree1_w = GTK_WIDGET(gtk_builder_get_object(builder, "treeview1"));
	gconfwindow.tree2_w = GTK_WIDGET(gtk_builder_get_object(builder, "treeview2"));
	gconfwindow.text_w = GTK_WIDGET(gtk_builder_get_object(builder, "textview3"));

	gconfwindow.back_btn = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
	gtk_widget_set_sensitive(gconfwindow.back_btn, FALSE);

	widget = GTK_WIDGET(gtk_builder_get_object(builder, "show_name1"));
	gtk_check_menu_item_set_active((GtkCheckMenuItem *) widget,
				       gconfwindow.show_name);

	widget = GTK_WIDGET(gtk_builder_get_object(builder, "show_range1"));
	gtk_check_menu_item_set_active((GtkCheckMenuItem *) widget,
				       gconfwindow.show_range);

	widget = GTK_WIDGET(gtk_builder_get_object(builder, "show_data1"));
	gtk_check_menu_item_set_active((GtkCheckMenuItem *) widget,
				       gconfwindow.show_value);

	gconfwindow.save_btn = GTK_WIDGET(gtk_builder_get_object(builder, "button3"));
	gconfwindow.save_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "save1"));
	conf_set_changed_callback(conf_changed);

	widget = GTK_WIDGET(gtk_builder_get_object(builder, "toolbar1"));

	replace_button_icon(builder, "button4", xpm_single_view);
	replace_button_icon(builder, "button5", xpm_split_view);
	replace_button_icon(builder, "button6", xpm_tree_view);
	
	g_object_unref (G_OBJECT (builder));

	txtbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gconfwindow.text_w));
	tag1 = gtk_text_buffer_create_tag(txtbuf, "mytag1",
					  "foreground", "red",
					  "weight", PANGO_WEIGHT_BOLD,
					  NULL);
	tag2 = gtk_text_buffer_create_tag(txtbuf, "mytag2",
					  /*"style", PANGO_STYLE_OBLIQUE, */
					  NULL);

	gtk_window_set_title(GTK_WINDOW(gconfwindow.main_wnd), rootmenu.prompt->text);

	gtk_widget_show(gconfwindow.main_wnd);
}

/**
 * Called by main() and load..(). Display data depending on the view mode
 * and changes the layout.
 */
void show_data(void)
{
	switch (view_mode) {
		case SINGLE_VIEW:
			gtk_widget_hide(gconfwindow.tree1_w);
			display_tree_part(&rootmenu);
			gtk_paned_set_position(GTK_PANED(gconfwindow.hpaned), 0);
			break;
		case SPLIT_VIEW:
		{
			gtk_widget_show(gconfwindow.tree1_w);
			gint w, h;
			gtk_window_get_default_size(GTK_WINDOW(gconfwindow.main_wnd), &w, &h);
			gtk_paned_set_position(GTK_PANED(gconfwindow.hpaned), w / 2);
			// Clear right frame, the user has to choose a subtree first
			GtkTreeStore* tree2 = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w)));
			gtk_tree_store_clear(tree2);
			// Display tree in left frame
			display_list();

			/* Disable back btn, like in full mode. */
			gtk_widget_set_sensitive(gconfwindow.back_btn, FALSE);
			break;
		}
		case FULL_VIEW:
		{
			gtk_widget_hide(gconfwindow.tree1_w);
			GtkTreeStore* tree2 = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w)));
			gtk_tree_store_clear(tree2);
			display_tree(&rootmenu, tree2);
			gtk_paned_set_position(GTK_PANED(gconfwindow.hpaned), 0);
			gtk_widget_set_sensitive(gconfwindow.back_btn, FALSE);
			break;
		}
	}
}

/* Utility Functions */


static void text_insert_help(struct menu *menu)
{
	GtkTextBuffer *buffer;
	GtkTextIter start, end;
	const char *prompt = _(menu_get_prompt(menu));
	struct gstr help = str_new();

	menu_get_ext_help(menu, &help);

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gconfwindow.text_w));
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	gtk_text_buffer_delete(buffer, &start, &end);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(gconfwindow.text_w), 15);

	gtk_text_buffer_get_end_iter(buffer, &end);
	gtk_text_buffer_insert_with_tags(buffer, &end, prompt, -1, tag1,
					 NULL);
	gtk_text_buffer_insert_at_cursor(buffer, "\n\n", 2);
	gtk_text_buffer_get_end_iter(buffer, &end);
	gtk_text_buffer_insert_with_tags(buffer, &end, str_get(&help), -1, tag2,
					 NULL);
	str_free(&help);
}


static void text_insert_msg(const char *title, const char *message)
{
	GtkTextBuffer *buffer;
	GtkTextIter start, end;
	const char *msg = message;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gconfwindow.text_w));
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	gtk_text_buffer_delete(buffer, &start, &end);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(gconfwindow.text_w), 15);

	gtk_text_buffer_get_end_iter(buffer, &end);
	gtk_text_buffer_insert_with_tags(buffer, &end, title, -1, tag1,
					 NULL);
	gtk_text_buffer_insert_at_cursor(buffer, "\n\n", 2);
	gtk_text_buffer_get_end_iter(buffer, &end);
	gtk_text_buffer_insert_with_tags(buffer, &end, msg, -1, tag2,
					 NULL);
}


/* Main Windows Callbacks */

gboolean on_window1_delete_event(GtkWidget * widget, GdkEvent * event,
				 gpointer user_data)
{
	GtkWidget *dialog, *label;
	gint result;

	if (!conf_get_changed())
		return FALSE;

	dialog = gtk_dialog_new_with_buttons(_("Warning !"),
					     GTK_WINDOW(gconfwindow.main_wnd),
					     (GtkDialogFlags)
					     (GTK_DIALOG_MODAL |
					      GTK_DIALOG_DESTROY_WITH_PARENT),
					     GTK_STOCK_OK,
					     GTK_RESPONSE_YES,
					     GTK_STOCK_NO,
					     GTK_RESPONSE_NO,
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog),
					GTK_RESPONSE_CANCEL);

	label = gtk_label_new(_("\nSave configuration ?\n"));
	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label);
	gtk_widget_show(label);

	result = gtk_dialog_run(GTK_DIALOG(dialog));
	switch (result) {
	case GTK_RESPONSE_YES:
		if (conf_write(NULL))
			text_insert_msg(_("Error"), _("Unable to save configuration on exit!"));
		return FALSE;
	case GTK_RESPONSE_NO:
		return FALSE;
	case GTK_RESPONSE_CANCEL:
	case GTK_RESPONSE_DELETE_EVENT:
	default:
		gtk_widget_destroy(dialog);
		return TRUE;
	}

	return FALSE;
}


void on_window1_destroy(GObject * object, gpointer user_data)
{
	gtk_main_quit();
}


/* Menu & Toolbar Callbacks */

void on_load1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new (_("Load file..."),
						GTK_WINDOW(gconfwindow.main_wnd),
						GTK_FILE_CHOOSER_ACTION_OPEN,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						NULL);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if (conf_read(filename))
			text_insert_msg(_("Error"), _("Unable to load configuration !"));
		else
			show_data();
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

void on_save_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (conf_write(NULL))
		text_insert_msg(_("Error"), _("Unable to save configuration !"));
}

void on_save_as1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new (_("Save File as..."),
				      GTK_WINDOW(gconfwindow.main_wnd),
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
	
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if (conf_write(filename))
			text_insert_msg(_("Error"), _("Unable to save configuration !"));
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}


void on_quit1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!on_window1_delete_event(NULL, NULL, NULL))
		gtk_widget_destroy(GTK_WIDGET(gconfwindow.main_wnd));
}


void on_show_name1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkTreeViewColumn *col;

	gconfwindow.show_name = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(gconfwindow.tree2_w), COL_NAME);
	if (col)
		gtk_tree_view_column_set_visible(col, gconfwindow.show_name);
}


void on_show_range1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkTreeViewColumn *col;

	gconfwindow.show_range = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(gconfwindow.tree2_w), COL_NO);
	if (col)
		gtk_tree_view_column_set_visible(col, gconfwindow.show_range);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(gconfwindow.tree2_w), COL_MOD);
	if (col)
		gtk_tree_view_column_set_visible(col, gconfwindow.show_range);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(gconfwindow.tree2_w), COL_YES);
	if (col)
		gtk_tree_view_column_set_visible(col, gconfwindow.show_range);

}


void on_show_data1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkTreeViewColumn *col;

	gconfwindow.show_value = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(gconfwindow.tree2_w), COL_VALUE);
	if (col)
		gtk_tree_view_column_set_visible(col, gconfwindow.show_value);
}


void
on_set_option_mode1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	gconfwindow.opt_mode = OPT_NORMAL;
	GtkTreeStore* tree2 = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w)));
	gtk_tree_store_clear(tree2);
	display_tree(&rootmenu, tree2);	/* instead of update_tree to speed-up */
}


void
on_set_option_mode2_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	gconfwindow.opt_mode = OPT_ALL;
	GtkTreeStore* tree2 = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w)));
	gtk_tree_store_clear(tree2);
	display_tree(&rootmenu, tree2);	/* instead of update_tree to speed-up */
}


void
on_set_option_mode3_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	gconfwindow.opt_mode = OPT_PROMPT;
	GtkTreeStore* tree2 = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w)));
	gtk_tree_store_clear(tree2);
	display_tree(&rootmenu, tree2);	/* instead of update_tree to speed-up */
}


void on_introduction1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dialog;
	const gchar *intro_text = _(
	    "Welcome to gkc, the GTK+ graphical configuration tool\n"
	    "For each option, a blank box indicates the feature is disabled, a\n"
	    "check indicates it is enabled, and a dot indicates that it is to\n"
	    "be compiled as a module.  Clicking on the box will cycle through the three states.\n"
	    "\n"
	    "If you do not see an option (e.g., a device driver) that you\n"
	    "believe should be present, try turning on Show All Options\n"
	    "under the Options menu.\n"
	    "Although there is no cross reference yet to help you figure out\n"
	    "what other options must be enabled to support the option you\n"
	    "are interested in, you can still view the help of a grayed-out\n"
	    "option.\n"
	    "\n"
	    "Toggling Show Debug Info under the Options menu will show \n"
	    "the dependencies, which you can then match by examining other options.");

	dialog = gtk_message_dialog_new(GTK_WINDOW(gconfwindow.main_wnd),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_INFO,
					GTK_BUTTONS_CLOSE, "%s", intro_text);
	g_signal_connect(dialog, "response",
				 G_CALLBACK(gtk_widget_destroy),
				 dialog);
	gtk_widget_show_all(dialog);
}


void on_about1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dialog;
	const gchar *about_text =
	    _("Updated to gtk3, copyright (c) 2013 David Gräff <david.graeff@udo.edu>.\n"
		  "gkc is copyright (c) 2002 Romain Lievin <roms@lpg.ticalc.org>.\n"
	      "Based on the source code from Roman Zippel.\n");

	dialog = gtk_message_dialog_new(GTK_WINDOW(gconfwindow.main_wnd),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_INFO,
					GTK_BUTTONS_CLOSE, "%s", about_text);
	g_signal_connect(dialog, "response",
				 G_CALLBACK(gtk_widget_destroy),
				 dialog);
	gtk_widget_show_all(dialog);
}


void on_license1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dialog;
	const gchar *license_text =
	    _("gkc is released under the terms of the GNU GPL v2.\n"
	      "For more information, please see the source code or\n"
	      "visit http://www.fsf.org/licenses/licenses.html\n");

	dialog = gtk_message_dialog_new(GTK_WINDOW(gconfwindow.main_wnd),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_INFO,
					GTK_BUTTONS_CLOSE, "%s", license_text);
	g_signal_connect(dialog, "response",
				 G_CALLBACK(gtk_widget_destroy),
				 dialog);
	gtk_widget_show_all(dialog);
}


void on_back_clicked(GtkButton * button, gpointer user_data)
{
	enum prop_type ptype;

	struct menu *current = getCurrentMenu()->parent;
	ptype = current->prompt ? current->prompt->type : P_UNKNOWN;
	if (ptype != P_MENU)
		current = current->parent;
	
	if (current == &rootmenu)
		gtk_widget_set_sensitive(gconfwindow.back_btn, FALSE);

	display_tree_part(current);
}


void on_load_clicked(GtkButton * button, gpointer user_data)
{
	on_load1_activate(NULL, user_data);
}


void on_single_clicked(GtkButton * button, gpointer user_data)
{
	view_mode = SINGLE_VIEW;
	show_data();
}


void on_split_clicked(GtkButton * button, gpointer user_data)
{
	view_mode = SPLIT_VIEW;
	show_data();
}


void on_full_clicked(GtkButton * button, gpointer user_data)
{
	view_mode = FULL_VIEW;
	show_data();
}


void on_collapse_clicked(GtkButton * button, gpointer user_data)
{
	gtk_tree_view_collapse_all(GTK_TREE_VIEW(gconfwindow.tree2_w));
}


void on_expand_clicked(GtkButton * button, gpointer user_data)
{
	gtk_tree_view_expand_all(GTK_TREE_VIEW(gconfwindow.tree2_w));
}

/**
 * If you click on one of the columns N|M|Y this method is called
 * by the toggle signal handler (via a proxy methods below)
 */
void changeEnabledToogle(int colChangable, int colValue, gchar *path_str)
{
	GtkTreeIter  iter;
	struct menu *menu;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
	GtkTreeModel* model2 = gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w));
	gtk_tree_model_get_iter (model2, &iter, path);
	gtk_tree_model_get(model2, &iter, COL_MENU, &menu, -1); // get menu data
	// get changable state
	gboolean enabled;
	gtk_tree_model_get(model2, &iter, colChangable, &enabled, -1);
	if (!enabled) // not changable, do nothing
		return;
	
	if (colValue != -1)
		change_sym_value(menu, colValue, GTK_TREE_STORE(model2));
	else
		toggle_sym_value(menu, GTK_TREE_STORE(model2));
	
	gtk_tree_view_expand_row(GTK_TREE_VIEW (gconfwindow.tree2_w), path, TRUE);
	gtk_tree_path_free (path);
}

void on_treeview2_makeDisable_clicked(GtkCellRendererToggle *cell, gchar *path_str,
               gpointer data)
{
	changeEnabledToogle(COL_NO_EN, COL_NO, path_str);
};

void on_treeview2_makeModule_clicked(GtkCellRendererToggle *cell, gchar *path_str,
               gpointer data)
{
	changeEnabledToogle(COL_MOD_EN, COL_MOD, path_str);
};

void on_treeview2_makeEnable_clicked(GtkCellRendererToggle *cell, gchar *path_str,
               gpointer data)
{
	changeEnabledToogle(COL_YES_EN, COL_YES, path_str);
};

void
on_treeview2_toggleEnable_clicked(GtkCellRendererToggle *cell, gchar *path_str,
               gpointer data)
{
	changeEnabledToogle(COL_BTNVISIBLE, -1, path_str);
}

/* Key pressed: update choice */
gboolean
on_treeviews_key_press_event(GtkWidget * widget,
			     GdkEventKey * event, gpointer user_data)
{
	GtkTreeView *view = GTK_TREE_VIEW(widget);
	GtkTreeModel* model2 = gtk_tree_view_get_model(view);
	GtkTreeStore* tree2 = GTK_TREE_STORE(model2);
	GtkTreePath *path;
	GtkTreeViewColumn *column;
	GtkTreeIter iter;
	struct menu *menu;
	gint col;

	gtk_tree_view_get_cursor(view, &path, &column);
	if (path == NULL)
		return FALSE;

	if (event->keyval == GDK_KEY_space) {
		if (gtk_tree_view_row_expanded(view, path))
			gtk_tree_view_collapse_row(view, path);
		else
			gtk_tree_view_expand_row(view, path, FALSE);
		return TRUE;
	}
	if (event->keyval == GDK_KEY_KP_Enter) {
	}
	if (widget == gconfwindow.tree1_w)
		return FALSE;

	gtk_tree_model_get_iter(model2, &iter, path);
	gtk_tree_model_get(model2, &iter, COL_MENU, &menu, -1);

	if (!strncasecmp(event->string, "n", 1))
		col = COL_NO;
	else if (!strncasecmp(event->string, "m", 1))
		col = COL_MOD;
	else if (!strncasecmp(event->string, "y", 1))
		col = COL_YES;
	else
		col = -1;
	change_sym_value(menu, col, tree2);

	return FALSE;
}


/* User click: goes down (single) */
gboolean
on_treeview2_button_press_event(GtkWidget * widget,
				GdkEventButton * event, gpointer user_data)
{
	if (event->type != GDK_2BUTTON_PRESS || view_mode == FULL_VIEW) {
		return FALSE;
	}
	
	GtkTreeView *view = GTK_TREE_VIEW(widget);
	GtkTreeModel* model2 = gtk_tree_view_get_model(view);
	GtkTreePath *path;
	GtkTreeViewColumn *column;
	GtkTreeIter iter;
	struct menu *menu;

	gtk_tree_view_get_cursor(view, &path, &column);
	if (path == NULL)
		return FALSE;

	if (!gtk_tree_model_get_iter(model2, &iter, path))
		return FALSE;
	gtk_tree_model_get(model2, &iter, COL_MENU, &menu, -1);

	enum prop_type ptype;
	ptype = menu->prompt ? menu->prompt->type : P_UNKNOWN;

	if (ptype == P_MENU) {
		// goes down into menu
		display_tree_part(menu);
		gtk_widget_set_sensitive(gconfwindow.back_btn, TRUE);
	}
	
	return FALSE;
}


/* Row selection changed: update help */
void
on_treeview2_selection_changed(GtkTreeSelection *selection, gpointer user_data)
{
	GtkTreeIter iter;
	struct menu *menu;

	GtkTreeModel* model2 = gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree2_w));
	if (gtk_tree_selection_get_selected(selection, &model2, &iter)) {
		gtk_tree_model_get(model2, &iter, COL_MENU, &menu, -1);
		text_insert_help(menu);
	}
}

/* User click: display sub-tree in the right frame. */
void
on_treeview1_selection_changed(GtkTreeSelection *selection, gpointer user_data)
{
	GtkTreeIter iter;
	struct menu *menu;

	GtkTreeModel* model1 = gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree1_w));
	if (gtk_tree_selection_get_selected(selection, &model1, &iter)) {
		gtk_tree_model_get(model1, &iter, COL_MENU, &menu, -1);

		display_tree_part(menu);
		text_insert_help(menu);

		gtk_widget_realize(gconfwindow.tree2_w);
		gtk_widget_grab_focus(gconfwindow.tree2_w);
	}
}

/* User clicked on checkbox in treeview 1. */
void
on_treeview1_checkbox_toggle(GtkCellRendererToggle *cell, gchar *path_str,
               gpointer data)
{
	GtkTreeIter  iter;
	struct menu *menu;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
	if (path == NULL)
		return;
		
	GtkTreeModel* model1 = gtk_tree_view_get_model(GTK_TREE_VIEW (gconfwindow.tree1_w));
	GtkTreeStore* tree1 = GTK_TREE_STORE(model1);
	gtk_tree_model_get_iter (model1, &iter, path);
	gtk_tree_path_free (path);
	gtk_tree_model_get(model1, &iter, COL_MENU, &menu, -1); // get menu data
	toggle_sym_value(menu, tree1); // toggle value internally
	display_tree_part(menu); // show new tree
}

/* Main */
int main(int ac, char *av[])
{
	const char *name;
	char *env;
	gchar *ui_file;

	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
	setlocale (LC_ALL, NULL);

	/* GTK stuffs */
	gtk_init(&ac, &av);

	//add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
	//add_pixmap_directory (PACKAGE_SOURCE_DIR "/pixmaps");

	/* Determine GUI path */
	env = getenv(SRCTREE);
	if (env)
		ui_file = g_strconcat(env, "/scripts/kconfig/gconfig/gconf.ui", NULL);
	else if (av[0][0] == '/')
		ui_file = g_strconcat(av[0], ".ui", NULL);
	else
		ui_file = g_strconcat(g_get_current_dir(), "/", av[0], ".ui", NULL);

	/* Conf stuffs */
	if (ac > 1 && av[1][0] == '-') {
		switch (av[1][1]) {
		case 'a':
			//showAll = 1;
			break;
		case 'h':
		case '?':
			printf("%s <config>\n", av[0]);
			exit(0);
		}
	}
	
	if (ac > 2 && av[1][0] == '-') {
		name = av[2];
	} else if(ac>1) {
		name = av[1];
	} else {
		printf("No config file!\n");
		exit(0);
	}

	conf_parse(name);
	fixup_rootmenu(&rootmenu);
	conf_read(NULL);
	
	/* Load the interface and connect signals */
	init_main_window(ui_file);
	init_node_parents();
	init_left_tree();
	init_right_tree();
	show_data();

	gtk_main();

	return 0;
}

