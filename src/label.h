/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef _WIN32_LABEL_H_
#define _WIN32_LABEL_H_

#include <windows.h>
#include <glib-object.h>
#include "window.h"
#include "control.h"

typedef struct _Win32Label Win32Label;
typedef struct _Win32LabelClass Win32LabelClass;

#define WIN32_TYPE_LABEL (win32_label_get_type ())
#define WIN32_LABEL(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), WIN32_TYPE_LABEL, Win32Label))
#define WIN32_LABEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), WIN32_TYPE_LABEL, Win32LabelClass))
#define WIN32_IS_LABEL(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WIN32_TYPE_LABEL))
#define WIN32_IS_LABEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), WIN32_TYPE_LABEL))
#define WIN32_LABEL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), WIN32_TYPE_LABEL, Win32LabelClass))

G_DEFINE_AUTOPTR_CLEANUP_FUNC (Win32Label, win32_window_unref)

/* CLASS Label
------------------------------------------- */
struct _Win32Label {
    Win32Control parent_instance;
    int text_align;
};

struct _Win32LabelClass {
    Win32ControlClass parent_class;
};

Win32Label* win32_label_new (Win32Window* parent, const char * text);

void  win32_label_set_text_align  (Win32Label *instance, int alignment);
int   win32_label_get_text_align  (Win32Label *instance);

/* INTERNAL */
GType win32_label_get_type (void) G_GNUC_CONST;
Win32Label* win32_label_construct (GType object_type, Win32Window* parent, const char *text);

#endif
