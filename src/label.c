/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"
#include <stdio.h>

static WNDPROC g_controlProc = NULL;
static gpointer win32_label_parent_class = NULL;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static GType win32_label_get_type_once (void);
static void win32_label_finalize (Win32Window * obj);
static void win32_label_auto_resize (Win32Window *window, const void *data);

HWND win32_label_create (Win32Window *self, Win32Window *parent);

/* CONSTRUCTOR
------------------------------------------- */
Win32Label* win32_label_construct (GType object_type, Win32Window* parent, const char * text)
{
    Win32Label* self = NULL;
    self = (Win32Label*) win32_control_construct (object_type, parent, win32_label_create, text);
    return self;
}

Win32Label* win32_label_new (Win32Window* parent, const char * text)
{
    return win32_label_construct (WIN32_TYPE_LABEL, parent, text);
}


/* INTERNAL CREATE
------------------------------------------- */
HWND win32_label_create (Win32Window *self, Win32Window *parent)
{
    HWND hwnd;
    Win32Window  *window  = (Win32Window*) self;
    Win32Control *control = (Win32Control*) self;
    Win32Label   *label   = (Win32Label* ) self;

    control->id = win32_control_generate_ID();
    wchar_t *text = fromUTF8(window->text);

    DWORD style; // Default is SS_LEFT = 0
    switch (label->text_align){
        case ALIGN_CENTER:
            style = SS_CENTER;
            break;
        case ALIGN_RIGHT:
            style = SS_RIGHT;
            break;
        default:
        case ALIGN_LEFT:
            style = SS_LEFT | SS_LEFTNOWORDWRAP;
            break;
    }

    // resize label when text is changed
    window->auto_resize = TRUE;

    // Creating the Window
    hwnd = CreateWindow(
        L"STATIC",
        text,
        // Control Styles:
        WS_VISIBLE | WS_CHILD | SS_NOPREFIX | style,
        window->left,   // x position
        window->top,    // y position
        window->pref_width,  // width
        window->pref_height, // height
        parent->hwnd,   // Parent window
        (HANDLE) control->id, // Control ID
        window->hInstance,
        NULL );

    window->hwnd = hwnd;

    g_controlProc = (WNDPROC) SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR) WndProc );

    // resize to fit the contents of the label
    win32_label_auto_resize (window, text);

    free (text);

    return hwnd;
}


/* PROPERTY SET TEXT-ALIGNMENT
------------------------------------------- */
void  win32_label_set_text_align  (Win32Label *instance, int alignment)
{
    Win32Window *window = (Win32Window*) instance;

    /* NOOP */
    if (instance->text_align == alignment) return;
    /* RANGE CHECK */
    if (alignment < ALIGN_LEFT || alignment > ALIGN_RIGHT) return;

    instance->text_align = (int) alignment;

    if (window->hwnd != NULL)
    {
        DWORD new_style; // Default is SS_LEFT = 0
        LONG_PTR  style;

        switch (alignment){
            case ALIGN_CENTER:
                new_style = SS_CENTER;
                break;
            case ALIGN_RIGHT:
                new_style = SS_RIGHT;
                break;
            default:
            case ALIGN_LEFT:
                new_style = SS_LEFT | SS_LEFTNOWORDWRAP;
                break;
        }
        style = GetWindowLongPtr( window->hwnd, GWL_STYLE );
        // Clear the previous alignment style then apply the new one
        style = ( style & ~(SS_LEFT | SS_CENTER | SS_RIGHT | SS_LEFTNOWORDWRAP) ) | new_style;
        SetWindowLongPtr( window->hwnd, GWL_STYLE, style );
        InvalidateRect( window->hwnd, NULL, TRUE );
    }
}


/* PROPERTY GET TEXT-ALIGNMENT
------------------------------------------- */
int   win32_label_get_text_align  (Win32Label *instance)
{
    return instance->text_align;
}


/* INTERNAL VIRTUAL PROCEDURE
------------------------------------------- */
void win32_label_auto_resize (Win32Window *window, const void *data)
{
    // respect the user-provided lengths
    if ( window->pref_width != 0 && window->pref_height != 0 ) return;
    if (!window->hwnd) return;

    SIZE size;
    wchar_t *text = (wchar_t*) data;
    int length = lstrlen(text);

    HDC context = GetDC(window->hwnd);
    if ( length == 0 ) GetTextExtentPoint32 (context, L"Dummy", 5, &size );
    else GetTextExtentPoint32 (context, text, length, &size );
    ReleaseDC (window->hwnd, context);

    if (window->pref_width)  size.cx = window->pref_width;
    if (window->pref_height) size.cy = window->pref_height;

    window->width = size.cx;
    window->height = size.cy;

    win32_window_resize (window, size.cx, size.cy);
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
static void win32_label_class_init (Win32LabelClass * klass, gpointer klass_data)
{
    win32_label_parent_class = g_type_class_peek_parent (klass);
    ((Win32WindowClass *) klass)->finalize = win32_label_finalize;
    ((Win32WindowClass *) klass)->auto_resize = win32_label_auto_resize;
}

static void win32_label_instance_init (Win32Label * self, gpointer klass)
{
}

/* INTERNAL CLEANUP
------------------------------------------- */
static void win32_label_finalize (Win32Window * obj)
{
    Win32Label * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, WIN32_TYPE_LABEL, Win32Label);
    WIN32_WINDOW_CLASS (win32_label_parent_class)->finalize (obj);
}


/* INTERNAL GTYPE REGISTRATION
------------------------------------------- */
static GType win32_label_get_type_once (void)
{
    static const GTypeInfo g_define_type_info = {
        sizeof (Win32LabelClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) win32_label_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (Win32Label),
        0,
        (GInstanceInitFunc) win32_label_instance_init,
        NULL
    };
    GType win32_label_type_id;
    win32_label_type_id = g_type_register_static (WIN32_TYPE_CONTROL, "Win32Label", &g_define_type_info, 0);
    return win32_label_type_id;
}

GType win32_label_get_type (void)
{
    static volatile gsize win32_label_type_id__volatile = 0;
    if (g_once_init_enter (&win32_label_type_id__volatile)) {
        GType win32_label_type_id;
        win32_label_type_id = win32_label_get_type_once ();
        g_once_init_leave (&win32_label_type_id__volatile, win32_label_type_id);
    }
    return win32_label_type_id__volatile;
}
