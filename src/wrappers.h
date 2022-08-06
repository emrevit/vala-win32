/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef WIN32_WRAPPERS_H
#define WIN32_WRAPPERS_H

#include <windows.h>
#include <glib-object.h>
#include <glib.h>

/* STRUCT WRAPPER RECT (REF COUNTED)
------------------------------------------- */
typedef struct _Win32Rect {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
    volatile int ref_count;
} Win32Rect;

void* win32_rect_ref   (void*);
void  win32_rect_unref (void*);

#endif


LRESULT win32_send_message(Win32Window* window, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL win32_post_message(Win32Window* window, UINT msg, WPARAM wParam, LPARAM lParam);