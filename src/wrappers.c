/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"


LRESULT win32_send_message(Win32Window* window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return SendMessage(window->hwnd, msg, wParam, lParam);
}


BOOL win32_post_message(Win32Window* window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return PostMessage(window->hwnd, msg, wParam, lParam);
}


/* INTERNAL REF RECT
------------------------------------------- */
void* win32_rect_ref (void* instance)
{
    Win32Rect * self = instance;
    g_atomic_int_inc (&self->ref_count);
    return self;
}


/* INTERNAL UNREF RECT
------------------------------------------- */
void win32_rect_unref (void* instance)
{
    Win32Rect * self = instance;
    if (g_atomic_int_dec_and_test (&self->ref_count)) {
        free (self);
    }
}
