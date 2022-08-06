/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef WIN32_DEVICE_CONTEXT_H
#define WIN32_DEVICE_CONTEXT_H

#include <windows.h>
#include <glib-object.h>
#include <glib.h>

/* CLASS DeviceContext (COMPACT)
------------------------------------------- */
void win32_device_context_text_out(HDC hdc, int x, int y, const char * text);

#endif







