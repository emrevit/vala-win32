/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"

static gpointer win32_window_parent_class = NULL;
static void win32_window_auto_resize_default (Win32Window *self, const void *data);
static void  win32_window_finalize (Win32Window * obj);
static GType win32_window_get_type_once (void);


/* METHOD
------------------------------------------- */
int win32_window_add_listener( Win32Window *self,
                               UINT eventID,
                               Win32Callback callback,
                               void *boundData,
                               Win32ReleaseFunction releaseData )
{
    return WIN32_WINDOW_GET_CLASS (self)->add_listener (self, eventID, callback, boundData, releaseData);
}


/* INTERNAL INSERT CALLBACK
------------------------------------------- */
int win32_window_insert_into_callback_queue ( Win32Window *self,
                                              UINT eventID,
                                              Win32Callback callback,
                                              void *boundData,
                                              Win32ReleaseFunction releaseData )
{
    Win32Window * window = (Win32Window*) self;
    Win32EventList * eventList = &window->attachedEvents;
    size_t num_ev = eventList->length;
    // Check if we have enough room in the list
    if ( (num_ev + 1) % INITIAL_LIST_SIZE == 0 ){
        eventList->items = realloc(eventList->items, sizeof(Win32EventListItem) * ((num_ev + 1) + INITIAL_LIST_SIZE) );
        // Initialize newly added slots
        memset( eventList->items + (num_ev + 1), 0, sizeof(Win32EventListItem) * INITIAL_LIST_SIZE );
    }

    Win32EventListItem *events = eventList->items;

    // Find the event's index in the list.
    // If it's not already in the list, it'll be appended to the list
    int i = 0;
    while ( events[i].eventID != WM_NULL && events[i].eventID != eventID ) i++;

    if ( events[i].callbacks == NULL ){
        events[i].callbacks = malloc( sizeof(Win32Callback) * INITIAL_QUEUE_SIZE );
        memset( events[i].callbacks, 0, sizeof(Win32Callback) * INITIAL_QUEUE_SIZE );
        // Check if memory allocation fails
        events[i].boundData = malloc( sizeof(void*) * INITIAL_QUEUE_SIZE );
        memset( events[i].boundData, 0, sizeof(void*) * INITIAL_QUEUE_SIZE );

        events[i].releaseData = malloc( sizeof(void*) * INITIAL_QUEUE_SIZE );
        memset( events[i].releaseData, 0, sizeof(void*) * INITIAL_QUEUE_SIZE );
    }

    size_t num_cb = events[i].numCallbacks;

    // Check if we have enough room in the callback queue
    if ( (num_cb + 1) % INITIAL_QUEUE_SIZE == 0 ){
        events[i].callbacks = realloc(events[i].callbacks, sizeof(Win32Callback) * ((num_cb + 1) + INITIAL_QUEUE_SIZE) );
        events[i].boundData = realloc(events[i].boundData, sizeof(void*) * ((num_cb + 1) + INITIAL_QUEUE_SIZE) );
        events[i].releaseData = realloc(events[i].releaseData, sizeof(void*) * ((num_cb + 1) + INITIAL_QUEUE_SIZE) );
        // Initialize newly added slots
        memset( events[i].callbacks + (num_cb + 1), 0, sizeof(Win32Callback) * INITIAL_QUEUE_SIZE );
        memset( events[i].boundData + (num_cb + 1), 0, sizeof(void*) * INITIAL_QUEUE_SIZE );
        memset( events[i].releaseData + (num_cb + 1), 0, sizeof(void*) * INITIAL_QUEUE_SIZE );
    }

    // We're adding a new event to the list so increase the item count
    if (events[i].eventID == 0 ){
        events[i].eventID = eventID;
        eventList->length += 1;
    }

    events[i].callbacks[num_cb] = callback;
    events[i].boundData[num_cb] = boundData;
    events[i].releaseData[num_cb] = releaseData;
    // increase callback number
    events[i].numCallbacks += 1;
    return TRUE;
}


/* INTERNAL INVOKE CALLBACK
------------------------------------------- */
LRESULT invoke_callback (Win32Window *window, UINT msg, WPARAM wParam, LPARAM lParam, Win32Callback callback, void * boundData)
{
    Win32Event event;
    event.id = msg;
    event.source  = window;
    event.wParam  = wParam;
    event.lParam  = lParam;
    event.handled = 0; // Hand over the event to the default window procedure after processing it

    callback( &event, boundData );

    if ( event.handled == 1 ) return STOP_PROPAGATION;
    else return 0;
}


