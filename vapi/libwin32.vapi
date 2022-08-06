/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

[CCode (cheader_filename = "vala-win32.h")]
namespace Win32
{
    delegate void Callback( Event event );

    /* POINTER */
    [Compact]
    [CCode (cname="void")]
    public class Handle {}

    #if WIN64
        [SimpleType]
        [CCode (cname = "LONG_PTR", has_type_id = false)]
        public struct LONG_PTR : int64 {}

        [SimpleType]
        [CCode (cname = "UINT_PTR", has_type_id = false)]
        public struct UINT_PTR : int64 {}
    #else
        [SimpleType]
        [CCode (cname = "LONG_PTR", has_type_id = false)]
        public struct LONG_PTR : long {}

        [SimpleType]
        [CCode (cname = "UINT_PTR", has_type_id = false)]
        public struct UINT_PTR : uint {}
    #endif

    [SimpleType]
    [CCode (cname = "LRESULT", has_type_id = false)]
    public struct LRESULT : LONG_PTR {}

    [SimpleType]
    [CCode (cname = "WPARAM", has_type_id = false)]
    public struct WPARAM : UINT_PTR {}

    [SimpleType]
    [CCode (cname = "LPARAM", has_type_id = false)]
    public struct LPARAM : LONG_PTR {}

    [CCode (cname="MSG", has_type_id = false)]
    public struct Message {
        WPARAM wParam;
        LPARAM lParam;
    }

    [CCode (cname = "GetMessageW")]
    int get_message (out Message msg, Handle? hwnd = null, uint wMsgFilterMin = 0, uint wMsgFilterMax = 0);

    [CCode (cname = "TranslateMessage")]
    bool translate_message (ref Message msg);

    [CCode (cname = "DispatchMessageW")]
    LRESULT dispatch_message (ref Message msg);

    LRESULT send_message(Window window, uint msg, WPARAM wParam = 0, LPARAM lParam = 0);

    bool post_message(Window? window, uint msg, WPARAM wParam = 0, LPARAM lParam = 0);


    [CCode (cname="PAINTSTRUCT", has_type_id = false)]
    struct PaintStruct {}

    [CCode (has_type_id = false)]
    class Rect {
        public long left;
        public long top;
        public long right;
        public long bottom;
    }

    [Compact]
    [CCode (cname="void", has_type_id = false, free_function="")]
    class DeviceContext
    {
        public void text_out(int x, int y, string text);
    }

    [CCode (type_id = "WIN32_TYPE_WINDOW")]
    abstract class Window
    {
        public string? text  { get; set; }
        public bool enabled  { get; set; }
        public int left   { get; set; }
        public int top    { get; set; }
        public int width  { get; set; }
        public int height { get; set; }

        public LayoutData positioning { get; set; }

        public void add_listener( uint event_id, owned Callback callback );

        public Rect get_client_rect();
        public Rect get_window_rect();

        public DeviceContext begin_paint(out PaintStruct ps);
        public void end_paint(out PaintStruct ps);

        public void move   (int left, int top);
        public void resize (int width, int height);
        public void move_and_resize (int left, int top, int width, int height);
    }

    [CCode (type_id = "WIN32_TYPE_CONTAINER")]
    abstract class Container : Window
    {
        public Layout layout { get; set; }

        [CCode (array_length_type="size_t")]
        public Window[] get_children();
    }

    [CCode (type_id = "WIN32_TYPE_APPLICATION_WINDOW")]
    class ApplicationWindow : Container
    {
        public int min_width  { get; set; }
        public int min_height { get; set; }
        public int max_width  { get; set; }
        public int max_height { get; set; }

        public ApplicationWindow( string title );

        public void show();

        [CCode (cname = "win32_application_window_get_type1")]
        public GLib.Type get_type();
    }

    [CCode (has_type_id = true)]
    abstract class Control : Window
    {
        public uint id { get; private set; }
    }

    [CCode (has_type_id = true)]
    class Button : Control
    {
        public Button( Window parent, string text="" );
    }

    [CCode (has_type_id = true)]
    class Label : Control
    {
        public Alignment text_align { get; set; }

        public Label( Window parent, string text="" );
    }

    [CCode (has_type_id = true)]
    class Edit : Control
    {
        //public Alignment text_align { get; set; }
        public bool readonly  { get; set; }

        public Edit( Window parent, string text="" );
        public Edit.multiline( Window parent, string text="" );
        // public Edit.multiline( Window parent, Alignment text_align=Alignment.LEFT );
        public Edit.password ( Window parent );
    }

    [Compact]
    [CCode (has_type_id = false)]
    class Event {

        WPARAM wParam;
        LPARAM lParam;

        public Window source;

        public bool handled { get; set;}

        [CCode (cname="FM_CLICKED")]
        public  const uint CLICK;
        [CCode (cname="WM_CREATE")]
        public  const uint CREATE;
        [CCode (cname="WM_PAINT")]
        public  const uint PAINT;
        [CCode (cname="WM_SIZE")]
        public  const uint SIZE;
        [CCode (cname="WM_LBUTTONDOWN")]
        public  const uint LBUTTON_DOWN;
        [CCode (cname="WM_COMMAND")]
        public  const uint Command;
        [CCode (cname="WM_CLIPBOARDUPDATE")]
        public  const uint CLIPBOARD_UPDATE;
    }

    /* LAYOUT */
    [CCode (has_type_id = false)]
    abstract class Layout
    {
        public void layout();
    }

    [CCode (has_type_id = false)]
    class RelativeLayout : Layout
    {
        public RelativeLayout(uint padding=0, uint spacing=0 );
        //public unowned RelativeLayout set_grid(uint scale);
        public unowned RelativeLayout with_spacing(uint vSpacing, uint hSpacing=-1);
        public unowned RelativeLayout with_padding(uint vPadding, uint hPadding=-1);
    }

    [CCode (has_type_id = false)]
    class LayoutData
    {
        public Anchor left   { get; set; }
        public Anchor top    { get; set; }
        public Anchor right  { get; set; }
        public Anchor bottom { get; set; }

        public LayoutData ();
    }

    [CCode (has_type_id = false)]
    class Anchor
    {
        // private Window reference;
        // private int offset;

        public static Anchor to_parent( uint ratio, int offset = 0 );
        public static Anchor to_sibling( Window sibling, int offset = 0);

        public unowned Anchor to_edge(Edge edge);
        public unowned Anchor with_offset(int offset);
    }

    [CCode (cname = "int", cprefix = "EDGE_", has_type_id = false)]
    public enum Edge {
        TOP,
        LEFT,
        BOTTOM,
        RIGHT
    }

    [CCode (cname = "int", cprefix = "ALIGN_", has_type_id = false)]
    public enum Alignment {
        LEFT,
        CENTER,
        RIGHT
    }

    [CCode (has_type_id = false)]
    class Clipboard {
        public static string? text { owned get; set; }
        public static bool add_format_listener (Window window);

