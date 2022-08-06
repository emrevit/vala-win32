/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"

static const wchar_t *szClassName = L"WindowClass";
static gpointer win32_application_window_parent_class = NULL;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void win32_application_window_finalize (Win32Window * obj);
static GType win32_application_window_get_type_once (void);


/* CONSTRUCTOR
------------------------------------------- */
Win32ApplicationWindow* win32_application_window_construct (GType object_type, const char * title)
{
    Win32ApplicationWindow* self = NULL;
    self = (Win32ApplicationWindow*) win32_container_construct (object_type);
    Win32Window *window = (Win32Window*) self;

    if ( title != NULL ){
        window->text  = _strdup (title);
    }

    // Defaults
    window->left = CW_USEDEFAULT;    // initial x position
    window->top  = CW_USEDEFAULT;    // initial y position
    window->width  = CW_USEDEFAULT;    // initial x size
    window->height = CW_USEDEFAULT;    // initial y size

    return self;
}

Win32ApplicationWindow* win32_application_window_new (const char * title)
{
    return win32_application_window_construct (WIN32_TYPE_APPLICATION_WINDOW, title);
}


/* INTERNAL CREATE
------------------------------------------- */
void win32_application_window_create (Win32ApplicationWindow *self)
{
    HWND hwnd;
    Win32Window * window = (Win32Window*) self;
    wchar_t *text = fromUTF8(window->text);

    // Creating the Window
    hwnd = CreateWindowEx(
        0, // WS_EX_CLIENTEDGE,
        szClassName,
        text,
        WS_OVERLAPPEDWINDOW,
        window->left,
        window->top,
        window->pref_width,
        window->pref_height,
        NULL, NULL,
        window->hInstance,
        window );

    free(text);

    if(hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        exit (1);  //exit
    }
}


/* METHOD SHOW
------------------------------------------- */
void win32_application_window_show(Win32ApplicationWindow *self)
{
    Win32Window * window = (Win32Window*) self;
    Win32Container * container = (Win32Container*) self;

    if ( window->hwnd == NULL ){
        // Update layout if there is any
        if (container->layout) container->layout->configure (container);
        // Create underlying window object
        win32_application_window_create( self );
    }
    ShowWindow (window->hwnd, SW_NORMAL);
    UpdateWindow (window->hwnd);
}


/* PROPERTY SET MIN-WIDTH
------------------------------------------- */
void win32_application_window_set_min_width  (Win32ApplicationWindow *self, INT min_width)
{
    self->min_width = min_width;
}

/* PROPERTY GET MIN-WIDTH
------------------------------------------- */
INT  win32_application_window_get_min_width  (Win32ApplicationWindow *self)
{
    return self->min_width;
}

/* PROPERTY SET MIN-HEIGHT
------------------------------------------- */
void win32_application_window_set_min_height (Win32ApplicationWindow *self, INT min_height)
{
    self->min_height = min_height;
}

/* PROPERTY GET MIN-HEIGHT
------------------------------------------- */
INT  win32_application_window_get_min_height (Win32ApplicationWindow *self)
{
    return self->min_height;
}

/* PROPERTY SET MAX-WIDTH
------------------------------------------- */
void win32_application_window_set_max_width  (Win32ApplicationWindow *self, INT max_width)
{
    self->max_width = max_width;
}

/* PROPERTY GET MAX-WIDTH
------------------------------------------- */
INT  win32_application_window_get_max_width  (Win32ApplicationWindow *self)
{
    return self->max_width;
}

/* PROPERTY SET MAX-HEIGHT
------------------------------------------- */
void win32_application_window_set_max_height (Win32ApplicationWindow *self, INT max_height)
{
    self->max_height = max_height;
}

/* PROPERTY GET MAX-HEIGHT
------------------------------------------- */
INT  win32_application_window_get_max_height (Win32ApplicationWindow *self)
{
    return self->max_height;
}


