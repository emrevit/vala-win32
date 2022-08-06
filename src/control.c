/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"
#include <stdio.h>

static gpointer win32_control_parent_class = NULL;

static void creation_callback ( Win32Event *event, void *boundData );
static void win32_control_finalize (Win32Window * obj);
static GType win32_control_get_type_once (void);


/* CONSTRUCTOR
------------------------------------------- */
Win32Control* win32_control_construct (GType object_type,
                                       Win32Window *parent,
                                       Win32WindowCreator create_window,
                                       const char *text )
{
    Win32Control* self = NULL;
    self = (Win32Control*) win32_window_construct (object_type);
    Win32Window * window = (Win32Window*) self;

    window->parent = parent;

    if ( text != NULL ){
        window->text  = _strdup (text);
    }

    if ( parent->hwnd != NULL ){
        create_window ((Win32Window*) self, parent);
    } else {
        // postpone control creation until parent window is constructed
        Win32CreationData *data = malloc( sizeof(Win32CreationData) );
        data->create_window = create_window;
        data->control = self;
        win32_window_insert_into_callback_queue( parent, WM_CREATE, creation_callback, data, NULL );
    }

    win32_container_add_child ((Win32Container*) parent, (Win32Window*) self);

    return self;
}


/* INTERNAL DELAYED CREATION
------------------------------------------- */
void creation_callback ( Win32Event *event, void *boundData )
{
    Win32CreationData * data = (Win32CreationData*) boundData;
    Win32WindowCreator create_window = data->create_window;
    Win32Control *control = data->control;
    Win32Window *window = (Win32Window*) control;

    // window->hwnd is assigned to hwnd inside the create_window function;
    HWND hwnd = create_window ((Win32Window*) control, event->source );

    // Disable control
    if (!window->enabled) EnableWindow(hwnd, FALSE);

    // Change the font used to the default gui font
    HFONT defaultFont = win32_get_default_gui_font();
    SendMessage(hwnd, WM_SETFONT, (LPARAM) defaultFont, TRUE);

    // by the time we override the control procedure,
    // the WM_NCCREATE message has already been processed.
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) window );

    // release CreationData
    free (data);
}


/* PROPERTY GET ID
------------------------------------------- */
UINT control_get_id (Win32Control *self)
{
    return self->id;
}


/* INTERNAL GTYPE
------------------------------------------- */
static void win32_control_class_init (Win32ControlClass * klass, gpointer klass_data)
{
    win32_control_parent_class = g_type_class_peek_parent (klass);
    ((Win32WindowClass *) klass)->finalize = win32_control_finalize;
}

static void win32_control_instance_init (Win32Control * self,  gpointer klass)
{
}


/* INTERNAL CLEANUP
------------------------------------------- */
static void win32_control_finalize (Win32Window * obj)
{
    Win32Control * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, WIN32_TYPE_CONTROL, Win32Control);

    WIN32_WINDOW_CLASS (win32_control_parent_class)->finalize (obj);
}


/* INTERNAL GTYPE REGISTRATION
------------------------------------------- */
static GType win32_control_get_type_once (void)
{
    static const GTypeInfo g_define_type_info = {
        sizeof (Win32ControlClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) win32_control_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (Win32Control),
        0,
        (GInstanceInitFunc) win32_control_instance_init,
        NULL
    };
    GType win32_control_type_id;
    win32_control_type_id = g_type_register_static (WIN32_TYPE_WINDOW, "Win32Control", &g_define_type_info, G_TYPE_FLAG_ABSTRACT);
    return win32_control_type_id;
}

GType win32_control_get_type (void)
{
    static volatile gsize win32_control_type_id__volatile = 0;
    if (g_once_init_enter (&win32_control_type_id__volatile)) {
        GType win32_control_type_id;
        win32_control_type_id = win32_control_get_type_once ();
        g_once_init_leave (&win32_control_type_id__volatile, win32_control_type_id);
    }
    return win32_control_type_id__volatile;
}


/* UTILITY
------------------------------------------- */
unsigned int win32_control_generate_ID(void){
    static unsigned int id = 4000;
    return id += 1;
}