/* INTERNAL DEFAULT WINDOW PROCEDURE
------------------------------------------- */
LRESULT win32_window_default_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Win32Window *window = (Win32Window *) GetWindowLongPtr( hwnd, GWLP_USERDATA);
    Win32EventList  *eventList = NULL;
    Win32EventListItem *events = NULL;
    // [!] Assignment is intentional
    if ( window != NULL ){
        eventList = &window->attachedEvents;
        events = eventList->items;
    }

    if ( msg == WM_NCCREATE ){
        //attach the event list to the window
        window = ((CREATESTRUCT*) lParam)->lpCreateParams;
        window->hwnd = hwnd;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) window);
    } else if ( msg == WM_DESTROY ){
        // Clear event list
        Win32ReleaseFunction bound_data_unref;

        if (events != NULL){
            int i = 0, n;
            // events array is a null terminated list
            while ( events[i].eventID != WM_NULL ){
                // callbacks array is a null terminated list
                n = 0;
                while ( events[i].callbacks[n] != NULL ){
                    if ( events[i].releaseData[n] ){
                        bound_data_unref = events[i].releaseData[n];
                        bound_data_unref( events[i].boundData[n] );
                    }
                    n += 1;
                }
                free(events[i].callbacks);
                free(events[i].boundData);
                free(events[i].releaseData);
                i += 1;
            }
            memset( eventList->items, 0, sizeof(Win32EventListItem) * eventList->length );
            eventList->length = 0;
        }
    }// END IF

    LRESULT result = 0;
    // To prevent memory access issues before the list is initialized
    if (events != NULL){
        int i = 0, n = 0;
        // events array is a null terminated list
        while ( events[i].eventID != WM_NULL ){
            // Check if the event has a registered callback
            if ( events[i].eventID == msg){
                // callbacks array is a null terminated list
                while ( events[i].callbacks[n] != NULL ){
                    result = invoke_callback( window, msg, wParam, lParam, events[i].callbacks[n], events[i].boundData[n]);
                    n += 1;
                }
                break; // No need to check further events in the list
            }
            i += 1;
        }
    }// END IF
    return result;
}


/* PROPERTY SET LAYOUT DATA
------------------------------------------- */
void win32_window_set_positioning (Win32Window *instance, Win32LayoutData* layoutData)
{
    if ( instance->positioning != NULL ){
        if (instance->positioning == layoutData ) return;
        // decrease the reference count of the existing anchor
        win32_layout_data_unref( instance->positioning );
    }

    // increase reference count
    instance->positioning = win32_layout_data_ref( layoutData );
}


/* PROPERTY GET LAYOUT DATA
------------------------------------------- */
Win32LayoutData*  win32_window_get_positioning (Win32Window *window)
{
    return window->positioning;
}


/* PROPERTY SET LEFT
------------------------------------------- */
void  win32_window_set_left (Win32Window *window, int left)
{
    window->left = left;

    if (window->hwnd != NULL){
        RECT rect;
        GetClientRect( window->hwnd, &rect );
        MapWindowPoints( window->hwnd, GetParent(window->hwnd), (LPPOINT) &rect, 2);

        int height = rect.bottom - rect.top;
        int width  = rect.right - rect.left;

        MoveWindow( window->hwnd, left, rect.top, width, height, /*REPAINT*/ TRUE );
    }
}


/* PROPERTY GET LEFT
------------------------------------------- */
int   win32_window_get_left (Win32Window *window)
{
    if (window->hwnd != NULL){
        RECT rect;
        GetClientRect( window->hwnd, &rect );
        MapWindowPoints( window->hwnd, GetParent(window->hwnd), (LPPOINT) &rect, 2);

        window->left = rect.left;
    }
    return window->left;
}


/* PROPERTY SET TOP
------------------------------------------- */
void  win32_window_set_top (Win32Window *window, int top)
{
    window->top = top;

    if (window->hwnd != NULL){
        RECT rect;
        GetClientRect( window->hwnd, &rect );
        MapWindowPoints( window->hwnd, GetParent(window->hwnd), (LPPOINT) &rect, 2);

        int height = rect.bottom - rect.top;
        int width  = rect.right - rect.left;

        MoveWindow( window->hwnd, rect.left, top, width, height, /*REPAINT*/ TRUE );
    }
}


