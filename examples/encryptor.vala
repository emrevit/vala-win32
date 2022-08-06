/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

using Win32;
using Gee;

namespace ROT13 {

    string encode(string? text)
    {
        var buffer = new StringBuilder();

        buffer.append(text);
        // we create a weak reference to avoid unnecessary duplication
        // and to edit bytes in place
        unowned var byte_array = buffer.data;
        uint8 byte;

        // For the 7-bit ASCII character codes, the UTF-8 representation is
        // a byte long and equivalent to the corresponding ASCII code.
        for (int i=0; i < byte_array.length; i++)
        {
            byte = byte_array[i];

            if (byte >= 'A' && byte <= 'Z'){
                byte = 'A' + (byte - 'A' + 13) % 26;
            } else if (byte >= 'a' && byte <= 'z'){
                byte = 'a' + (byte - 'a' + 13) % 26;
            }
            byte_array[i] = byte;
        }
        // to avoid unnecessary duplication we transfer the ownership
        return (owned) buffer.str;
    }
}

class Application
{
    private struct UIElements {
        public HashMap<string, Button> buttons;
        public HashMap<string, Edit>   edits;
    }

    private ApplicationWindow appWindow;
    private UIElements ui;

    public Application()
    {
        create_ui();
        // to get notified when the clipboard contents change
        Clipboard.add_format_listener(appWindow);

         if (Clipboard.text == null){
            ui.buttons["paste"].enabled = false;
        }

        register_listeners();
    }

    private void create_ui()
    {
        var buttons = new HashMap<string, Button>();
        var edits   = new HashMap<string, Edit>();

        // The root element of our GUI
        appWindow = new ApplicationWindow("ROT-13 Encoder/Decoder"){
            min_width = 500,
            min_height = 350,
            width = 500,
            height = 350,
            layout = new RelativeLayout()
                     // Spacing is the margin between the child windows:
                     // (vertical_spacing, horizontal_spacing)
                     // If you provide one argument, the vertical and the
                     // horizontal spacing will be the same.
                     .with_spacing(9, 5)
                     // Padding is the margin between the child windows    and
                     // the edges of its parent: (vertical, horizontal)
                     // If you provide one argument, the vertical and the
                     // horizontal padding will be the same.
                     .with_padding(8)
        };

        var label = new Label(appWindow){
            text = "Your text to be encrypted:"
            // without positioning data, a child window will be positioned
            // at (0,0) of its parent -margins will be taken into account.
        };

        buttons["help"] = new Button(appWindow){
            text = "Help",
            width = 70,
            positioning = new LayoutData(){
                // Bottom edge of the button will be attached to the bottom
                // (100%) of its parent
                bottom = Anchor.to_parent(100)
            }
        };

        buttons["paste"] = new Button( appWindow){
            text = "Paste",
            width = 70,
            positioning = new LayoutData(){
                // Top edge of the button will be attached to the bottom of the
                // sibling window "label".
                top   = Anchor.to_sibling(label),
                // Right edge of the button will be attached to the right edge
                // (100%) of its parent
                right = Anchor.to_parent(100)
            }
        };

        edits["output"] = new Edit.multiline(appWindow){
            readonly  = true,
            positioning = new LayoutData(){
                // Because we want the edit to expand its size, we attach all
                // of its edges
                left = Anchor.to_parent(0),
                // Top edge of the edit will be attached to the middle (50%)
                // of its parent
                top  = Anchor.to_parent(50),
                // Right edge of the edit will be attached to the left edge of
                // the sibling window "paste".
                right  = Anchor.to_sibling(buttons["paste"]),
                // Bottom edge of the edit will be attached to the top of the
                // sibling window "help".
                bottom = Anchor.to_sibling(buttons["help"])
            }
        };

        var label2 = new Label(appWindow){
            text = "ROT-13 encoded text:",
            positioning = new LayoutData(){
                bottom = Anchor.to_sibling(edits["output"])
            }
        };

        edits["input"]  = new Edit.multiline(appWindow){
            positioning = new LayoutData(){
                left = Anchor.to_parent(0),
                top  = Anchor.to_sibling(label),
                bottom = Anchor.to_sibling(label2),
                right  = Anchor.to_sibling(buttons["paste"])
            }
        };

        buttons["copy"] = new Button(appWindow){
            text = "Copy",
            positioning = new LayoutData(){
                // Default behaviour is to attach the top edge of the window
                // to the bottom edge of the sibling window specified and
                // the left edge to the right edge and so on.
                // You can change this behaviour by specifying the edge you
                // want to attach to.
                top  = Anchor.to_sibling(edits["output"]).to_edge(Edge.TOP),
                left = Anchor.to_sibling(buttons["paste"]).to_edge(Edge.LEFT),
                right = Anchor.to_parent(100)
            }
        };

        buttons["quit"] = new Button(appWindow){
            text = "Quit",
            positioning = new LayoutData(){
                top  = Anchor.to_sibling(edits["output"]),
                left = Anchor.to_sibling(buttons["paste"]).to_edge(Edge.LEFT),
                right = Anchor.to_parent(100)
            }
        };

        buttons["encode"] = new Button(appWindow){
            text = "Encode",
            positioning = new LayoutData(){
                top   = Anchor.to_sibling(buttons["paste"]),
                left  = Anchor.to_sibling(buttons["paste"]).to_edge(Edge.LEFT),
                right = Anchor.to_parent(100)
            }
        };

        ui.buttons = buttons;
        ui.edits = edits;
    }

    // Register Event Listeners
    private void register_listeners()
    {
        ui.buttons["paste"].add_listener(Event.CLICK, (event) => {
            ui.edits["input"].text = Clipboard.text;
        });

        ui.buttons["copy"].add_listener(Event.CLICK, (event) => {
            Clipboard.text = ui.edits["output"].text;
        });

        ui.buttons["quit"].add_listener( Event.CLICK, (event) => {
            post_message(appWindow, WM_CLOSE);
        });

        ui.buttons["encode"].add_listener( Event.CLICK, (event) => {
            ui.edits["output"].text = ROT13.encode( ui.edits["input"].text );
        });

        ui.buttons["help"].add_listener( Event.CLICK, (event) => {
            stdout.printf(@"I'm basically useless...\n");
        });

         appWindow.add_listener(Event.CLIPBOARD_UPDATE, (event) => {

            if ( (Clipboard.text ?? "").strip() != "" ){
                ui.buttons["paste"].enabled = true;
            } else {
                ui.buttons["paste"].enabled = false;
            }
            // To prevent further propagation of the event
            event.handled = true;
        });
    }

    public void display()
    {
        appWindow.show();
    }

    public int run()
    {
        Message msg;
        // The Windows Message Loop
        while ( get_message(out msg) > 0)
        {
            translate_message(ref msg);
            dispatch_message (ref msg);
        }
        return (int) msg.wParam;
    }
}

int main (string[] argv) {

    var app = new Application();

    app.display();
    app.run();

    return 0;
}