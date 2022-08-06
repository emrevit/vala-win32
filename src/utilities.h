/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef WIN32_UTILITIES_H
#define WIN32_UTILITIES_H

#include <windows.h>
#include <glib-object.h>
#include <glib.h>

wchar_t* fromUTF8 (const char* src);
char*    toUTF8   (const wchar_t* src);
HFONT win32_get_default_gui_font(void);

#endif
