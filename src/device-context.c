/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"
#include <stdio.h>

void win32_device_context_text_out(HDC hdc, int x, int y, const char * text)
{
    wchar_t* _text = fromUTF8( text );
    size_t length = wcslen( _text );
    TextOut( hdc, x, y, _text, length );
    free (_text);
}
