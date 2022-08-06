/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef _WIN32_CONTROL_H_
#define _WIN32_CONTROL_H_

#include <windows.h>
#include <glib-object.h>
#include "window.h"

#define ALIGN_LEFT      0
#define ALIGN_CENTER    1
#define ALIGN_RIGHT     2

typedef struct _Win32CreationData Win32CreationData;
typedef HWND (*Win32WindowCreator) ( Win32Window *child, Win32Window *parent );

typedef struct _Win32Control Win32Control;
typedef struct _Win32ControlClass Win32ControlClass;

#define WIN32_TYPE_CONTROL (win32_control_get_type ())
#define WIN32_CONTROL(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), WIN32_TYPE_CONTROL, Win32Control))
#define WIN32_CONTROL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), WIN32_TYPE_CONTROL, Win32ControlClass))
#define WIN32_IS_CONTROL(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WIN32_TYPE_CONTROL))
#define WIN32_IS_CONTROL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), WIN32_TYPE_CONTROL))
#define WIN32_CONTROL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), WIN32_TYPE_CONTROL, Win32ControlClass))

G_DEFINE_AUTOPTR_CLEANUP_FUNC (Win32Control, win32_window_unref)

/* CLASS Control
------------------------------------------- */
struct _Win32CreationData {
    int ref_count; // not used, just for padding
    Win32WindowCreator create_window;
    Win32Control * control;
};

struct _Win32Control {
    Win32Window parent_instance;
    UINT id;
};

struct _Win32ControlClass {
    Win32WindowClass parent_class;
};

/* INTERNAL */
unsigned int win32_control_generate_ID (void);
Win32Control *win32_control_create( Win32Control *control, Win32Window* parent, Win32WindowCreator create_function, const char *text);

GType win32_control_get_type (void) G_GNUC_CONST;
Win32Control* win32_control_construct (GType object_type,
                                       Win32Window *parent,
                                       Win32WindowCreator create_window,
                                       const char *text );

#endif