        //public static owned string? get_text();

        private Clipboard ();
    }

}// END Win32

// Standard Windows messages
public const uint WM_NULL;
public const uint WM_CREATE;
public const uint WM_DESTROY;
public const uint WM_MOVE;
public const uint WM_SIZE;
public const uint WM_ACTIVATE;
public const uint WM_SETFOCUS;
public const uint WM_KILLFOCUS;
public const uint WM_ENABLE;
public const uint WM_SETREDRAW;
public const uint WM_SETTEXT;
public const uint WM_GETTEXT;
public const uint WM_GETTEXTLENGTH;
public const uint WM_PAINT;
public const uint WM_CLOSE;
public const uint WM_QUERYENDSESSION;
public const uint WM_QUIT;
public const uint WM_QUERYOPEN;
public const uint WM_ERASEBKGND;
public const uint WM_SYSCOLORCHANGE;
public const uint WM_ENDSESSION;
public const uint WM_SHOWWINDOW;
public const uint WM_CTLCOLOR;
public const uint WM_WININICHANGE;
public const uint WM_DEVMODECHANGE;
public const uint WM_ACTIVATEAPP;
public const uint WM_FONTCHANGE;
public const uint WM_TIMECHANGE;
public const uint WM_CANCELMODE;
public const uint WM_SETCURSOR;
public const uint WM_MOUSEACTIVATE;
public const uint WM_CHILDACTIVATE;
public const uint WM_QUEUESYNC;
public const uint WM_GETMINMAXINFO;
public const uint WM_PAINTICON;
public const uint WM_ICONERASEBKGND;
public const uint WM_NEXTDLGCTL;
public const uint WM_SPOOLERSTATUS;
public const uint WM_DRAWITEM;
public const uint WM_MEASUREITEM;
public const uint WM_DELETEITEM;
public const uint WM_VKEYTOITEM;
public const uint WM_CHARTOITEM;
public const uint WM_SETFONT;
public const uint WM_GETFONT;
public const uint WM_SETHOTKEY;
public const uint WM_GETHOTKEY;
public const uint WM_QUERYDRAGICON;
public const uint WM_COMPAREITEM;
public const uint WM_GETOBJECT;
public const uint WM_COMPACTING;
public const uint WM_COMMNOTIFY;
public const uint WM_WINDOWPOSCHANGING;
public const uint WM_WINDOWPOSCHANGED;
public const uint WM_POWER;
public const uint WM_COPYGLOBALDATA;
public const uint WM_COPYDATA;
public const uint WM_CANCELJOURNAL;
public const uint WM_NOTIFY;
public const uint WM_INPUTLANGCHANGEREQUEST;
public const uint WM_INPUTLANGCHANGE;
public const uint WM_TCARD;
public const uint WM_HELP;
public const uint WM_USERCHANGED;
public const uint WM_NOTIFYFORMAT;
public const uint WM_CONTEXTMENU;
public const uint WM_STYLECHANGING;
public const uint WM_STYLECHANGED;
public const uint WM_DISPLAYCHANGE;
public const uint WM_GETICON;
public const uint WM_SETICON;
public const uint WM_NCCREATE;
public const uint WM_NCDESTROY;
public const uint WM_NCCALCSIZE;
public const uint WM_NCHITTEST;
public const uint WM_NCPAINT;
public const uint WM_NCACTIVATE;
public const uint WM_GETDLGCODE;
public const uint WM_SYNCPAINT;
public const uint WM_NCMOUSEMOVE;
public const uint WM_NCLBUTTONDOWN;
public const uint WM_NCLBUTTONUP;
public const uint WM_NCLBUTTONDBLCLK;
public const uint WM_NCRBUTTONDOWN;
public const uint WM_NCRBUTTONUP;
public const uint WM_NCRBUTTONDBLCLK;
public const uint WM_NCMBUTTONDOWN;
public const uint WM_NCMBUTTONUP;
public const uint WM_NCMBUTTONDBLCLK;
public const uint WM_NCXBUTTONDOWN;
public const uint WM_NCXBUTTONUP;
public const uint WM_NCXBUTTONDBLCLK;
public const uint EM_GETSEL;
public const uint EM_SETSEL;
public const uint EM_GETRECT;
public const uint EM_SETRECT;
public const uint EM_SETRECTNP;
public const uint EM_SCROLL;
public const uint EM_LINESCROLL;
public const uint EM_SCROLLCARET;
public const uint EM_GETMODIFY;
public const uint EM_SETMODIFY;
public const uint EM_GETLINECOUNT;
public const uint EM_LINEINDEX;
public const uint EM_SETHANDLE;
public const uint EM_GETHANDLE;
public const uint EM_GETTHUMB;
public const uint EM_LINELENGTH;
public const uint EM_REPLACESEL;
public const uint EM_SETFONT;
public const uint EM_GETLINE;
public const uint EM_LIMITTEXT;
public const uint EM_SETLIMITTEXT;
public const uint EM_CANUNDO;
public const uint EM_UNDO;
public const uint EM_FMTLINES;
public const uint EM_LINEFROMCHAR;
public const uint EM_SETWORDBREAK;
public const uint EM_SETTABSTOPS;
public const uint EM_SETPASSWORDCHAR;
public const uint EM_EMPTYUNDOBUFFER;
public const uint EM_GETFIRSTVISIBLELINE;
public const uint EM_SETREADONLY;
public const uint EM_SETWORDBREAKPROC;
public const uint EM_GETWORDBREAKPROC;
public const uint EM_GETPASSWORDCHAR;
public const uint EM_SETMARGINS;
public const uint EM_GETMARGINS;
public const uint EM_GETLIMITTEXT;
public const uint EM_POSFROMCHAR;
public const uint EM_CHARFROMPOS;
public const uint EM_SETIMESTATUS;
public const uint EM_GETIMESTATUS;
public const uint SBM_SETPOS;
public const uint SBM_GETPOS;
public const uint SBM_SETRANGE;
public const uint SBM_GETRANGE;
public const uint SBM_ENABLE_ARROWS;
public const uint SBM_SETRANGEREDRAW;
public const uint SBM_SETSCROLLINFO;
public const uint SBM_GETSCROLLINFO;
public const uint SBM_GETSCROLLBARINFO;
public const uint BM_GETCHECK;
public const uint BM_SETCHECK;
public const uint BM_GETSTATE;
public const uint BM_SETSTATE;
public const uint BM_SETSTYLE;
public const uint BM_CLICK;
public const uint BM_GETIMAGE;
public const uint BM_SETIMAGE;
public const uint BM_SETDONTCLICK;
public const uint WM_INPUT;
public const uint WM_KEYDOWN;
public const uint WM_KEYFIRST;
public const uint WM_KEYUP;
public const uint WM_CHAR;
public const uint WM_DEADCHAR;
public const uint WM_SYSKEYDOWN;
public const uint WM_SYSKEYUP;
public const uint WM_SYSCHAR;
public const uint WM_SYSDEADCHAR;
public const uint WM_UNICHAR;
public const uint WM_KEYLAST;
public const uint WM_WNT_CONVERTREQUESTEX;
public const uint WM_CONVERTREQUEST;
public const uint WM_CONVERTRESULT;
public const uint WM_INTERIM;
public const uint WM_IME_STARTCOMPOSITION;
public const uint WM_IME_ENDCOMPOSITION;
public const uint WM_IME_COMPOSITION;
public const uint WM_IME_KEYLAST;
public const uint WM_INITDIALOG;
public const uint WM_COMMAND;
public const uint WM_SYSCOMMAND;
public const uint WM_TIMER;
public const uint WM_HSCROLL;
public const uint WM_VSCROLL;
public const uint WM_INITMENU;
public const uint WM_INITMENUPOPUP;
public const uint WM_SYSTIMER;
public const uint WM_MENUSELECT;
public const uint WM_MENUCHAR;
public const uint WM_ENTERIDLE;
public const uint WM_MENURBUTTONUP;
public const uint WM_MENUDRAG;
public const uint WM_MENUGETOBJECT;
public const uint WM_UNINITMENUPOPUP;
public const uint WM_MENUCOMMAND;
public const uint WM_CHANGEUISTATE;
public const uint WM_UPDATEUISTATE;
public const uint WM_QUERYUISTATE;
public const uint WM_LBTRACKPOINT;
public const uint WM_CTLCOLORMSGBOX;
public const uint WM_CTLCOLOREDIT;
public const uint WM_CTLCOLORLISTBOX;
public const uint WM_CTLCOLORBTN;
public const uint WM_CTLCOLORDLG;
public const uint WM_CTLCOLORSCROLLBAR;
public const uint WM_CTLCOLORSTATIC;
public const uint CB_GETEDITSEL;
public const uint CB_LIMITTEXT;
public const uint CB_SETEDITSEL;
public const uint CB_ADDSTRING;
public const uint CB_DELETESTRING;
public const uint CB_DIR;
public const uint CB_GETCOUNT;
public const uint CB_GETCURSEL;
public const uint CB_GETLBTEXT;
public const uint CB_GETLBTEXTLEN;
public const uint CB_INSERTSTRING;
public const uint CB_RESETCONTENT;
public const uint CB_FINDSTRING;
public const uint CB_SELECTSTRING;
public const uint CB_SETCURSEL;
public const uint CB_SHOWDROPDOWN;
public const uint CB_GETITEMDATA;
public const uint CB_SETITEMDATA;
public const uint CB_GETDROPPEDCONTROLRECT;
public const uint CB_SETITEMHEIGHT;
public const uint CB_GETITEMHEIGHT;
public const uint CB_SETEXTENDEDUI;
public const uint CB_GETEXTENDEDUI;
public const uint CB_GETDROPPEDSTATE;
public const uint CB_FINDSTRINGEXACT;
public const uint CB_SETLOCALE;
public const uint CB_GETLOCALE;
public const uint CB_GETTOPINDEX;
public const uint CB_SETTOPINDEX;
public const uint CB_GETHORIZONTALEXTENT;
public const uint CB_SETHORIZONTALEXTENT;
public const uint CB_GETDROPPEDWIDTH;
public const uint CB_SETDROPPEDWIDTH;
public const uint CB_INITSTORAGE;
public const uint CB_MULTIPLEADDSTRING;
public const uint CB_GETCOMBOBOXINFO;
public const uint CB_MSGMAX;
public const uint WM_MOUSEFIRST;
public const uint WM_MOUSEMOVE;
public const uint WM_LBUTTONDOWN;
public const uint WM_LBUTTONUP;
public const uint WM_LBUTTONDBLCLK;
public const uint WM_RBUTTONDOWN;
public const uint WM_RBUTTONUP;
public const uint WM_RBUTTONDBLCLK;
public const uint WM_MBUTTONDOWN;
public const uint WM_MBUTTONUP;
public const uint WM_MBUTTONDBLCLK;
public const uint WM_MOUSELAST;
public const uint WM_MOUSEWHEEL;
public const uint WM_XBUTTONDOWN;
public const uint WM_XBUTTONUP;
public const uint WM_XBUTTONDBLCLK;
public const uint WM_MOUSEHWHEEL;
public const uint WM_PARENTNOTIFY;
public const uint WM_ENTERMENULOOP;
public const uint WM_EXITMENULOOP;
public const uint WM_NEXTMENU;
public const uint WM_SIZING;
public const uint WM_CAPTURECHANGED;
public const uint WM_MOVING;
public const uint WM_POWERBROADCAST;
public const uint WM_DEVICECHANGE;
public const uint WM_MDICREATE;
public const uint WM_MDIDESTROY;
public const uint WM_MDIACTIVATE;
public const uint WM_MDIRESTORE;
public const uint WM_MDINEXT;
public const uint WM_MDIMAXIMIZE;
public const uint WM_MDITILE;
public const uint WM_MDICASCADE;
public const uint WM_MDIICONARRANGE;
public const uint WM_MDIGETACTIVE;
public const uint WM_MDISETMENU;
public const uint WM_ENTERSIZEMOVE;
public const uint WM_EXITSIZEMOVE;
public const uint WM_DROPFILES;
public const uint WM_MDIREFRESHMENU;
public const uint WM_IME_REPORT;
public const uint WM_IME_SETCONTEXT;
public const uint WM_IME_NOTIFY;
public const uint WM_IME_CONTROL;
public const uint WM_IME_COMPOSITIONFULL;
public const uint WM_IME_SELECT;
public const uint WM_IME_CHAR;
public const uint WM_IME_REQUEST;
public const uint WM_IMEKEYDOWN;
public const uint WM_IME_KEYDOWN;
public const uint WM_IMEKEYUP;
public const uint WM_IME_KEYUP;
public const uint WM_NCMOUSEHOVER;
public const uint WM_MOUSEHOVER;
public const uint WM_NCMOUSELEAVE;
public const uint WM_MOUSELEAVE;
public const uint WM_CUT;
public const uint WM_COPY;
public const uint WM_PASTE;
public const uint WM_CLEAR;
public const uint WM_UNDO;
public const uint WM_RENDERFORMAT;
public const uint WM_RENDERALLFORMATS;
public const uint WM_DESTROYCLIPBOARD;
public const uint WM_DRAWCLIPBOARD;
public const uint WM_PAINTCLIPBOARD;
public const uint WM_VSCROLLCLIPBOARD;
public const uint WM_SIZECLIPBOARD;
public const uint WM_ASKCBFORMATNAME;
public const uint WM_CHANGECBCHAIN;
public const uint WM_HSCROLLCLIPBOARD;
public const uint WM_QUERYNEWPALETTE;
public const uint WM_PALETTEISCHANGING;
public const uint WM_PALETTECHANGED;
public const uint WM_HOTKEY;
public const uint WM_PRINT;
public const uint WM_PRINTCLIENT;
public const uint WM_APPCOMMAND;
public const uint WM_HANDHELDFIRST;
public const uint WM_HANDHELDLAST;
public const uint WM_AFXFIRST;
public const uint WM_AFXLAST;
public const uint WM_PENWINFIRST;
public const uint WM_RCRESULT;
public const uint WM_HOOKRCRESULT;
public const uint WM_GLOBALRCCHANGE;
public const uint WM_PENMISCINFO;
public const uint WM_SKB;
public const uint WM_HEDITCTL;
public const uint WM_PENCTL;
public const uint WM_PENMISC;
public const uint WM_CTLINIT;
public const uint WM_PENEVENT;
public const uint WM_PENWINLAST;
public const uint DDM_SETFMT;
public const uint DM_GETDEFID;
public const uint NIN_SELECT;
public const uint TBM_GETPOS;
public const uint WM_PSD_PAGESETUPDLG;
public const uint WM_USER;
public const uint CBEM_INSERTITEMA;
public const uint DDM_DRAW;
public const uint DM_SETDEFID;
public const uint HKM_SETHOTKEY;
public const uint PBM_SETRANGE;
public const uint RB_INSERTBANDA;
public const uint SB_SETTEXTA;
public const uint TB_ENABLEBUTTON;
public const uint TBM_GETRANGEMIN;
public const uint TTM_ACTIVATE;
public const uint WM_CHOOSEFONT_GETLOGFONT;
public const uint WM_PSD_FULLPAGERECT;
public const uint CBEM_SETIMAGELIST;
public const uint DDM_CLOSE;
public const uint DM_REPOSITION;
public const uint HKM_GETHOTKEY;
public const uint PBM_SETPOS;
public const uint RB_DELETEBAND;
public const uint SB_GETTEXTA;
public const uint TB_CHECKBUTTON;
public const uint TBM_GETRANGEMAX;
public const uint WM_PSD_MINMARGINRECT;
public const uint CBEM_GETIMAGELIST;
public const uint DDM_BEGIN;
public const uint HKM_SETRULES;
public const uint PBM_DELTAPOS;
public const uint RB_GETBARINFO;
public const uint SB_GETTEXTLENGTHA;
public const uint TBM_GETTIC;
public const uint TB_PRESSBUTTON;
public const uint TTM_SETDELAYTIME;
public const uint WM_PSD_MARGINRECT;
public const uint CBEM_GETITEMA;
public const uint DDM_END;
public const uint PBM_SETSTEP;
public const uint RB_SETBARINFO;
public const uint SB_SETPARTS;
public const uint TB_HIDEBUTTON;
public const uint TBM_SETTIC;
public const uint TTM_ADDTOOLA;
public const uint WM_PSD_GREEKTEXTRECT;
public const uint CBEM_SETITEMA;
public const uint PBM_STEPIT;
public const uint TB_INDETERMINATE;
public const uint TBM_SETPOS;
public const uint TTM_DELTOOLA;
public const uint WM_PSD_ENVSTAMPRECT;
public const uint CBEM_GETCOMBOCONTROL;
public const uint PBM_SETRANGE32;
public const uint RB_SETBANDINFOA;
public const uint SB_GETPARTS;
public const uint TB_MARKBUTTON;
public const uint TBM_SETRANGE;
public const uint TTM_NEWTOOLRECTA;
public const uint WM_PSD_YAFULLPAGERECT;
public const uint CBEM_GETEDITCONTROL;
public const uint PBM_GETRANGE;
public const uint RB_SETPARENT;
public const uint SB_GETBORDERS;
public const uint TBM_SETRANGEMIN;
public const uint TTM_RELAYEVENT;
public const uint CBEM_SETEXSTYLE;
public const uint PBM_GETPOS;
public const uint RB_HITTEST;
public const uint SB_SETMINHEIGHT;
public const uint TBM_SETRANGEMAX;
public const uint TTM_GETTOOLINFOA;
public const uint CBEM_GETEXSTYLE;
public const uint CBEM_GETEXTENDEDSTYLE;
public const uint PBM_SETBARCOLOR;
public const uint RB_GETRECT;
public const uint SB_SIMPLE;
public const uint TB_ISBUTTONENABLED;
public const uint TBM_CLEARTICS;
public const uint TTM_SETTOOLINFOA;
public const uint CBEM_HASEDITCHANGED;
public const uint RB_INSERTBANDW;
public const uint SB_GETRECT;
public const uint TB_ISBUTTONCHECKED;
public const uint TBM_SETSEL;
public const uint TTM_HITTESTA;
public const uint WIZ_QUERYNUMPAGES;
public const uint CBEM_INSERTITEMW;
public const uint RB_SETBANDINFOW;
public const uint SB_SETTEXTW;
public const uint TB_ISBUTTONPRESSED;
public const uint TBM_SETSELSTART;
public const uint TTM_GETTEXTA;
public const uint WIZ_NEXT;
public const uint CBEM_SETITEMW;
public const uint RB_GETBANDCOUNT;
public const uint SB_GETTEXTLENGTHW;
public const uint TB_ISBUTTONHIDDEN;
public const uint TBM_SETSELEND;
public const uint TTM_UPDATETIPTEXTA;
public const uint WIZ_PREV;
public const uint CBEM_GETITEMW;
public const uint RB_GETROWCOUNT;
public const uint SB_GETTEXTW;
public const uint TB_ISBUTTONINDETERMINATE;
public const uint TTM_GETTOOLCOUNT;
public const uint CBEM_SETEXTENDEDSTYLE;
public const uint RB_GETROWHEIGHT;
public const uint SB_ISSIMPLE;
public const uint TB_ISBUTTONHIGHLIGHTED;
public const uint TBM_GETPTICS;
public const uint TTM_ENUMTOOLSA;
public const uint SB_SETICON;
public const uint TBM_GETTICPOS;
public const uint TTM_GETCURRENTTOOLA;
public const uint RB_IDTOINDEX;
public const uint SB_SETTIPTEXTA;
public const uint TBM_GETNUMTICS;
public const uint TTM_WINDOWFROMPOINT;
public const uint RB_GETTOOLTIPS;
public const uint SB_SETTIPTEXTW;
public const uint TBM_GETSELSTART;
public const uint TB_SETSTATE;
public const uint TTM_TRACKACTIVATE;
public const uint RB_SETTOOLTIPS;
public const uint SB_GETTIPTEXTA;
public const uint TB_GETSTATE;
public const uint TBM_GETSELEND;
public const uint TTM_TRACKPOSITION;
public const uint RB_SETBKCOLOR;
public const uint SB_GETTIPTEXTW;
public const uint TB_ADDBITMAP;
public const uint TBM_CLEARSEL;
public const uint TTM_SETTIPBKCOLOR;
public const uint RB_GETBKCOLOR;
public const uint SB_GETICON;
public const uint TB_ADDBUTTONSA;
public const uint TBM_SETTICFREQ;
public const uint TTM_SETTIPTEXTCOLOR;
public const uint RB_SETTEXTCOLOR;
public const uint TB_INSERTBUTTONA;
public const uint TBM_SETPAGESIZE;
public const uint TTM_GETDELAYTIME;
public const uint RB_GETTEXTCOLOR;
public const uint TB_DELETEBUTTON;
public const uint TBM_GETPAGESIZE;
public const uint TTM_GETTIPBKCOLOR;
public const uint RB_SIZETORECT;
public const uint TB_GETBUTTON;
public const uint TBM_SETLINESIZE;
public const uint TTM_GETTIPTEXTCOLOR;
public const uint RB_BEGINDRAG;
public const uint TB_BUTTONCOUNT;
public const uint TBM_GETLINESIZE;
public const uint TTM_SETMAXTIPWIDTH;
public const uint RB_ENDDRAG;
public const uint TB_COMMANDTOINDEX;
public const uint TBM_GETTHUMBRECT;
public const uint TTM_GETMAXTIPWIDTH;
public const uint RB_DRAGMOVE;
public const uint TBM_GETCHANNELRECT;
public const uint TB_SAVERESTOREA;
public const uint TTM_SETMARGIN;
public const uint RB_GETBARHEIGHT;
public const uint TB_CUSTOMIZE;
public const uint TBM_SETTHUMBLENGTH;
public const uint TTM_GETMARGIN;
public const uint RB_GETBANDINFOW;
public const uint TB_ADDSTRINGA;
public const uint TBM_GETTHUMBLENGTH;
public const uint TTM_POP;
public const uint RB_GETBANDINFOA;
public const uint TB_GETITEMRECT;
public const uint TBM_SETTOOLTIPS;
public const uint TTM_UPDATE;
public const uint RB_MINIMIZEBAND;
public const uint TB_BUTTONSTRUCTSIZE;
public const uint TBM_GETTOOLTIPS;
public const uint TTM_GETBUBBLESIZE;
public const uint RB_MAXIMIZEBAND;
public const uint TBM_SETTIPSIDE;
public const uint TB_SETBUTTONSIZE;
public const uint TTM_ADJUSTRECT;
public const uint TBM_SETBUDDY;
public const uint TB_SETBITMAPSIZE;
public const uint TTM_SETTITLEA;
public const uint MSG_FTS_JUMP_VA;
public const uint TB_AUTOSIZE;
public const uint TBM_GETBUDDY;
public const uint TTM_SETTITLEW;
public const uint RB_GETBANDBORDERS;
public const uint MSG_FTS_JUMP_QWORD;
public const uint RB_SHOWBAND;
public const uint TB_GETTOOLTIPS;
public const uint MSG_REINDEX_REQUEST;
public const uint TB_SETTOOLTIPS;
public const uint MSG_FTS_WHERE_IS_IT;
public const uint RB_SETPALETTE;
public const uint TB_SETPARENT;
public const uint RB_GETPALETTE;
public const uint RB_MOVEBAND;
public const uint TB_SETROWS;
public const uint TB_GETROWS;
public const uint TB_GETBITMAPFLAGS;
public const uint TB_SETCMDID;
public const uint RB_PUSHCHEVRON;
public const uint TB_CHANGEBITMAP;
public const uint TB_GETBITMAP;
public const uint MSG_GET_DEFFONT;
public const uint TB_GETBUTTONTEXTA;
public const uint TB_REPLACEBITMAP;
public const uint TB_SETINDENT;
public const uint TB_SETIMAGELIST;
public const uint TB_GETIMAGELIST;
public const uint TB_LOADIMAGES;
public const uint EM_CANPASTE;
public const uint TTM_ADDTOOLW;
public const uint EM_DISPLAYBAND;
public const uint TB_GETRECT;
public const uint TTM_DELTOOLW;
public const uint EM_EXGETSEL;
public const uint TB_SETHOTIMAGELIST;
public const uint TTM_NEWTOOLRECTW;
public const uint EM_EXLIMITTEXT;
public const uint TB_GETHOTIMAGELIST;
public const uint TTM_GETTOOLINFOW;
public const uint EM_EXLINEFROMCHAR;
public const uint TB_SETDISABLEDIMAGELIST;
public const uint TTM_SETTOOLINFOW;
public const uint EM_EXSETSEL;
public const uint TB_GETDISABLEDIMAGELIST;
public const uint TTM_HITTESTW;
public const uint EM_FINDTEXT;
public const uint TB_SETSTYLE;
public const uint TTM_GETTEXTW;
public const uint EM_FORMATRANGE;
public const uint TB_GETSTYLE;
public const uint TTM_UPDATETIPTEXTW;
public const uint EM_GETCHARFORMAT;
public const uint TB_GETBUTTONSIZE;
public const uint TTM_ENUMTOOLSW;
public const uint EM_GETEVENTMASK;
public const uint TB_SETBUTTONWIDTH;
public const uint TTM_GETCURRENTTOOLW;
public const uint EM_GETOLEINTERFACE;
public const uint TB_SETMAXTEXTROWS;
public const uint EM_GETPARAFORMAT;
public const uint TB_GETTEXTROWS;
public const uint EM_GETSELTEXT;
public const uint TB_GETOBJECT;
public const uint EM_HIDESELECTION;
public const uint TB_GETBUTTONINFOW;
public const uint EM_PASTESPECIAL;
public const uint TB_SETBUTTONINFOW;
public const uint EM_REQUESTRESIZE;
public const uint TB_GETBUTTONINFOA;
public const uint EM_SELECTIONTYPE;
public const uint TB_SETBUTTONINFOA;
public const uint EM_SETBKGNDCOLOR;
public const uint TB_INSERTBUTTONW;
public const uint EM_SETCHARFORMAT;
public const uint TB_ADDBUTTONSW;
public const uint EM_SETEVENTMASK;
public const uint TB_HITTEST;
public const uint EM_SETOLECALLBACK;
public const uint TB_SETDRAWTEXTFLAGS;
public const uint EM_SETPARAFORMAT;
public const uint TB_GETHOTITEM;
public const uint EM_SETTARGETDEVICE;
public const uint TB_SETHOTITEM;
public const uint EM_STREAMIN;
public const uint TB_SETANCHORHIGHLIGHT;
public const uint EM_STREAMOUT;
public const uint TB_GETANCHORHIGHLIGHT;
public const uint EM_GETTEXTRANGE;
public const uint TB_GETBUTTONTEXTW;
public const uint EM_FINDWORDBREAK;
public const uint TB_SAVERESTOREW;
public const uint EM_SETOPTIONS;
public const uint TB_ADDSTRINGW;
public const uint EM_GETOPTIONS;
public const uint TB_MAPACCELERATORA;
public const uint EM_FINDTEXTEX;
public const uint TB_GETINSERTMARK;
public const uint EM_GETWORDBREAKPROCEX;
public const uint TB_SETINSERTMARK;
public const uint EM_SETWORDBREAKPROCEX;
public const uint TB_INSERTMARKHITTEST;
public const uint EM_SETUNDOLIMIT;
public const uint TB_MOVEBUTTON;
public const uint TB_GETMAXSIZE;
public const uint EM_REDO;
public const uint TB_SETEXTENDEDSTYLE;
public const uint EM_CANREDO;
public const uint TB_GETEXTENDEDSTYLE;
public const uint EM_GETUNDONAME;
public const uint TB_GETPADDING;
public const uint EM_GETREDONAME;
public const uint TB_SETPADDING;
public const uint EM_STOPGROUPTYPING;
public const uint TB_SETINSERTMARKCOLOR;
public const uint EM_SETTEXTMODE;
public const uint TB_GETINSERTMARKCOLOR;
public const uint EM_GETTEXTMODE;
public const uint TB_MAPACCELERATORW;
public const uint EM_AUTOURLDETECT;
public const uint TB_GETSTRINGW;
public const uint EM_GETAUTOURLDETECT;
public const uint TB_GETSTRINGA;
public const uint EM_SETPALETTE;
public const uint EM_GETTEXTEX;
public const uint EM_GETTEXTLENGTHEX;
public const uint EM_SHOWSCROLLBAR;
public const uint EM_SETTEXTEX;
public const uint TAPI_REPLY;
public const uint ACM_OPENA;
public const uint BFFM_SETSTATUSTEXTA;
public const uint CDM_FIRST;
public const uint CDM_GETSPEC;
public const uint EM_SETPUNCTUATION;
public const uint IPM_CLEARADDRESS;
public const uint WM_CAP_UNICODE_START;
public const uint ACM_PLAY;
public const uint BFFM_ENABLEOK;
public const uint CDM_GETFILEPATH;
public const uint EM_GETPUNCTUATION;
public const uint IPM_SETADDRESS;
public const uint PSM_SETCURSEL;
public const uint UDM_SETRANGE;
public const uint WM_CHOOSEFONT_SETLOGFONT;
public const uint ACM_STOP;
public const uint BFFM_SETSELECTIONA;
public const uint CDM_GETFOLDERPATH;
public const uint EM_SETWORDWRAPMODE;
public const uint IPM_GETADDRESS;
public const uint PSM_REMOVEPAGE;
public const uint UDM_GETRANGE;
public const uint WM_CAP_SET_CALLBACK_ERRORW;
public const uint WM_CHOOSEFONT_SETFLAGS;
public const uint ACM_OPENW;
public const uint BFFM_SETSELECTIONW;
public const uint CDM_GETFOLDERIDLIST;
public const uint EM_GETWORDWRAPMODE;
public const uint IPM_SETRANGE;
public const uint PSM_ADDPAGE;
public const uint UDM_SETPOS;
public const uint WM_CAP_SET_CALLBACK_STATUSW;
public const uint BFFM_SETSTATUSTEXTW;
public const uint CDM_SETCONTROLTEXT;
public const uint EM_SETIMECOLOR;
public const uint IPM_SETFOCUS;
public const uint PSM_CHANGED;
public const uint UDM_GETPOS;
public const uint CDM_HIDECONTROL;
public const uint EM_GETIMECOLOR;
public const uint IPM_ISBLANK;
public const uint PSM_RESTARTWINDOWS;
public const uint UDM_SETBUDDY;
public const uint CDM_SETDEFEXT;
public const uint EM_SETIMEOPTIONS;
public const uint PSM_REBOOTSYSTEM;
public const uint UDM_GETBUDDY;
public const uint EM_GETIMEOPTIONS;
public const uint PSM_CANCELTOCLOSE;
public const uint UDM_SETACCEL;
public const uint EM_CONVPOSITION;
public const uint PSM_QUERYSIBLINGS;
public const uint UDM_GETACCEL;
public const uint MCIWNDM_GETZOOM;
public const uint PSM_UNCHANGED;
public const uint UDM_SETBASE;
public const uint PSM_APPLY;
public const uint UDM_GETBASE;
public const uint PSM_SETTITLEA;
public const uint UDM_SETRANGE32;
public const uint PSM_SETWIZBUTTONS;
public const uint UDM_GETRANGE32;
public const uint WM_CAP_DRIVER_GET_NAMEW;
public const uint PSM_PRESSBUTTON;
public const uint UDM_SETPOS32;
public const uint WM_CAP_DRIVER_GET_VERSIONW;
public const uint PSM_SETCURSELID;
public const uint UDM_GETPOS32;
public const uint PSM_SETFINISHTEXTA;
public const uint PSM_GETTABCONTROL;
public const uint PSM_ISDIALOGMESSAGE;
public const uint MCIWNDM_REALIZE;
public const uint PSM_GETCURRENTPAGEHWND;
public const uint MCIWNDM_SETTIMEFORMATA;
public const uint PSM_INSERTPAGE;
public const uint EM_SETLANGOPTIONS;
public const uint MCIWNDM_GETTIMEFORMATA;
public const uint PSM_SETTITLEW;
public const uint WM_CAP_FILE_SET_CAPTURE_FILEW;
public const uint EM_GETLANGOPTIONS;
public const uint MCIWNDM_VALIDATEMEDIA;
public const uint PSM_SETFINISHTEXTW;
public const uint WM_CAP_FILE_GET_CAPTURE_FILEW;
public const uint EM_GETIMECOMPMODE;
public const uint EM_FINDTEXTW;
public const uint MCIWNDM_PLAYTO;
public const uint WM_CAP_FILE_SAVEASW;
public const uint EM_FINDTEXTEXW;
public const uint MCIWNDM_GETFILENAMEA;
public const uint EM_RECONVERSION;
public const uint MCIWNDM_GETDEVICEA;
public const uint PSM_SETHEADERTITLEA;
public const uint WM_CAP_FILE_SAVEDIBW;
public const uint EM_SETIMEMODEBIAS;
public const uint MCIWNDM_GETPALETTE;
public const uint PSM_SETHEADERTITLEW;
public const uint EM_GETIMEMODEBIAS;
public const uint MCIWNDM_SETPALETTE;
public const uint PSM_SETHEADERSUBTITLEA;
public const uint MCIWNDM_GETERRORA;
public const uint PSM_SETHEADERSUBTITLEW;
public const uint PSM_HWNDTOINDEX;
public const uint PSM_INDEXTOHWND;
public const uint MCIWNDM_SETINACTIVETIMER;
public const uint PSM_PAGETOINDEX;
public const uint PSM_INDEXTOPAGE;
public const uint DL_BEGINDRAG;
public const uint MCIWNDM_GETINACTIVETIMER;
public const uint PSM_IDTOINDEX;
public const uint DL_DRAGGING;
public const uint PSM_INDEXTOID;
public const uint DL_DROPPED;
public const uint PSM_GETRESULT;
public const uint DL_CANCELDRAG;
public const uint PSM_RECALCPAGESIZES;
public const uint MCIWNDM_GET_SOURCE;
public const uint MCIWNDM_PUT_SOURCE;
public const uint MCIWNDM_GET_DEST;
public const uint MCIWNDM_PUT_DEST;
public const uint MCIWNDM_CAN_PLAY;
public const uint MCIWNDM_CAN_WINDOW;
public const uint MCIWNDM_CAN_RECORD;
public const uint MCIWNDM_CAN_SAVE;
public const uint MCIWNDM_CAN_EJECT;
public const uint MCIWNDM_CAN_CONFIG;
public const uint IE_GETINK;
public const uint IE_MSGFIRST;
public const uint MCIWNDM_PALETTEKICK;
public const uint IE_SETINK;
public const uint IE_GETPENTIP;
public const uint IE_SETPENTIP;
public const uint IE_GETERASERTIP;
public const uint IE_SETERASERTIP;
public const uint IE_GETBKGND;
public const uint IE_SETBKGND;
public const uint IE_GETGRIDORIGIN;
public const uint IE_SETGRIDORIGIN;
public const uint IE_GETGRIDPEN;
public const uint IE_SETGRIDPEN;
public const uint IE_GETGRIDSIZE;
public const uint IE_SETGRIDSIZE;
public const uint IE_GETMODE;
public const uint IE_SETMODE;
public const uint IE_GETINKRECT;
public const uint WM_CAP_SET_MCI_DEVICEW;
public const uint WM_CAP_GET_MCI_DEVICEW;
public const uint WM_CAP_PAL_OPENW;
public const uint WM_CAP_PAL_SAVEW;
public const uint IE_GETAPPDATA;
public const uint IE_SETAPPDATA;
public const uint IE_GETDRAWOPTS;
public const uint IE_SETDRAWOPTS;
public const uint IE_GETFORMAT;
public const uint IE_SETFORMAT;
public const uint IE_GETINKINPUT;
public const uint IE_SETINKINPUT;
public const uint IE_GETNOTIFY;
public const uint IE_SETNOTIFY;
public const uint IE_GETRECOG;
public const uint IE_SETRECOG;
public const uint IE_GETSECURITY;
public const uint IE_SETSECURITY;
public const uint IE_GETSEL;
public const uint IE_SETSEL;
public const uint CDM_LAST;
public const uint EM_SETBIDIOPTIONS;
public const uint IE_DOCOMMAND;
public const uint MCIWNDM_NOTIFYMODE;
public const uint EM_GETBIDIOPTIONS;
public const uint IE_GETCOMMAND;
public const uint EM_SETTYPOGRAPHYOPTIONS;
public const uint IE_GETCOUNT;
public const uint EM_GETTYPOGRAPHYOPTIONS;
public const uint IE_GETGESTURE;
public const uint MCIWNDM_NOTIFYMEDIA;
public const uint EM_SETEDITSTYLE;
public const uint IE_GETMENU;
public const uint EM_GETEDITSTYLE;
public const uint IE_GETPAINTDC;
public const uint MCIWNDM_NOTIFYERROR;
public const uint IE_GETPDEVENT;
public const uint IE_GETSELCOUNT;
public const uint IE_GETSELITEMS;
public const uint IE_GETSTYLE;
public const uint MCIWNDM_SETTIMEFORMATW;
public const uint EM_OUTLINE;
public const uint MCIWNDM_GETTIMEFORMATW;
public const uint EM_GETSCROLLPOS;
public const uint EM_SETSCROLLPOS;
public const uint EM_SETFONTSIZE;
public const uint EM_GETZOOM;
public const uint MCIWNDM_GETFILENAMEW;
public const uint EM_SETZOOM;
public const uint MCIWNDM_GETDEVICEW;
public const uint EM_GETVIEWKIND;
public const uint EM_SETVIEWKIND;
public const uint EM_GETPAGE;
public const uint MCIWNDM_GETERRORW;
public const uint EM_SETPAGE;
public const uint EM_GETHYPHENATEINFO;
public const uint EM_SETHYPHENATEINFO;
public const uint EM_GETPAGEROTATE;
public const uint EM_SETPAGEROTATE;
public const uint EM_GETCTFMODEBIAS;
public const uint EM_SETCTFMODEBIAS;
public const uint EM_GETCTFOPENSTATUS;
public const uint EM_SETCTFOPENSTATUS;
public const uint EM_GETIMECOMPTEXT;
public const uint EM_ISIME;
public const uint EM_GETIMEPROPERTY;
public const uint EM_GETQUERYRTFOBJ;
public const uint EM_SETQUERYRTFOBJ;
public const uint FM_GETFOCUS;
public const uint FM_GETDRIVEINFOA;
public const uint FM_GETSELCOUNT;
public const uint FM_GETSELCOUNTLFN;
public const uint FM_GETFILESELA;
public const uint FM_GETFILESELLFNA;
public const uint FM_REFRESH_WINDOWS;
public const uint FM_RELOAD_EXTENSIONS;
public const uint FM_GETDRIVEINFOW;
public const uint FM_GETFILESELW;
public const uint FM_GETFILESELLFNW;
public const uint WLX_WM_SAS;
public const uint SM_GETSELCOUNT;
public const uint UM_GETSELCOUNT;
public const uint WM_CPL_LAUNCH;
public const uint SM_GETSERVERSELA;
public const uint UM_GETUSERSELA;
public const uint WM_CPL_LAUNCHED;
public const uint SM_GETSERVERSELW;
public const uint UM_GETUSERSELW;
public const uint SM_GETCURFOCUSA;
public const uint UM_GETGROUPSELA;
public const uint SM_GETCURFOCUSW;
public const uint UM_GETGROUPSELW;
public const uint SM_GETOPTIONS;
public const uint UM_GETCURFOCUSA;
public const uint UM_GETCURFOCUSW;
public const uint UM_GETOPTIONS;
public const uint UM_GETOPTIONS2;
public const uint LVM_FIRST;
public const uint LVM_GETBKCOLOR;
public const uint LVM_SETBKCOLOR;
public const uint LVM_GETIMAGELIST;
public const uint LVM_SETIMAGELIST;
public const uint LVM_GETITEMCOUNT;
public const uint LVM_GETITEMA;
public const uint LVM_SETITEMA;
public const uint LVM_INSERTITEMA;
public const uint LVM_DELETEITEM;
public const uint LVM_DELETEALLITEMS;
public const uint LVM_GETCALLBACKMASK;
public const uint LVM_SETCALLBACKMASK;
public const uint LVM_GETNEXTITEM;
public const uint LVM_FINDITEMA;
public const uint LVM_GETITEMRECT;
public const uint LVM_SETITEMPOSITION;
public const uint LVM_GETITEMPOSITION;
public const uint LVM_GETSTRINGWIDTHA;
public const uint LVM_HITTEST;
public const uint LVM_ENSUREVISIBLE;
public const uint LVM_SCROLL;
public const uint LVM_REDRAWITEMS;
public const uint LVM_ARRANGE;
public const uint LVM_EDITLABELA;
public const uint LVM_GETEDITCONTROL;
public const uint LVM_GETCOLUMNA;
public const uint LVM_SETCOLUMNA;
public const uint LVM_INSERTCOLUMNA;
public const uint LVM_DELETECOLUMN;
public const uint LVM_GETCOLUMNWIDTH;
public const uint LVM_SETCOLUMNWIDTH;
public const uint LVM_GETHEADER;
public const uint LVM_CREATEDRAGIMAGE;
public const uint LVM_GETVIEWRECT;
public const uint LVM_GETTEXTCOLOR;
public const uint LVM_SETTEXTCOLOR;
public const uint LVM_GETTEXTBKCOLOR;
public const uint LVM_SETTEXTBKCOLOR;
public const uint LVM_GETTOPINDEX;
public const uint LVM_GETCOUNTPERPAGE;
public const uint LVM_GETORIGIN;
public const uint LVM_UPDATE;
public const uint LVM_SETITEMSTATE;
public const uint LVM_GETITEMSTATE;
public const uint LVM_GETITEMTEXTA;
public const uint LVM_SETITEMTEXTA;
public const uint LVM_SETITEMCOUNT;
public const uint LVM_SORTITEMS;
public const uint LVM_SETITEMPOSITION32;
public const uint LVM_GETSELECTEDCOUNT;
public const uint LVM_GETITEMSPACING;
public const uint LVM_GETISEARCHSTRINGA;
public const uint LVM_SETICONSPACING;
public const uint LVM_SETEXTENDEDLISTVIEWSTYLE;
public const uint LVM_GETEXTENDEDLISTVIEWSTYLE;
public const uint LVM_GETSUBITEMRECT;
public const uint LVM_SUBITEMHITTEST;
public const uint LVM_SETCOLUMNORDERARRAY;
public const uint LVM_GETCOLUMNORDERARRAY;
public const uint LVM_SETHOTITEM;
public const uint LVM_GETHOTITEM;
public const uint LVM_SETHOTCURSOR;
public const uint LVM_GETHOTCURSOR;
public const uint LVM_APPROXIMATEVIEWRECT;
public const uint LVM_SETWORKAREAS;
public const uint LVM_GETSELECTIONMARK;
public const uint LVM_SETSELECTIONMARK;
public const uint LVM_SETBKIMAGEA;
public const uint LVM_GETBKIMAGEA;
public const uint LVM_GETWORKAREAS;
public const uint LVM_SETHOVERTIME;
public const uint LVM_GETHOVERTIME;
public const uint LVM_GETNUMBEROFWORKAREAS;
public const uint LVM_SETTOOLTIPS;
public const uint LVM_GETITEMW;
public const uint LVM_SETITEMW;
public const uint LVM_INSERTITEMW;
public const uint LVM_GETTOOLTIPS;
public const uint LVM_FINDITEMW;
public const uint LVM_GETSTRINGWIDTHW;
public const uint LVM_GETCOLUMNW;
public const uint LVM_SETCOLUMNW;
public const uint LVM_INSERTCOLUMNW;
public const uint LVM_GETITEMTEXTW;
public const uint LVM_SETITEMTEXTW;
public const uint LVM_GETISEARCHSTRINGW;
public const uint LVM_EDITLABELW;
public const uint LVM_GETBKIMAGEW;
public const uint LVM_SETSELECTEDCOLUMN;
public const uint LVM_SETTILEWIDTH;
public const uint LVM_SETVIEW;
public const uint LVM_GETVIEW;
public const uint LVM_INSERTGROUP;
public const uint LVM_SETGROUPINFO;
public const uint LVM_GETGROUPINFO;
public const uint LVM_REMOVEGROUP;
public const uint LVM_MOVEGROUP;
public const uint LVM_MOVEITEMTOGROUP;
public const uint LVM_SETGROUPMETRICS;
public const uint LVM_GETGROUPMETRICS;
public const uint LVM_ENABLEGROUPVIEW;
public const uint LVM_SORTGROUPS;
public const uint LVM_INSERTGROUPSORTED;
public const uint LVM_REMOVEALLGROUPS;
public const uint LVM_HASGROUP;
public const uint LVM_SETTILEVIEWINFO;
public const uint LVM_GETTILEVIEWINFO;
public const uint LVM_SETTILEINFO;
public const uint LVM_GETTILEINFO;
public const uint LVM_SETINSERTMARK;
public const uint LVM_GETINSERTMARK;
public const uint LVM_INSERTMARKHITTEST;
public const uint LVM_GETINSERTMARKRECT;
public const uint LVM_SETINSERTMARKCOLOR;
public const uint LVM_GETINSERTMARKCOLOR;
public const uint LVM_SETINFOTIP;
public const uint LVM_GETSELECTEDCOLUMN;
public const uint LVM_ISGROUPVIEWENABLED;
public const uint LVM_GETOUTLINECOLOR;
public const uint LVM_SETOUTLINECOLOR;
public const uint LVM_CANCELEDITLABEL;
public const uint LVM_MAPINDEXTOID;
public const uint LVM_MAPIDTOINDEX;
public const uint LVM_ISITEMVISIBLE;
public const uint LVM_GETEMPTYTEXT;
public const uint LVM_GETFOOTERRECT;
public const uint LVM_GETFOOTERINFO;
public const uint LVM_GETFOOTERITEMRECT;
public const uint LVM_GETFOOTERITEM;
public const uint LVM_GETITEMINDEXRECT;
public const uint LVM_SETITEMINDEXSTATE;
public const uint LVM_GETNEXTITEMINDEX;
public const uint OCM__BASE;
public const uint LVM_SETUNICODEFORMAT;
public const uint LVM_GETUNICODEFORMAT;
public const uint OCM_CTLCOLOR;
public const uint OCM_DRAWITEM;
public const uint OCM_MEASUREITEM;
public const uint OCM_DELETEITEM;
public const uint OCM_VKEYTOITEM;
public const uint OCM_CHARTOITEM;
public const uint OCM_COMPAREITEM;
public const uint OCM_NOTIFY;
public const uint OCM_COMMAND;
public const uint OCM_HSCROLL;
public const uint OCM_VSCROLL;
public const uint OCM_CTLCOLORMSGBOX;
public const uint OCM_CTLCOLOREDIT;
public const uint OCM_CTLCOLORLISTBOX;
public const uint OCM_CTLCOLORBTN;
public const uint OCM_CTLCOLORDLG;
public const uint OCM_CTLCOLORSCROLLBAR;
public const uint OCM_CTLCOLORSTATIC;
public const uint OCM_PARENTNOTIFY;
public const uint WM_APP;
public const uint WM_RASDIALEVENT;
public const uint WM_CLIPBOARDUPDATE;
