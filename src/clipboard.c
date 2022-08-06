/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"
#include <winuser.h>

static void add_format_listener_callback ( Win32Event *event, void *boundData );

/* CLIPBOARD SET TEXT
------------------------------------------- */
void win32_clipboard_set_text(const char* text)
{
    HGLOBAL hmem; // Handle to a moveable memory block
    wchar_t *buffer;

    if (!OpenClipboard(NULL)) return;
    EmptyClipboard();

    int length = MultiByteToWideChar(CP_UTF8, 0, text, -1, 0, 0);

    // Allocate a global memory object for the text.
    hmem = GlobalAlloc(GMEM_MOVEABLE, (length) * sizeof(wchar_t));

    if (hmem == NULL){
        CloseClipboard();
        return;
    }

    // Lock the handle and copy the text to the buffer.
    buffer = GlobalLock(hmem);
    MultiByteToWideChar(CP_UTF8, 0, text, -1, buffer, length);
    GlobalUnlock(hmem);

    // Place the handle on the clipboard.
    SetClipboardData(CF_UNICODETEXT, hmem);
    // Close the clipboard.
    CloseClipboard();
}


/* CLIPBOARD GET TEXT
------------------------------------------- */
char* win32_clipboard_get_text(void)
{
    HGLOBAL hmem; // Handle to a moveable memory block
    const wchar_t *clipboardData;
    char *text = NULL;

    if ( !IsClipboardFormatAvailable(CF_UNICODETEXT) ) return NULL;
    if ( !OpenClipboard(NULL) ) return NULL;

    hmem = GetClipboardData(CF_UNICODETEXT);
    if (hmem != NULL)
    {
        clipboardData = GlobalLock(hmem);
        if (clipboardData != NULL)
        {
            text = toUTF8( clipboardData );
            GlobalUnlock(hmem);
        }
    }
    CloseClipboard();

    return text;
}


/* STATIC METHOD
------------------------------------------- */
BOOL win32_clipboard_add_format_listener (Win32Window *window)
{
    if (window->hwnd) return AddClipboardFormatListener(window->hwnd);
    win32_window_insert_into_callback_queue( window, WM_CREATE, add_format_listener_callback, NULL, NULL );
    return FALSE;
}

static void add_format_listener_callback ( Win32Event *event, void *boundData )
{
    Win32Window *window = event->source;
    AddClipboardFormatListener(window->hwnd);
}

