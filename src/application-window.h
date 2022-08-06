/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef _WIN32_APPLICATION_WINDOW_H_
#define _WIN32_APPLICATION_WINDOW_H_

#include <windows.h>
#include <glib-object.h>
#include "window.h"
#include "container.h"

typedef struct _Win32ApplicationWindow Win32ApplicationWindow;
typedef struct _Win32ApplicationWindowClass Win32ApplicationWindowClass;

#define WIN32_TYPE_APPLICATION_WINDOW (win32_application_window_get_type ())
#define WIN32_APPLICATION_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), WIN32_TYPE_APPLICATION_WINDOW, Win32ApplicationWindow))
#define WIN32_APPLICATION_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), WIN32_TYPE_APPLICATION_WINDOW, Win32ApplicationWindowClass))
#define WIN32_IS_APPLICATION_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WIN32_TYPE_APPLICATION_WINDOW))
#define WIN32_IS_APPLICATION_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), WIN32_TYPE_APPLICATION_WINDOW))
#define WIN32_APPLICATION_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), WIN32_TYPE_APPLICATION_WINDOW, Win32ApplicationWindowClass))

G_DEFINE_AUTOPTR_CLEANUP_FUNC (Win32ApplicationWindow, win32_window_unref)

/* CLASS ApplicationWindow
------------------------------------------- */
struct _Win32ApplicationWindow {
    Win32Container parent_instance;
    INT min_width;
    INT min_height;
    INT max_width;
    INT max_height;
};

struct _Win32ApplicationWindowClass {
    Win32ContainerClass parent_class;
};

Win32ApplicationWindow* win32_application_window_new (const char * title);
void win32_application_window_show (Win32ApplicationWindow *self);

void win32_application_window_set_min_width  (Win32ApplicationWindow *self, INT min_width);
INT  win32_application_window_get_min_width  (Win32ApplicationWindow *self);

void win32_application_window_set_min_height (Win32ApplicationWindow *self, INT min_height);
INT  win32_application_window_get_min_height (Win32ApplicationWindow *self);

void win32_application_window_set_max_width  (Win32ApplicationWindow *self, INT max_width);
INT  win32_application_window_get_max_width  (Win32ApplicationWindow *self);

void win32_application_window_set_max_height (Win32ApplicationWindow *self, INT max_height);
INT  win32_application_window_get_max_height (Win32ApplicationWindow *self);

/* INTERNAL */
void win32_application_window_create (Win32ApplicationWindow *self);

/* GTYPE */
GType win32_application_window_get_type (void) G_GNUC_CONST;
Win32ApplicationWindow* win32_application_window_construct (GType object_type, const char * title);
GType win32_application_window_get_type1 (void* obj);


#endif
