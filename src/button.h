/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef _WIN32_BUTTON_H_
#define _WIN32_BUTTON_H_

#include <windows.h>
#include <glib-object.h>
#include "window.h"
#include "control.h"

typedef struct _Win32Button Win32Button;
typedef struct _Win32ButtonClass Win32ButtonClass;

#define WIN32_TYPE_BUTTON (win32_button_get_type ())
#define WIN32_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), WIN32_TYPE_BUTTON, Win32Button))
#define WIN32_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), WIN32_TYPE_BUTTON, Win32ButtonClass))
#define WIN32_IS_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WIN32_TYPE_BUTTON))
#define WIN32_IS_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), WIN32_TYPE_BUTTON))
#define WIN32_BUTTON_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), WIN32_TYPE_BUTTON, Win32ButtonClass))

G_DEFINE_AUTOPTR_CLEANUP_FUNC (Win32Button, win32_window_unref)

/* CLASS Button
------------------------------------------- */
struct _Win32Button {
    Win32Control parent_instance;
};

struct _Win32ButtonClass {
    Win32ControlClass parent_class;
};

Win32Button* win32_button_new (Win32Window* parent, const char * text);

/* INTERNAL */
GType win32_button_get_type (void) G_GNUC_CONST;
Win32Button* win32_button_construct (GType object_type, Win32Window* parent, const char *text);


#endif
