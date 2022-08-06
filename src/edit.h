/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef _WIN32_EDIT_H_
#define _WIN32_EDIT_H_

#include <windows.h>
#include <glib-object.h>
#include "window.h"
#include "control.h"

typedef struct _Win32Edit Win32Edit;
typedef struct _Win32EditClass Win32EditClass;

#define WIN32_TYPE_EDIT (win32_edit_get_type ())
#define WIN32_EDIT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), WIN32_TYPE_EDIT, Win32Edit))
#define WIN32_EDIT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), WIN32_TYPE_EDIT, Win32EditClass))
#define WIN32_IS_EDIT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WIN32_TYPE_EDIT))
#define WIN32_IS_EDIT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), WIN32_TYPE_EDIT))
#define WIN32_EDIT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), WIN32_TYPE_EDIT, Win32EditClass))

G_DEFINE_AUTOPTR_CLEANUP_FUNC (Win32Edit, win32_window_unref)

/* CLASS Edit
------------------------------------------- */
struct _Win32Edit {
    Win32Control parent_instance;
    int text_align;
    BOOL multiline;
    BOOL readonly;
    BOOL password;
};

struct _Win32EditClass {
    Win32ControlClass parent_class;
};

Win32Edit* win32_edit_new (Win32Window* parent, const char * text);
Win32Edit* win32_edit_new_multiline (Win32Window* parent, const char * text);
Win32Edit* win32_edit_new_password (Win32Window* parent );

void win32_edit_set_text_align (Win32Edit *instance, int alignment);
int  win32_edit_get_text_align (Win32Edit *instance);

void win32_edit_set_readonly (Win32Edit *instance, BOOL value);
BOOL win32_edit_get_readonly (Win32Edit *instance);

/* INTERNAL */
GType win32_edit_get_type (void) G_GNUC_CONST;
Win32Edit* win32_edit_construct (GType object_type, Win32Window* parent, const char *text);


#endif
