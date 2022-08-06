/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef _WIN32_WINDOW_H_
#define _WIN32_WINDOW_H_

#include <windows.h>
#include <glib-object.h>

typedef struct _Win32Event Win32Event;
typedef struct _Win32EventList Win32EventList;

typedef struct _Win32WindowClass Win32WindowClass;

// Delegates
typedef void (*Win32Callback) ( Win32Event *event, void *boundData);
typedef void (*Win32ReleaseFunction) (void* userData);

/* EVENT Declarations
------------------------------------------- */
// Event Object
struct _Win32Event {
    UINT id;
    Win32Window *source;
    WPARAM wParam;
    LPARAM lParam;
    BOOL handled;
};

typedef struct _Win32EventListItem {
    UINT eventID;
    Win32Callback *callbacks;
    Win32ReleaseFunction *releaseData;
    void ** boundData;
    size_t numCallbacks;
} Win32EventListItem;

struct _Win32EventList {
    Win32EventListItem *items;
    size_t length;
};

#define  STOP_PROPAGATION  8000

BOOL win32_event_get_handled (Win32Event *event);
void win32_event_set_handled (Win32Event *event, BOOL value);

/* CLASS Window
------------------------------------------- */
#define WIN32_TYPE_WINDOW (win32_window_get_type ())
#define WIN32_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), WIN32_TYPE_WINDOW, Win32Window))
#define WIN32_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), WIN32_TYPE_WINDOW, Win32WindowClass))
#define WIN32_IS_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WIN32_TYPE_WINDOW))
#define WIN32_IS_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), WIN32_TYPE_WINDOW))
#define WIN32_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), WIN32_TYPE_WINDOW, Win32WindowClass))

#define _win32_window_unref0(var) ((var == NULL) ? NULL : (var = (win32_window_unref (var), NULL)))

struct _Win32Window {
    GTypeInstance parent_instance;
    volatile int ref_count;
    HWND hwnd;
    Win32Window* parent;
    HINSTANCE hInstance;
    char* text;
    BOOL enabled;
    INT top;
    INT left;
    INT width;
    INT height;
    Win32LayoutData* positioning;
    INT pref_width;
    INT pref_height;
    BOOL auto_resize;
    Win32EventList attachedEvents;
};

struct _Win32WindowClass {
    GTypeClass parent_class;
    void (*finalize) (Win32Window *self);
    void (*auto_resize) (Win32Window *window, const void *data);
    int  (*add_listener)(Win32Window *window, UINT eventID, Win32Callback callback, void * boundData, Win32ReleaseFunction releaseData);
};

Win32Window* win32_window_new (void);
Win32Window* win32_window_construct (GType object_type);

// Window* window_new (void);
int win32_window_add_listener (Win32Window *window,
                               UINT eventID,
                               Win32Callback callback,
                               void *boundData,
                               Win32ReleaseFunction releaseData);

/* PROPERTIES */
void win32_window_set_positioning (Win32Window *window, Win32LayoutData* layoutData);
Win32LayoutData* win32_window_get_positioning (Win32Window *window);

void  win32_window_set_text (Win32Window *window, const char* text);
char* win32_window_get_text (Win32Window *window);

void  win32_window_set_enabled (Win32Window *window, BOOL isEnabled);
BOOL  win32_window_get_enabled (Win32Window *window);

void  win32_window_set_top  (Win32Window *window, int top);
int   win32_window_get_top  (Win32Window *window);

void  win32_window_set_left (Win32Window *window, int left);
int   win32_window_get_left (Win32Window *window);

void  win32_window_set_width  (Win32Window *window, int width);
int   win32_window_get_width  (Win32Window *window);

void  win32_window_set_height (Win32Window *window, int height);
int   win32_window_get_height (Win32Window *window);

/* METHODS */
Win32Rect* win32_window_get_window_rect(Win32Window *window);
Win32Rect* win32_window_get_client_rect(Win32Window *window);

void win32_window_move   (Win32Window *window, int left, int top);
void win32_window_resize (Win32Window *window, int width, int height);
void win32_window_move_and_resize  (Win32Window *window, int left, int top, int width, int height);

HDC win32_window_begin_paint(Win32Window *window, PAINTSTRUCT *ps);
void win32_window_end_paint(Win32Window *window, PAINTSTRUCT *ps);

/* INTERNAL */
LRESULT win32_window_default_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL  win32_window_insert_into_callback_queue (Win32Window *window,
                                               UINT eventID,
                                               Win32Callback callback,
                                               void *boundData,
                                               Win32ReleaseFunction releaseData);

gpointer win32_window_ref   (gpointer instance);
void     win32_window_unref (gpointer instance);

void value_set_win32_window  (GValue* value, gpointer v_object);
void value_take_win32_window (GValue* value, gpointer v_object);
gpointer value_get_win32_window (const GValue* value);

GType win32_window_get_type (void) G_GNUC_CONST;
G_DEFINE_AUTOPTR_CLEANUP_FUNC (Win32Window, win32_window_unref)

#endif