/* PROPERTY GET TOP
------------------------------------------- */
int   win32_window_get_top (Win32Window *window)
{
    if (window->hwnd != NULL){
        RECT rect;
        GetClientRect( window->hwnd, &rect );
        MapWindowPoints( window->hwnd, GetParent(window->hwnd), (LPPOINT) &rect, 2);
        window->top = rect.top;
    }
    return window->top;
}


/* PROPERTY SET WIDTH
------------------------------------------- */
void  win32_window_set_width (Win32Window *window, int width)
{
    window->pref_width = width;

    if (window->hwnd != NULL){
        RECT rect;
        GetClientRect( window->hwnd, &rect );
        MapWindowPoints( window->hwnd, GetParent(window->hwnd), (LPPOINT) &rect, 2);

        int height = rect.bottom - rect.top;

        MoveWindow( window->hwnd, rect.left, rect.top, width, height, /*REPAINT*/ TRUE );
    }
}


/* PROPERTY GET WIDTH
------------------------------------------- */
int   win32_window_get_width (Win32Window *window)
{
    if (window->hwnd != NULL){
        RECT rect;
        GetClientRect( window->hwnd, &rect );
        MapWindowPoints( window->hwnd, GetParent(window->hwnd), (LPPOINT) &rect, 2);

        int width = rect.right - rect.left;
        return window->width = width;
    }
    return window->pref_width;
}


/* PROPERTY SET HEIGHT
------------------------------------------- */
void  win32_window_set_height (Win32Window *window, int height)
{
    window->pref_height = height;

    if (window->hwnd != NULL){
        RECT rect;
        GetClientRect( window->hwnd, &rect );
        MapWindowPoints( window->hwnd, GetParent(window->hwnd), (LPPOINT) &rect, 2);

        int width = rect.right - rect.left;

        MoveWindow( window->hwnd, rect.left, rect.top, width, height, /*REPAINT*/ TRUE );
    }
}


/* PROPERTY GET HEIGHT
------------------------------------------- */
int   win32_window_get_height (Win32Window *window)
{
    if (window->hwnd != NULL){
        RECT rect;
        GetClientRect( window->hwnd, &rect );
        MapWindowPoints( window->hwnd, GetParent(window->hwnd), (LPPOINT) &rect, 2);

        int height = rect.bottom - rect.top;
        return window->height = height;
    }
    return window->pref_height;
}


/* PROPERTY SET TEXT
------------------------------------------- */
void  win32_window_set_text (Win32Window *window, const char* text)
{
    char* tmp0;
    wchar_t* tmp1;
    tmp0 = window->text;
    window->text = _strdup (text);
    tmp1 = fromUTF8( window->text );

    if (window->hwnd != NULL){
        SetWindowText( window->hwnd, tmp1);
        if (window->auto_resize){
            WIN32_WINDOW_GET_CLASS(window)->auto_resize(window, tmp1);
        }
    }
    free (tmp1);
    free (tmp0);
}


/* PROPERTY GET TEXT
------------------------------------------- */
char* win32_window_get_text (Win32Window *window)
{
    if (window->hwnd == NULL) return window->text;

    int bufferSize = GetWindowTextLength(window->hwnd);

    if ( bufferSize == 0 ) return ""; //NULL;

    wchar_t * buffer = malloc( sizeof(wchar_t) * (bufferSize+1) );
    memset(buffer, 0, sizeof(wchar_t) * (bufferSize+1) );
    GetWindowText(window->hwnd, buffer, bufferSize+1);

    char * tmp0 = window->text;
    window->text = toUTF8( buffer );

    free (tmp0);
    free (buffer);

    return window->text;
}


/* PROPERTY SET ENABLED
------------------------------------------- */
void  win32_window_set_enabled (Win32Window *self, BOOL isEnabled)
{
    self->enabled = isEnabled;

    if (self->hwnd != NULL){
        EnableWindow(self->hwnd, isEnabled);
    }
}


/* PROPERTY GET ENABLED
------------------------------------------- */
BOOL  win32_window_get_enabled (Win32Window *self)
{
    if (self->hwnd != NULL){
        self->enabled = IsWindowEnabled(self->hwnd);
    }
    return self->enabled;
}


/* METHOD
------------------------------------------- */
void win32_window_move (Win32Window *window, int left, int top)
{
    window->left = left;
    window->top  = top;

    if (window->hwnd != NULL){
        RECT rect;
        GetWindowRect( window->hwnd, &rect);

        int height = rect.bottom - rect.top;
        int width  = rect.right - rect.left;

        MoveWindow( window->hwnd, left, top, width, height, /*REPAINT*/ TRUE );
    }
}