/* INTERNAL WINDOW PROCEDURE
------------------------------------------- */
// the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static Win32ApplicationWindow *applicationWindow = NULL;
    if (!applicationWindow) applicationWindow = (Win32ApplicationWindow*) GetWindowLongPtr( hwnd, GWLP_USERDATA);

    LRESULT result;
    result = win32_window_default_procedure(hwnd, msg, wParam, lParam);
    if ( result == STOP_PROPAGATION ) return 0;

    static HBRUSH hbrush = NULL;

    switch (msg)
    {
        case WM_COMMAND:
            // Forward message
            if ( (HWND) lParam != NULL ) SendMessage( (HWND) lParam, FM_COMMAND, 0, 0 );
            return 0;

        case WM_GETMINMAXINFO: {//window's size/position is about to change
            if (!applicationWindow) return 0;
            // lParam is a pointer to MINMAXINFO structure
            LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
            if (applicationWindow->min_width  > 0 ) lpMMI->ptMinTrackSize.x = applicationWindow->min_width;
            if (applicationWindow->min_height > 0 ) lpMMI->ptMinTrackSize.y = applicationWindow->min_height;
            if (applicationWindow->max_width  > applicationWindow->min_width ) lpMMI->ptMaxTrackSize.x = applicationWindow->max_width;
            if (applicationWindow->max_height > applicationWindow->min_height) lpMMI->ptMaxTrackSize.y = applicationWindow->max_height;
            return 0; }

        case WM_SIZE: {
            // Layout children
            Win32Container *container = (Win32Container *) applicationWindow;
            if ( container->layout != NULL) container->layout->recalculate (container);
            return 0; }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}


/* INTERNAL GTYPE
------------------------------------------- */
static void win32_application_window_class_init (Win32ApplicationWindowClass * klass, gpointer klass_data)
{
    win32_application_window_parent_class = g_type_class_peek_parent (klass);
    ((Win32WindowClass *) klass)->finalize = win32_application_window_finalize;

    WNDCLASSEX wndclass;
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Check if the class is already registered in a previous call
    if ( !GetClassInfoEx( hInstance, szClassName, &wndclass) ){
        // Register the Window Class
        wndclass.cbSize        = sizeof(WNDCLASSEX);
        wndclass.style         = 0;
        wndclass.lpfnWndProc   = WndProc;
        wndclass.cbClsExtra    = 0;
        wndclass.cbWndExtra    = 0;
        wndclass.hInstance     = hInstance;
        wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
        wndclass.lpszMenuName  = NULL;
        wndclass.lpszClassName = szClassName;
        wndclass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

        if(!RegisterClassEx(&wndclass))
        {
            MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
            exit (1); // exit
        }
    }
}


static void win32_application_window_instance_init (Win32ApplicationWindow * self, gpointer klass)
{
}

/* INTERNAL CLEANUP
------------------------------------------- */
static void win32_application_window_finalize (Win32Window * obj)
{
    Win32ApplicationWindow * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, WIN32_TYPE_APPLICATION_WINDOW, Win32ApplicationWindow);
    WIN32_WINDOW_CLASS (win32_application_window_parent_class)->finalize (obj);
}


/* INTERNAL GTYPE REGISTRATION
------------------------------------------- */
static GType win32_application_window_get_type_once (void)
{
    static const GTypeInfo g_define_type_info = {
        sizeof (Win32ApplicationWindowClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) win32_application_window_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (Win32ApplicationWindow),
        0,
        (GInstanceInitFunc) win32_application_window_instance_init,
        NULL
    };
    GType win32_application_window_type_id;
    win32_application_window_type_id = g_type_register_static (WIN32_TYPE_CONTAINER, "Win32ApplicationWindow", &g_define_type_info, 0);
    return win32_application_window_type_id;
}


GType win32_application_window_get_type (void)
{
    static volatile gsize win32_application_window_type_id__volatile = 0;
    if (g_once_init_enter (&win32_application_window_type_id__volatile)) {
        GType win32_application_window_type_id;
        win32_application_window_type_id = win32_application_window_get_type_once ();
        g_once_init_leave (&win32_application_window_type_id__volatile, win32_application_window_type_id);
    }
    return win32_application_window_type_id__volatile;
}


GType win32_application_window_get_type1 (void* obj)
{
    return win32_application_window_get_type ();
}

