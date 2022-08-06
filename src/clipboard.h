/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef _WIN32_CLIPBOARD_H_
#define _WIN32_CLIPBOARD_H_

#include <windows.h>
#include <glib-object.h>

void win32_clipboard_set_text(const char* text);

char* win32_clipboard_get_text(void);

BOOL win32_clipboard_add_format_listener (Win32Window *window);

#endif
