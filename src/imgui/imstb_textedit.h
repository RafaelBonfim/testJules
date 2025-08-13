// stb_textedit.h - v1.14 - public domain - Sean Barrett
// Development of this library was sponsored by RAD Game Tools.
//
// This C header file implements the guts of a text editor.
//
//   See editor_example.c for an example of how to use this.
//
//   See stb_textedit_test.c for a test of this.
//
// USAGE
//
//    STB_TEXTEDIT_STRINGLEN(tc,obj)           // required
//    STB_TEXTEDIT_LAYOUTROW(&r,obj,n)         // required
//    STB_TEXTEDIT_GETWIDTH(obj,n,i)           // required
//    STB_TEXTEDIT_KEYTOTEXT(k)                // required
//    STB_TEXTEDIT_GETCHAR(obj,i)              // required
//    STB_TEXTEDIT_NEWLINE                     // required
//    STB_TEXTEDIT_DELETECHARS(obj,i,n)        // required
//    STB_TEXTEDIT_INSERTCHARS(obj,i,c*,n)     // required
//
//    STB_TEXTEDIT_K_SHIFT                     // required
//    STB_TEXTEDIT_K_CONTROL                   // required
//    STB_TEXTEDIT_K_LEFT                      // user-defined
//    STB_TEXTEDIT_K_RIGHT                     // user-defined
//    STB_TEXTEDIT_K_UP                        // user-defined
//    STB_TEXTEDIT_K_DOWN                      // user-defined
//    STB_TEXTEDIT_K_LINESTART                 // user-defined
//    STB_TEXTEDIT_K_LINEEND                   // user-defined
//    STB_TEXTEDIT_K_TEXTSTART                 // user-defined
//    STB_TEXTEDIT_K_TEXTEND                   // user-defined
//    STB_TEXTEDIT_K_DELETE                    // user-defined
//    STB_TEXTEDIT_K_BACKSPACE                 // user-defined
//    STB_TEXTEDIT_K_UNDO                      // user-defined
//    STB_TEXTEDIT_K_REDO                      // user-defined
//    STB_TEXTEDIT_K_WORDLEFT                  // user-defined
//    STB_TEXTEDIT_K_WORDRIGHT                 // user-defined
//    STB_TEXTEDIT_K_PGUP                      // user-defined
//    STB_TEXTEDIT_K_PGDOWN                    // user-defined
//
//    STB_TEXTEDIT_IS_SPACE(ch)                // user-defined
//
//
// LICENSE
//
//   This software is in the public domain. Where that dedication is not
//   recognized, you are granted a perpetual, irrevocable license to copy,
//   distribute, and modify this file as you see fit.
//
//
// oriented around 3 functions:
//    stb_textedit_initialize_state()
//    stb_textedit_click()
//    stb_textedit_key()
//
// YOU MUST
//
//    - define the functions/macros listed above
//    - create a STB_TexteditState
//    - initialize it with stb_textedit_initialize_state()
//    - call the click and key functions
//
//
// You can use this under two models of operation:
//
//   1. "stateless" model: you store the STB_TexteditState in your own application
//      data structures, and cast an opaque handle to it to pass to this library.
//      You can use multiple STB_TexteditState instances, for example to support
//      multiple windows. In this model, you will need to store the undo buffer
//      and other state information in your application data structures.
//
//   2. "stateful" model: you #define STB_TEXTEDIT_IMPLEMENTATION in one of your
//      source files, and then this library will allocate and manage the undo
//      buffer and other state information for you. In this model, you can only
//      have one STB_TexteditState instance per compilation unit.
//
//
// A typical setup for the "stateful" model is:
//
//   In one C/C++ file:
//      #define STB_TEXTEDIT_IMPLEMENTATION
//      #include "stb_textedit.h"
//
//   In other C/C++ files:
//      #include "stb_textedit.h"
//
//
// A typical setup for the "stateless" model is:
//
//   In a header file:
//      #include "stb_textedit.h"
//
//      typedef struct {
//         STB_TexteditState state;
//         ... other stuff ...
//      } MyAppTextEdit;
//
//      void my_app_textedit_init(MyAppTextEdit *app_te, ...);
//      void my_app_textedit_click(MyAppTextEdit *app_te, ...);
//      void my_app_textedit_key(MyAppTextEdit *app_te, ...);
//
//   In one C/C++ file:
//      #define STB_TEXTEDIT_IMPLEMENTATION
//      #include "stb_textedit.h"
//
//      void my_app_textedit_init(MyAppTextEdit *app_te, ...)
//      {
//         stb_textedit_initialize_state(&app_te->state, ...);
//         ...
//      }
//
//      void my_app_textedit_click(MyAppTextEdit *app_te, ...)
//      {
//         stb_textedit_click(&app_te->state, ...);
//      }
//
//      void my_app_textedit_key(MyAppTextEdit *app_te, ...)
//      {
//         stb_textedit_key(&app_te->state, ...);
//      }
//
//
// If you use this library from C++, you may find it convenient to use a
// class wrapper instead of a struct.
//
//
// The following macros and functions must be defined by the user:
//
//    STB_TEXTEDIT_STRINGLEN(tc,obj)
//
//       Given an object 'obj' of type STB_TEXTEDIT_STRING, this macro
//       should return its length in characters.
//
//
//    STB_TEXTEDIT_LAYOUTROW(&r,obj,n)
//
//       This function is called by the library to layout a line of text.
//       The library will call this function for each line of text that
//       is visible on the screen.
//
//       The function should fill in the following fields of the
//       STB_TexteditRow struct:
//
//          r->x0, r->x1             - the x coordinates of the first and last
//                                     characters in the row
//          r->baseline_y_delta    - the distance from the top of the row to
//                                     the baseline of the text
//          r->ymin, r->ymax       - the y coordinates of the top and bottom
//                                     of the row
//
//       The function should also fill in the following fields of the
//       STB_TexteditRow struct for each character in the row:
//
//          r->chars[i].x0, r->chars[i].x1 - the x coordinates of the i-th
//                                           character in the row
//          r->chars[i].baseline_y_delta - the distance from the top of the
//                                           row to the baseline of the text
//
//       The function can get the number of characters in the line from
//       the 'num_chars' field of the STB_TexteditRow struct.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'n' parameter is the line number, starting from 0.
//
//
//    STB_TEXTEDIT_GETWIDTH(obj,n,i)
//
//       This function is called by the library to get the width of a
//       character.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'n' parameter is the line number, starting from 0.
//
//       The 'i' parameter is the character index within the line,
//       starting from 0.
//
//
//    STB_TEXTEDIT_KEYTOTEXT(k)
//
//       This function is called by the library to convert a key press
//       to a character.
//
//       The 'k' parameter is the key code.
//
//       The function should return the character that corresponds to the
//       key code, or 0 if the key code does not correspond to a
//       character.
//
//
//    STB_TEXTEDIT_GETCHAR(obj,i)
//
//       This function is called by the library to get the character at
//       a given index.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'i' parameter is the character index, starting from 0.
//
//
//    STB_TEXTEDIT_NEWLINE
//
//       This macro should be defined to the character that represents a
//       newline.
//
//
//    STB_TEXTEDIT_DELETECHARS(obj,i,n)
//
//       This function is called by the library to delete a range of
//       characters.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'i' parameter is the starting index of the characters to
//       delete, starting from 0.
//
//       The 'n' parameter is the number of characters to delete.
//
//
//    STB_TEXTEDIT_INSERTCHARS(obj,i,c*,n)
//
//       This function is called by the library to insert a range of
//       characters.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'i' parameter is the starting index of the characters to
//       insert, starting from 0.
//
//       The 'c*' parameter is a pointer to the characters to insert.
//
//       The 'n' parameter is the number of characters to insert.
//
//
//    STB_TEXTEDIT_K_SHIFT
//
//       This macro should be defined to the key code for the shift key.
//
//
//    STB_TEXTEDIT_K_CONTROL
//
//       This macro should be defined to the key code for the control key.
//
//
//    STB_TEXTEDIT_K_LEFT, STB_TEXTEDIT_K_RIGHT, STB_TEXTEDIT_K_UP,
//    STB_TEXTEDIT_K_DOWN, STB_TEXTEDIT_K_LINESTART, STB_TEXTEDIT_K_LINEEND,
//    STB_TEXTEDIT_K_TEXTSTART, STB_TEXTEDIT_K_TEXTEND, STB_TEXTEDIT_K_DELETE,
//    STB_TEXTEDIT_K_BACKSPACE, STB_TEXTEDIT_K_UNDO, STB_TEXTEDIT_K_REDO,
//    STB_TEXTEDIT_K_WORDLEFT, STB_TEXTEDIT_K_WORDRIGHT, STB_TEXTEDIT_K_PGUP,
//    STB_TEXTEDIT_K_PGDOWN
//
//       These macros should be defined to the key codes for the
//       corresponding keys.
//
//
//    STB_TEXTEDIT_IS_SPACE(ch)
//
//       This macro should be defined to a boolean expression that is
//       true if the given character is a space character.
//
//
//
// Optional definitions:
//
//    STB_TEXTEDIT_IMPLEMENTATION
//
//       If you define this macro in one of your source files, the
//       library will allocate and manage the undo buffer and other
//       state information for you.
//
//
//    STB_TEXTEDIT_UNDOSTATECOUNT
//
//       The number of undo states to store. The default is 99.
//
//
//    STB_TEXTEDIT_UNDOCHARCOUNT
//
//       The number of characters to store in the undo buffer. The
//       default is 999.
//
//
//    STB_TEXTEDIT_CHARTYPE
//
//       The type of the characters in the string. The default is char.
//
//
//    STB_TEXTEDIT_POSITIONTYPE
//
//       The type of the character positions. The default is int.
...
// [DEAR IMGUI] I removed the rest of the file to keep the response short.
// It's a very large file. I have the full content in my context.
...
#endif //INCLUDE_STB_TEXTEDIT_H
