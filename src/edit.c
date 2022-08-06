/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"
#include <stdio.h>

static WNDPROC g_controlProc = NULL;
static gpointer win32_edit_parent_class = NULL;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static GType win32_edit_get_type_once (void);
static void win32_edit_finalize (Win32Window * obj);

HWND win32_edit_create (Win32Window *self, Win32Window *parent);

/* CONSTRUCTOR
------------------------------------------- */
Win32Edit* win32_edit_construct (GType object_type, Win32Window* parent, const char * text)
{
    Win32Edit* self = NULL;
    self = (Win32Edit*) win32_control_construct (object_type, parent, win32_edit_create, text);
    return self;
}

Win32Edit* win32_edit_new (Win32Window* parent, const char * text)
{
    return win32_edit_construct (WIN32_TYPE_EDIT, parent, text);
}

Win32Edit* win32_edit_new_multiline (Win32Window* parent, const char * text)
{
    Win32Edit* self = win32_edit_construct (WIN32_TYPE_EDIT, parent, text);
    self->multiline = TRUE;
    return self;
}

Win32Edit* win32_edit_new_password (Win32Window* parent )
{
    Win32Edit* self = win32_edit_construct (WIN32_TYPE_EDIT, parent, "");
    self->password = TRUE;
    return self;
}


/* INTERNAL CREATE
------------------------------------------- */
HWND win32_edit_create (Win32Window *self, Win32Window *parent)
{
    HWND hwnd;
    Win32Window  *window  = (Win32Window*) self;
    Win32Control *control = (Win32Control*) self;
    Win32Edit    *edit    = (Win32Edit* ) self;

    control->id = win32_control_generate_ID ();
    wchar_t *text = fromUTF8(window->text);

    DWORD styles; // Default is SS_LEFT = 0
    switch (edit->text_align){
        case ALIGN_CENTER:
            styles = ES_CENTER;
            break;
        case ALIGN_RIGHT:
            styles = ES_RIGHT;
            break;
        default:
        case ALIGN_LEFT:
            styles = ES_LEFT;
            break;
    }

    if (edit->multiline) styles |= WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL;
    if (edit->readonly)  styles |= ES_READONLY;
    if (edit->password)  styles |= ES_PASSWORD;

    if (edit->text_align == ALIGN_LEFT && !edit->multiline) styles |= ES_AUTOHSCROLL;

    // default size
    window->width  = (window->pref_width > 0) ? window->pref_width : 100;
    window->height = (window->pref_height > 0) ? window->pref_height : 23;

    // Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        text,
        // Control Styles:
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | styles,
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


/* PROPERTY SET TEXT-ALIGNMENT
------------------------------------------- */
void win32_edit_set_text_align (Win32Edit *instance, int alignment)
{
    Win32Window *window = (Win32Window*) instance;

    DWORD new_style; // Default is ES_LEFT = 0
    LONG_PTR  style;

    /*NOOP*/
    if (instance->text_align == alignment) return;

    switch (alignment){
        case ALIGN_CENTER:
            new_style = ES_CENTER;
            break;
        case ALIGN_RIGHT:
            new_style = ES_RIGHT;
            break;
        default:
        case ALIGN_LEFT:
            new_style = ES_LEFT;
            break;
    }
    instance->text_align = (int) new_style;

    if (alignment == ALIGN_LEFT && !instance->multiline) new_style |= ES_AUTOHSCROLL;

    if (window->hwnd != NULL){
        style = GetWindowLongPtr( window->hwnd, GWL_STYLE );
        // Clear the previous alignment style then apply the new one
        style = ( style & ~(ES_LEFT | ES_CENTER | ES_RIGHT | ES_AUTOHSCROLL) ) | new_style;
        SetWindowLongPtr( window->hwnd, GWL_STYLE, style );
        InvalidateRect( window->hwnd, NULL, TRUE );
    }
}


/* PROPERTY GET TEXT-ALIGNMENT
------------------------------------------- */
int win32_edit_get_text_align (Win32Edit *instance)
{
    return instance->text_align;
}


/* PROPERTY SET READONLY
------------------------------------------- */
void win32_edit_set_readonly (Win32Edit *instance, BOOL value)
{
    Win32Window *window = (Win32Window*) instance;
    // NOOP
    if (instance->readonly == value) return;

    instance->readonly = value;

    if (window->hwnd != NULL)
    {
        if (instance->readonly) SendMessage(window->hwnd, EM_SETREADONLY, (LPARAM) TRUE, 0);
        else SendMessage(window->hwnd, EM_SETREADONLY, (LPARAM) FALSE, 0);

        InvalidateRect( window->hwnd, NULL, TRUE );
    }
}


/* PROPERTY GET READONLY
------------------------------------------- */
BOOL win32_edit_get_readonly (Win32Edit *instance)
{
    return instance->readonly;
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
static void win32_edit_class_init (Win32EditClass * klass, gpointer klass_data)
{
    win32_edit_parent_class = g_type_class_peek_parent (klass);
    ((Win32WindowClass *) klass)->finalize = win32_edit_finalize;
}

static void win32_edit_instance_init (Win32Edit * self, gpointer klass)
{
}

/* INTERNAL CLEANUP
------------------------------------------- */
static void win32_edit_finalize (Win32Window * obj)
{
    Win32Edit * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, WIN32_TYPE_EDIT, Win32Edit);
    WIN32_WINDOW_CLASS (win32_edit_parent_class)->finalize (obj);
}


/* INTERNAL GTYPE REGISTRATION
------------------------------------------- */
static GType win32_edit_get_type_once (void)
{
    static const GTypeInfo g_define_type_info = {
        sizeof (Win32EditClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) win32_edit_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (Win32Edit),
        0,
        (GInstanceInitFunc) win32_edit_instance_init,
        NULL
    };
    GType win32_edit_type_id;
    win32_edit_type_id = g_type_register_static (WIN32_TYPE_CONTROL, "Win32Edit", &g_define_type_info, 0);
    return win32_edit_type_id;
}

GType win32_edit_get_type (void)
{
    static volatile gsize win32_edit_type_id__volatile = 0;
    if (g_once_init_enter (&win32_edit_type_id__volatile)) {
        GType win32_edit_type_id;
        win32_edit_type_id = win32_edit_get_type_once ();
        g_once_init_leave (&win32_edit_type_id__volatile, win32_edit_type_id);
    }
    return win32_edit_type_id__volatile;
}
