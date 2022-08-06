/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "utilities.h"

/* UTILITY
------------------------------------------- */
HFONT win32_get_default_gui_font(void)
{
    static HFONT hFont = NULL;

    if (hFont) return hFont;

    NONCLIENTMETRICS ncMetrics;
    ncMetrics.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncMetrics, 0 );
    hFont = CreateFontIndirect(&ncMetrics.lfMessageFont);

    return hFont;
}


/* UTILITY
------------------------------------------- */
wchar_t* fromUTF8 (const char* src)
{
    if(!src) return NULL;

    int des_len = MultiByteToWideChar(CP_UTF8, 0, src, -1, 0, 0);
    wchar_t *buffer = malloc( (des_len) * sizeof(wchar_t) );

    if (buffer) {
        MultiByteToWideChar(CP_UTF8, 0, src, -1, buffer, des_len);
    }

    return buffer;
}

/* UTILITY
------------------------------------------- */
char* toUTF8(const wchar_t* src)
{
    if(!src) return NULL;

    int des_len = WideCharToMultiByte(CP_UTF8, 0, src, -1, 0, 0, NULL, NULL);
    char *buffer = malloc( (des_len) * sizeof(char) );

    if (buffer) {
        WideCharToMultiByte(CP_UTF8, 0, src, -1, buffer, des_len, NULL, NULL);
    }

    return buffer;
}