/* METHOD
------------------------------------------- */
void win32_window_resize (Win32Window *window, int width, int height)
{
    window->width  = width;
    window->height = height;

    if (window->hwnd != NULL){
        RECT rect;
        GetClientRect( window->hwnd, &rect);
        MapWindowPoints( window->hwnd, GetParent(window->hwnd), (LPPOINT) &rect, 2);
        MoveWindow( window->hwnd, rect.left, rect.top, width, height, /*REPAINT*/ TRUE );
    }
}


/* METHOD
------------------------------------------- */
void win32_window_move_and_resize  (Win32Window *window, int left, int top, int width, int height)
{
    window->left = left;
    window->top  = top;
    window->width  = width;
    window->height = height;

    if (window->hwnd != NULL){
        RECT *rect = malloc( sizeof(RECT) );
        GetWindowRect( window->hwnd, rect);
        MoveWindow( window->hwnd, left, top, width, height, /*REPAINT*/ TRUE );
        free (rect);
    }
}


/* METHOD
------------------------------------------- */
HDC win32_window_begin_paint(Win32Window *window, PAINTSTRUCT *ps)
{
    return BeginPaint (window->hwnd, ps);
}


/* METHOD
------------------------------------------- */
void win32_window_end_paint(Win32Window *window, PAINTSTRUCT *ps)
{
    EndPaint (window->hwnd, ps);
}

/* METHOD
------------------------------------------- */
Win32Rect* win32_window_get_window_rect(Win32Window *window)
{
    Win32Rect *result = malloc( sizeof(Win32Rect) );
    memset( result, 0, sizeof(Win32Rect) );
    g_atomic_int_inc (&result->ref_count);
    if ( GetWindowRect( window->hwnd, (RECT*) result) ) return result;
    else return NULL;
}


/* METHOD
------------------------------------------- */
Win32Rect* win32_window_get_client_rect(Win32Window *window)
{
    Win32Rect *result = malloc( sizeof(Win32Rect) );
    memset( result, 0, sizeof(Win32Rect) );
    g_atomic_int_inc (&result->ref_count);
    if ( GetClientRect( window->hwnd, (RECT*) result) ) return result;
    else return NULL;
}


/* INTERNAL VIRTUAL METHOD
------------------------------------------- */
void win32_window_auto_resize_default (Win32Window *self, const void *data)
{
    // NO-OP
}


/* INTERNAL GTYPE
------------------------------------------- */
static void value_win32_window_init (GValue* value)
{
    value->data[0].v_pointer = NULL;
}

static void value_win32_window_free_value (GValue* value)
{
    if (value->data[0].v_pointer) {
        win32_window_unref (value->data[0].v_pointer);
    }
}

static void value_win32_window_copy_value (const GValue* src_value, GValue* dest_value)
{
    if (src_value->data[0].v_pointer) {
        dest_value->data[0].v_pointer = win32_window_ref (src_value->data[0].v_pointer);
    } else {
        dest_value->data[0].v_pointer = NULL;
    }
}

gpointer value_get_win32_window (const GValue* value)
{
    g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, WIN32_TYPE_WINDOW), NULL);
    return value->data[0].v_pointer;
}

void value_set_win32_window (GValue* value, gpointer v_object)
{
    Win32Window * old;
    g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, WIN32_TYPE_WINDOW));
    old = value->data[0].v_pointer;
    if (v_object) {
        g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, WIN32_TYPE_WINDOW));
        g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
        value->data[0].v_pointer = v_object;
        win32_window_ref (value->data[0].v_pointer);
    } else {
        value->data[0].v_pointer = NULL;
    }
    if (old) {
        win32_window_unref (old);
    }
}

void value_take_win32_window (GValue* value, gpointer v_object)
{
    Win32Window * old;
    g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, WIN32_TYPE_WINDOW));
    old = value->data[0].v_pointer;
    if (v_object) {
        g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, WIN32_TYPE_WINDOW));
        g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
        value->data[0].v_pointer = v_object;
    } else {
        value->data[0].v_pointer = NULL;
    }
    if (old) {
        win32_window_unref (old);
    }
}

static void win32_window_class_init (Win32WindowClass * klass,  gpointer klass_data)
{
    win32_window_parent_class = g_type_class_peek_parent (klass);
    ((Win32WindowClass *) klass)->finalize = win32_window_finalize;
    ((Win32WindowClass *) klass)->auto_resize = win32_window_auto_resize_default;
    ((Win32WindowClass *) klass)->add_listener = win32_window_insert_into_callback_queue;
}

