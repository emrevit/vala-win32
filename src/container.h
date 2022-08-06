/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef _WIN32_CONTAINER_H_
#define _WIN32_CONTAINER_H_

#include <windows.h>
#include <glib-object.h>
#include "window.h"

typedef struct _Win32WindowList Win32WindowList;

typedef struct _Win32ContainerClass Win32ContainerClass;

#define WIN32_TYPE_CONTAINER (win32_container_get_type ())
#define WIN32_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), WIN32_TYPE_CONTAINER, Win32Container))
#define WIN32_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), WIN32_TYPE_CONTAINER, Win32ContainerClass))
#define WIN32_IS_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WIN32_TYPE_CONTAINER))
#define WIN32_IS_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), WIN32_TYPE_CONTAINER))
#define WIN32_CONTAINER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), WIN32_TYPE_CONTAINER, Win32ContainerClass))

G_DEFINE_AUTOPTR_CLEANUP_FUNC (Win32Container, win32_window_unref)

/* CLASS Container
------------------------------------------- */
struct _Win32WindowList {
    Win32Window ** items;
    size_t length;
};

struct _Win32Container {
    Win32Window parent_instance;
    Win32WindowList childWindows;
    Win32Layout * layout;
};

struct _Win32ContainerClass {
    Win32WindowClass parent_class;
};

Win32Window ** win32_container_window_get_children (Win32Container *self, size_t *length );
void           win32_container_set_layout  (Win32Container *self, Win32Layout *layout);
Win32Layout *  win32_container_get_layout  (Win32Container *self);

/* INTERNAL */
void win32_container_add_child (Win32Container *self, Win32Window *child);

GType win32_container_get_type (void) G_GNUC_CONST;
Win32Container* win32_container_construct (GType object_type);


#endif
