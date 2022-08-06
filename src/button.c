/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"
#include <stdio.h>

static WNDPROC g_controlProc = NULL;
static gpointer win32_button_parent_class = NULL;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static GType win32_button_get_type_once (void);
static void win32_button_finalize (Win32Window * obj);

HWND win32_button_create (Win32Window *self, Win32Window *parent);

/* CONSTRUCTOR
------------------------------------------- */
Win32Button* win32_button_construct (GType object_type, Win32Window* parent, const char * text)
{
    Win32Button* self = NULL;
    self = (Win32Button*) win32_control_construct (object_type, parent, win32_button_create, text);
    return self;
}

Win32Button* win32_button_new (Win32Window* parent, const char * text)
{
    return win32_button_construct (WIN32_TYPE_BUTTON, parent, text);
}


/* INTERNAL CREATE
------------------------------------------- */
HWND win32_button_create (Win32Window *self, Win32Window *parent)
{
    HWND hwnd;
    Win32Window  *window  = (Win32Window*) self;
    Win32Control *control = (Win32Control*) self;

    // defaults for a button
    window->width  = (window->pref_width > 0) ? window->pref_width : 75;
    window->height = (window->pref_height > 0) ? window->pref_height : 23;

    control->id = win32_control_generate_ID();
    wchar_t *text = fromUTF8(window->text);

    // Creating the Window
    hwnd = CreateWindow(
        L"BUTTON",
        text,
        // Control Styles:
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        window->left,   // x position
        window->top,    // y position
        window->width,  // width
        window->height, // height
        parent->hwnd,   // Parent window
        (HANDLE) control->id, // Control ID
        window->hInstance,
        NULL );

    window->hwnd = hwnd;

    g_controlProc = (WNDPROC) SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR) WndProc );

    free(text);

    return hwnd;
}


/* INTERNAL WINDOW PROCEDURE
------------------------------------------- */
// the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result;
    result = win32_window_default_procedure(hwnd, msg, wParam, lParam);
    if ( result == STOP_PROPAGATION ) return 0;

    return CallWindowProc( g_controlProc, hwnd, msg, wParam, lParam);
}


/* INTERNAL GTYPE
------------------------------------------- */
static void win32_button_class_init (Win32ButtonClass * klass, gpointer klass_data)
{
    win32_button_parent_class = g_type_class_peek_parent (klass);
    ((Win32WindowClass *) klass)->finalize = win32_button_finalize;
}

static void win32_button_instance_init (Win32Button * self, gpointer klass)
{
}

/* INTERNAL CLEANUP
------------------------------------------- */
static void win32_button_finalize (Win32Window * obj)
{
    Win32Button * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, WIN32_TYPE_BUTTON, Win32Button);

    WIN32_WINDOW_CLASS (win32_button_parent_class)->finalize (obj);
}


/* INTERNAL GTYPE REGISTRATION
------------------------------------------- */
static GType win32_button_get_type_once (void)
{
    static const GTypeInfo g_define_type_info = {
        sizeof (Win32ButtonClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) win32_button_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (Win32Button),
        0,
        (GInstanceInitFunc) win32_button_instance_init,
        NULL
    };
    GType win32_button_type_id;
    win32_button_type_id = g_type_register_static (WIN32_TYPE_CONTROL, "Win32Button", &g_define_type_info, 0);
    return win32_button_type_id;
}

GType win32_button_get_type (void)
{
    static volatile gsize win32_button_type_id__volatile = 0;
    if (g_once_init_enter (&win32_button_type_id__volatile)) {
        GType win32_button_type_id;
        win32_button_type_id = win32_button_get_type_once ();
        g_once_init_leave (&win32_button_type_id__volatile, win32_button_type_id);
    }
    return win32_button_type_id__volatile;
}
