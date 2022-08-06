/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"

static gpointer win32_container_parent_class = NULL;

static void  win32_container_finalize (Win32Window * obj);
static GType win32_container_get_type_once (void);


/* CONSTRUCTOR
------------------------------------------- */
Win32Container* win32_container_construct (GType object_type)
{
    Win32Container* self = NULL;
    self = (Win32Container*) win32_window_construct (object_type);

    // Initialize children list
    self->childWindows.items = malloc( sizeof(Win32Window*) * INITIAL_LIST_SIZE );
    memset( self->childWindows.items, 0, sizeof(Win32Window*) * INITIAL_LIST_SIZE );
    self->childWindows.length = 0;

    return self;
}


/* METHOD GET CHILDREN
------------------------------------------- */
Win32Window ** win32_container_get_children (Win32Container *self, size_t *length )
{
    *length = self->childWindows.length;
    Win32Window ** children = malloc( sizeof(Win32Window*) * (*length+1) );

    for ( int i =0; i < (*length); i++ ){
        children[i] = win32_window_ref (self->childWindows.items[i]);
    }
    return children;
}


/* INTERNAL ADD CHILD
------------------------------------------- */
void win32_container_add_child (Win32Container *self, Win32Window *child)
{
    Win32Window * window = (Win32Window*) self;
    Win32WindowList *childList = &self->childWindows;
    size_t length = childList->length;

    // Check if we have enough room in the list
    if ( (length+1) % INITIAL_LIST_SIZE == 0 ){
        childList->items = realloc(childList->items, sizeof(Win32Window *) * ((length + 1) + INITIAL_LIST_SIZE) );
        // Initialize newly added slots
        memset( childList->items + (length + 1), 0, sizeof(Win32Window *) * INITIAL_LIST_SIZE );
    }
    childList->items[ length ] = win32_window_ref( child );
    childList->length += 1;

    // Update layout if there is any
    if (window->hwnd && self->layout) self->layout->configure(self);

}


/* PROPERTY SET LAYOUT
------------------------------------------- */
void win32_container_set_layout  (Win32Container *self, Win32Layout *layout)
{
    if ( self->layout != NULL ){
        if (self->layout == layout ) return;
        // decrease the reference count of the existing layout
        _win32_layout_unref0( self->layout );
    }

    // increase reference count
    win32_layout_ref( layout );

    self->layout = layout;
}


/* PROPERTY GET LAYOUT
------------------------------------------- */
Win32Layout *win32_container_get_layout  (Win32Container *self)
{
    return self->layout;
}


/* INTERNAL GTYPE
------------------------------------------- */
static void win32_container_class_init (Win32ContainerClass * klass, gpointer klass_data)
{
    win32_container_parent_class = g_type_class_peek_parent (klass);
    // Overrides
    ((Win32WindowClass *) klass)->finalize = win32_container_finalize;
}

static void win32_container_instance_init (Win32Container * self, gpointer klass)
{
}


/* INTERNAL CLEANUP
------------------------------------------- */
static void win32_container_finalize (Win32Window * obj)
{
    Win32Container * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, WIN32_TYPE_CONTAINER, Win32Container);

    // Unref children
    size_t length = self->childWindows.length;
    Win32Window ** children = self->childWindows.items;
    for ( int i =0; i < length; i++ ){
        _win32_window_unref0 (children[i]);
    }

    free (self->childWindows.items);
    WIN32_WINDOW_CLASS (win32_container_parent_class)->finalize (obj);
}


/* INTERNAL GTYPE REGISTRATION
------------------------------------------- */
static GType win32_container_get_type_once (void)
{
    static const GTypeInfo g_define_type_info = {
        sizeof (Win32ContainerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) win32_container_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (Win32Container),
        0,
        (GInstanceInitFunc) win32_container_instance_init,
        NULL
    };
    GType win32_container_type_id;
    win32_container_type_id = g_type_register_static (WIN32_TYPE_WINDOW, "Win32Container", &g_define_type_info, G_TYPE_FLAG_ABSTRACT);
    return win32_container_type_id;
}

GType win32_container_get_type (void)
{
    static volatile gsize win32_container_type_id__volatile = 0;
    if (g_once_init_enter (&win32_container_type_id__volatile)) {
        GType win32_container_type_id;
        win32_container_type_id = win32_container_get_type_once ();
        g_once_init_leave (&win32_container_type_id__volatile, win32_container_type_id);
    }
    return win32_container_type_id__volatile;
}