static void win32_window_instance_init (Win32Window * self, gpointer klass)
{
    self->ref_count = 1;
}

static GType win32_window_get_type_once (void)
{
    static const GTypeValueTable g_define_type_value_table = {
        value_win32_window_init,
        value_win32_window_free_value,
        value_win32_window_copy_value,
        NULL, NULL, NULL, NULL, NULL
    };
    static const GTypeInfo g_define_type_info = {
        sizeof (Win32WindowClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) win32_window_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (Win32Window),
        0,
        (GInstanceInitFunc) win32_window_instance_init,
        &g_define_type_value_table
    };
    static const GTypeFundamentalInfo g_define_type_fundamental_info = {
        (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE)
    };

    GType win32_window_type_id;
    win32_window_type_id = g_type_register_fundamental (
        g_type_fundamental_next (),
        "Win32Window",
        &g_define_type_info,
        &g_define_type_fundamental_info,
        G_TYPE_FLAG_ABSTRACT
    );

    return win32_window_type_id;
}

GType win32_window_get_type (void)
{
    static volatile gsize win32_window_type_id__volatile = 0;
    if (g_once_init_enter (&win32_window_type_id__volatile)) {
        GType win32_window_type_id;
        win32_window_type_id = win32_window_get_type_once ();
        g_once_init_leave (&win32_window_type_id__volatile, win32_window_type_id);
    }
    return win32_window_type_id__volatile;
}

/* INTERNAL CONSTRUCT
------------------------------------------- */
Win32Window* win32_window_construct (GType object_type)
{
    Win32Window* self = NULL;
    self = (Win32Window*) g_type_create_instance (object_type);

    self->hInstance = GetModuleHandle(NULL);
    self->enabled   = TRUE;

    // Initialize callback list for the window
    Win32EventList * eventList = &self->attachedEvents;
    eventList->items = malloc( sizeof(Win32EventListItem) * INITIAL_LIST_SIZE );
    memset( eventList->items, 0, sizeof(Win32EventListItem) * INITIAL_LIST_SIZE );

    // Layout data
    //*TODO*/ allow different structures to be used for positioning
    self->positioning = malloc( sizeof(Win32LayoutData) );
    memset( self->positioning, 0, sizeof(Win32LayoutData) );
    win32_layout_data_ref (self->positioning);

    return self;
}


/* INTERNAL CLEANUP
------------------------------------------- */
static void win32_window_finalize (Win32Window * obj)
{
    Win32Window * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, WIN32_TYPE_WINDOW, Win32Window);

    // Underlying window may not be created yet
    if (self->hwnd != NULL) DestroyWindow (self->hwnd);

    if ( self->positioning != NULL ){
        if ( self->positioning->left != NULL ) win32_anchor_unref (self->positioning->left);
        if ( self->positioning->top  != NULL ) win32_anchor_unref (self->positioning->top);
        if ( self->positioning->right  != NULL ) win32_anchor_unref (self->positioning->right);
        if ( self->positioning->bottom != NULL ) win32_anchor_unref (self->positioning->bottom);
        win32_layout_data_unref (self->positioning);
    }
    // free event queue
    // Note: event lists are freed after WM_DESTROY message
    free (self->attachedEvents.items);

    free (self->text);
}


/* INTERNAL REF
------------------------------------------- */
void* win32_window_ref (void* instance)
{
    Win32Window * self = instance;
    g_atomic_int_inc (&self->ref_count);
    return instance;
}


/* INTERNAL UNREF
------------------------------------------- */
void win32_window_unref (gpointer instance)
{
    Win32Window * self = instance;
    if (g_atomic_int_dec_and_test (&self->ref_count)) {
        WIN32_WINDOW_GET_CLASS (self)->finalize (self);
        g_type_free_instance ((GTypeInstance *) self);
    }
}

/*┌────────────────────────────────────────────────────────┐
  │ EVENT OBJECT                                           │
  └────────────────────────────────────────────────────────┘*/

/* PROPERTY SET
------------------------------------------- */
BOOL win32_event_get_handled (Win32Event *event)
{
    return event->handled;
}


/* PROPERTY SET
------------------------------------------- */
void win32_event_set_handled (Win32Event *event, BOOL value)
{
    event->handled = value;
}
