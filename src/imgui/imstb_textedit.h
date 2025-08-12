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
//
//
//
// The following functions are defined by the library:
//
//    stb_textedit_initialize_state(state, is_single_line)
//
//       Initializes the given STB_TexteditState.
//
//       The 'is_single_line' parameter should be true if the text edit
//       control is single-line, and false if it is multi-line.
//
//
//    stb_textedit_click(obj, state, x, y)
//
//       This function should be called when the user clicks the mouse
//       in the text edit control.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'state' parameter is a pointer to the STB_TexteditState.
//
//       The 'x' and 'y' parameters are the coordinates of the mouse
//       click, relative to the top-left corner of the text edit
//       control.
//
//
//    stb_textedit_drag(obj, state, x, y)
//
//       This function should be called when the user drags the mouse
//       in the text edit control.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'state' parameter is a pointer to the STB_TexteditState.
//
//       The 'x' and 'y' parameters are the coordinates of the mouse,
//       relative to the top-left corner of the text edit control.
//
//
//    stb_textedit_key(obj, state, key)
//
//       This function should be called when the user presses a key.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'state' parameter is a pointer to the STB_TexteditState.
//
//       The 'key' parameter is the key code of the key that was
//       pressed.
//
//
//    stb_textedit_cut(obj, state)
//
//       This function should be called when the user wants to cut the
//       selected text.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'state' parameter is a pointer to the STB_TexteditState.
//
//
//    stb_textedit_paste(obj, state, text, len)
//
//       This function should be called when the user wants to paste
//       text into the text edit control.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'state' parameter is a pointer to the STB_TexteditState.
//
//       The 'text' parameter is a pointer to the text to paste.
//
//       The 'len' parameter is the length of the text to paste.
//
//
//    stb_textedit_undo(obj, state)
//
//       This function should be called when the user wants to undo the
//       last operation.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'state' parameter is a pointer to the STB_TexteditState.
//
//
//    stb_textedit_redo(obj, state)
//
//       This function should be called when the user wants to redo the
//       last undone operation.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'state' parameter is a pointer to the STB_TexteditState.
//
//
//
//
// The following structs are defined by the library:
//
//    STB_TexteditState
//
//       This struct contains the state of the text edit control.
//
//
//    STB_TexteditRow
//
//       This struct contains information about a single row of text.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION:
//
//    stb_textedit_create_state(is_single_line)
//
//       Creates and initializes a new STB_TexteditState.
//
//
//    stb_textedit_destroy_state(state)
//
//       Destroys the given STB_TexteditState.
//
//
//
//
// The following macros are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION:
//
//    stb_textedit_get_undo_state(state, &len)
//
//       Returns a pointer to the undo state, and sets 'len' to the
//       length of the undo state.
//
//
//    stb_textedit_set_undo_state(state, ptr, len)
//
//       Sets the undo state.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_GETWIDTH_NEW:
//
//    stb_textedit_get_width_new(obj, n, i)
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
//       This function is the same as STB_TEXTEDIT_GETWIDTH, but it
//       returns the width as a float instead of an int.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_LAYOUTROW_NEW:
//
//    stb_textedit_layout_row_new(r, obj, n)
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
//       This function is the same as STB_TEXTEDIT_LAYOUTROW, but the
//       x coordinates are floats instead of ints.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_GETCHAR_NEW:
//
//    stb_textedit_get_char_new(obj, i)
//
//       This function is called by the library to get the character at
//       a given index.
//
//       The 'obj' parameter is the same object that was passed to the
//       stb_textedit_initialize_state() function.
//
//       The 'i' parameter is the character index, starting from 0.
//
//       This function is the same as STB_TEXTEDIT_GETCHAR, but it
//       returns the character as an int instead of a char.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_DELETECHARS_NEW:
//
//    stb_textedit_delete_chars_new(obj, i, n)
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
//       This function is the same as STB_TEXTEDIT_DELETECHARS, but the
//       index and count are ints instead of shorts.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_INSERTCHARS_NEW:
//
//    stb_textedit_insert_chars_new(obj, i, c, n)
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
//       The 'c' parameter is a pointer to the characters to insert.
//
//       The 'n' parameter is the number of characters to insert.
//
//       This function is the same as STB_TEXTEDIT_INSERTCHARS, but the
//       index and count are ints instead of shorts.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_KEYTOTEXT_NEW:
//
//    stb_textedit_key_to_text_new(k)
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
//       This function is the same as STB_TEXTEDIT_KEYTOTEXT, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_STRINGLEN_NEW:
//
//    stb_textedit_string_len_new(tc, obj)
//
//       Given an object 'obj' of type STB_TEXTEDIT_STRING, this macro
//       should return its length in characters.
//
//       This function is the same as STB_TEXTEDIT_STRINGLEN, but the
//       length is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_IS_SPACE_NEW:
//
//    stb_textedit_is_space_new(ch)
//
//       This macro should be defined to a boolean expression that is
//       true if the given character is a space character.
//
//       This function is the same as STB_TEXTEDIT_IS_SPACE, but the
//       character is an int instead of a char.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_SHIFT_NEW:
//
//    stb_textedit_k_shift_new()
//
//       This macro should be defined to the key code for the shift key.
//
//       This function is the same as STB_TEXTEDIT_K_SHIFT, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_CONTROL_NEW:
//
//    stb_textedit_k_control_new()
//
//       This macro should be defined to the key code for the control key.
//
//       This function is the same as STB_TEXTEDIT_K_CONTROL, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_LEFT_NEW:
//
//    stb_textedit_k_left_new()
//
//       This macro should be defined to the key code for the left arrow
//       key.
//
//       This function is the same as STB_TEXTEDIT_K_LEFT, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_RIGHT_NEW:
//
//    stb_textedit_k_right_new()
//
//       This macro should be defined to the key code for the right arrow
//       key.
//
//       This function is the same as STB_TEXTEDIT_K_RIGHT, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_UP_NEW:
//
//    stb_textedit_k_up_new()
//
//       This macro should be defined to the key code for the up arrow
//       key.
//
//       This function is the same as STB_TEXTEDIT_K_UP, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_DOWN_NEW:
//
//    stb_textedit_k_down_new()
//
//       This macro should be defined to the key code for the down arrow
//       key.
//
//       This function is the same as STB_TEXTEDIT_K_DOWN, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_LINESTART_NEW:
//
//    stb_textedit_k_linestart_new()
//
//       This macro should be defined to the key code for the home key.
//
//       This function is the same as STB_TEXTEDIT_K_LINESTART, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_LINEEND_NEW:
//
//    stb_textedit_k_lineend_new()
//
//       This macro should be defined to the key code for the end key.
//
//       This function is the same as STB_TEXTEDIT_K_LINEEND, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_TEXTSTART_NEW:
//
//    stb_textedit_k_textstart_new()
//
//       This macro should be defined to the key code for the control-home
//       key combination.
//
//       This function is the same as STB_TEXTEDIT_K_TEXTSTART, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_TEXTEND_NEW:
//
//    stb_textedit_k_textend_new()
//
//       This macro should be defined to the key code for the control-end
//       key combination.
//
//       This function is the same as STB_TEXTEDIT_K_TEXTEND, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_DELETE_NEW:
//
//    stb_textedit_k_delete_new()
//
//       This macro should be defined to the key code for the delete key.
//
//       This function is the same as STB_TEXTEDIT_K_DELETE, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_BACKSPACE_NEW:
//
//    stb_textedit_k_backspace_new()
//
//       This macro should be defined to the key code for the backspace
//       key.
//
//       This function is the same as STB_TEXTEDIT_K_BACKSPACE, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_UNDO_NEW:
//
//    stb_textedit_k_undo_new()
//
//       This macro should be defined to the key code for the undo
//       operation.
//
//       This function is the same as STB_TEXTEDIT_K_UNDO, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_REDO_NEW:
//
//    stb_textedit_k_redo_new()
//
//       This macro should be defined to the key code for the redo
//       operation.
//
//       This function is the same as STB_TEXTEDIT_K_REDO, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_WORDLEFT_NEW:
//
//    stb_textedit_k_wordleft_new()
//
//       This macro should be defined to the key code for the control-left
//       arrow key combination.
//
//       This function is the same as STB_TEXTEDIT_K_WORDLEFT, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_WORDRIGHT_NEW:
//
//    stb_textedit_k_wordright_new()
//
//       This macro should be defined to the key code for the control-right
//       arrow key combination.
//
//       This function is the same as STB_TEXTEDIT_K_WORDRIGHT, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_PGUP_NEW:
//
//    stb_textedit_k_pgup_new()
//
//       This macro should be defined to the key code for the page up
//       key.
//
//       This function is the same as STB_TEXTEDIT_K_PGUP, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_K_PGDOWN_NEW:
//
//    stb_textedit_k_pgdown_new()
//
//       This macro should be defined to the key code for the page down
//       key.
//
//       This function is the same as STB_TEXTEDIT_K_PGDOWN, but the
//       key code is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_NEWLINE_NEW:
//
//    stb_textedit_newline_new()
//
//       This macro should be defined to the character that represents a
//       newline.
//
//       This function is the same as STB_TEXTEDIT_NEWLINE, but the
//       character is an int instead of a char.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_CHARTYPE_NEW:
//
//    stb_textedit_chartype_new()
//
//       The type of the characters in the string. The default is char.
//
//       This function is the same as STB_TEXTEDIT_CHARTYPE, but the
//       type is an int instead of a char.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_POSITIONTYPE_NEW:
//
//    stb_textedit_positiontype_new()
//
//       The type of the character positions. The default is int.
//
//       This function is the same as STB_TEXTEDIT_POSITIONTYPE, but the
//       type is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_UNDOSTATECOUNT_NEW:
//
//    stb_textedit_undostatecount_new()
//
//       The number of undo states to store. The default is 99.
//
//       This function is the same as STB_TEXTEDIT_UNDOSTATECOUNT, but the
//       count is an int instead of a short.
//
//
//
//
// The following functions are defined by the library if you define
// STB_TEXTEDIT_IMPLEMENTATION and STB_TEXTEDIT_UNDOCHARCOUNT_NEW:
//
//    stb_textedit_undocharcount_new()
//
//       The number of characters to store in the undo buffer. The
//       default is 999.
//
//       This function is the same as STB_TEXTEDIT_UNDOCHARCOUNT, but the
//       count is an int instead of a short.
//

#ifndef INCLUDE_STB_TEXTEDIT_H
#define INCLUDE_STB_TEXTEDIT_H

////////////////////////////////////////////////////////////////////////
//
//     STB_TexteditState
//
//

#ifndef STB_TEXTEDIT_UNDOSTATECOUNT
#define STB_TEXTEDIT_UNDOSTATECOUNT   99
#endif
#ifndef STB_TEXTEDIT_UNDOCHARCOUNT
#define STB_TEXTEDIT_UNDOCHARCOUNT   999
#endif
#ifndef STB_TEXTEDIT_CHARTYPE
#define STB_TEXTEDIT_CHARTYPE       int
#endif
#ifndef STB_TEXTEDIT_POSITIONTYPE
#define STB_TEXTEDIT_POSITIONTYPE   int
#endif

typedef struct
{
   // private data
   STB_TEXTEDIT_POSITIONTYPE  where;
   STB_TEXTEDIT_POSITIONTYPE  insert_length;
   STB_TEXTEDIT_POSITIONTYPE  delete_length;
   int                        char_storage;
} StbUndoRecord;

typedef struct
{
   // private data
   StbUndoRecord          undo_rec[STB_TEXTEDIT_UNDOSTATECOUNT];
   STB_TEXTEDIT_CHARTYPE  undo_char[STB_TEXTEDIT_UNDOCHARCOUNT];
   short                  undo_point;
   short                  redo_point;
   int                    undo_char_point;
   int                    redo_char_point;
} StbUndoState;

typedef struct
{
   /////////////////////
   //
   // public data
   //

   int cursor;
   // position of the text cursor within the string

   int select_start;          // other end of the selection
   int select_end;
   // selection start and end point in characters; if equal, no selection

   unsigned char insert_mode;
   // each textfield keeps its own insert mode state. to keep it alternating,
   // call toggle_insert_mode() over and over

   /////////////////////
   //
   // private data
   //
   unsigned char cursor_at_end_of_line; // not implemented yet
   unsigned char initialized;
   unsigned char has_preferred_x;
   unsigned char single_line;
   unsigned char padding1, padding2, padding3;
   float preferred_x; // this is applied to the cursor on up/down keys
   StbUndoState undostate;
} STB_TexteditState;


////////////////////////////////////////////////////////////////////////
//
//     StbTexteditRow
//
//

typedef struct
{
   float x0,x1;
   // starting x location, final x location (assuming straight text)
   float baseline_y_delta;
   // position of baseline relative to y-coordinate of row
   float ymin,ymax;
   // height of row above and below baseline
   int num_chars;
} StbTexteditRow;
#endif //INCLUDE_STB_TEXTEDIT_H

#ifdef STB_TEXTEDIT_IMPLEMENTATION

#ifndef STB_TEXTEDIT_memmove
#include <string.h>
#define STB_TEXTEDIT_memmove memmove
#endif


/////////////////////////////////////////////////////////////////////////////
//
//      Mouse input handling
//

// traverse the layout to locate the nearest character to a display position
static int stb_text_locate_coord(STB_TEXTEDIT_STRING *str, float x, float y)
{
   StbTexteditRow r;
   int n = STB_TEXTEDIT_STRINGLEN(str);
   float S = STB_TEXTEDIT_GETSCROLL_Y(str);
   float E = STB_TEXTEDIT_GETSCROLL_Y(str) + STB_TEXTEDIT_GETHEIGHT(str);
   int i,j;
   int best_line=-1;
   float best_dist=999999;

   // search for the line nearest to y
   for (i=0; i < n; ++i) {
      STB_TEXTEDIT_LAYOUTROW(&r, str, i);
      if (r.ymin > y && r.ymax < y) {
         best_line = i;
         best_dist = 0;
         break;
      }
      if (y < r.ymin) {
         if (r.ymin - y < best_dist) {
            best_dist = r.ymin - y;
            best_line = i;
         }
      } else {
         if (y - r.ymax < best_dist) {
            best_dist = y - r.ymax;
            best_line = i;
         }
      }
   }
   if (best_line < 0)
      return n;

   // now search along the line for x
   STB_TEXTEDIT_LAYOUTROW(&r, str, best_line);
   best_dist = 999999;
   j = 0;
   for (i=0; i < r.num_chars; ++i) {
      if (x < r.chars[i].x1) {
         if (x - r.chars[i].x0 < best_dist) {
            best_dist = x - r.chars[i].x0;
            j = i;
         }
         if (r.chars[i].x1 - x < best_dist) {
            best_dist = r.chars[i].x1 - x;
            j = i+1;
         }
      }
   }
   return j;
}

// API click: on mouse down, move the cursor to the click point
static void stb_textedit_click(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, float x, float y)
{
   state->cursor = stb_text_locate_coord(str, x, y);
   state->select_start = state->cursor;
   state->select_end   = state->cursor;
   state.has_preferred_x = 0;
}

// API drag: on mouse drag, move the cursor and selection endpoint to the drag point
static void stb_textedit_drag(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, float x, float y)
{
   int p = stb_text_locate_coord(str, x, y);
   if (state->select_start == state->cursor)
      state->select_start = p;
   state->cursor = state->select_end = p;
}

/////////////////////////////////////////////////////////////////////////////
//
//      Keyboard input handling
//

// forward declarations
static void stb_text_undo(STB_TEXTEDIT_STRING *str, STB_TexteditState *state);
static void stb_text_redo(STB_TEXTEDIT_STRING *str, STB_TexteditState *state);
static void stb_text_makeundo_delete(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int length);
static void stb_text_makeundo_insert(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int length);
static void stb_text_makeundo_replace(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int old_length, int new_length);

typedef struct
{
   float x,y;    // position of cursor
   float height; // height of character
   int   line;
   int   first_char, length; // first char of line, and length
} StbFindState;

// find the line, and the position on the line, of a given character
static void stb_text_find_charpos(StbFindState *find, STB_TEXTEDIT_STRING *str, int n, int single_line)
{
   StbTexteditRow r;
   int i,len;
   if (single_line) {
      STB_TEXTEDIT_LAYOUTROW(&r, str, 0);
      find->line = 0;
      find->first_char = 0;
      find->length = STB_TEXTEDIT_STRINGLEN(str);
      find->x = r.x0;
      find->y = r.y0;
      find->height = r.ymax - r.ymin;
      for (i=0; i < n; ++i)
         find->x += STB_TEXTEDIT_GETWIDTH(str, 0, i);
   } else {
      len = STB_TEXTEDIT_STRINGLEN(str);
      if (n > len) n = len;
      find->line = -1;
      find->first_char = 0;
      find->length = 0;
      find->x = find->y = 0;
      find->height = 0;
      for (i=0; i < len; ) {
         STB_TEXTEDIT_LAYOUTROW(&r, str, find->line+1);
         if (n >= i && n < i + r.num_chars) {
            find->line++;
            find->first_char = i;
            find->length = r.num_chars;
            find->x = r.x0;
            find->y = r.y0;
            find->height = r.ymax - r.ymin;
            for (i=0; i < n-find->first_char; ++i)
               find->x += STB_TEXTEDIT_GETWIDTH(str, find->line, i);
            return;
         }
         i += r.num_chars;
         if (i < len && STB_TEXTEDIT_GETCHAR(str, i) == STB_TEXTEDIT_NEWLINE)
            ++i;
      }
      // if n is end of file, find is correct
      find->line++;
      find->first_char = i;
      find->length = 0;
      find->x = r.x0;
      find->y = r.y0;
      find->height = r.ymax - r.ymin;
   }
}

#define STB_TEXT_HAS_SELECTION(s)   ((s)->select_start != (s)->select_end)

// make sure there's a selection
static void stb_text_clamp(STB_TEXTEDIT_STRING *str, STB_TexteditState *state)
{
   int n = STB_TEXTEDIT_STRINGLEN(str);
   if (STB_TEXT_HAS_SELECTION(state)) {
      if (state->select_start > n) state->select_start = n;
      if (state->select_end   > n) state->select_end = n;
      // if clamping forced them to be equal, move the cursor to that place
      if (state->select_start == state->select_end)
         state->cursor = state->select_start;
   }
   if (state->cursor > n) state->cursor = n;
}

// delete the selection
static void stb_text_delete_selection(STB_TEXTEDIT_STRING *str, STB_TexteditState *state)
{
   stb_text_clamp(str, state);
   if (STB_TEXT_HAS_SELECTION(state)) {
      if (state->select_start < state->select_end) {
         stb_text_makeundo_delete(str, state, state->select_start, state->select_end - state->select_start);
         STB_TEXTEDIT_DELETECHARS(str, state->select_start, state->select_end - state->select_start);
         state->cursor = state->select_start;
      } else {
         stb_text_makeundo_delete(str, state, state->select_end, state->select_start - state->select_end);
         STB_TEXTEDIT_DELETECHARS(str, state->select_end, state->select_start - state->select_end);
         state->cursor = state->select_end;
      }
      state->select_end = state->select_start = state->cursor;
      state->has_preferred_x = 0;
   }
}

// canoncialize the selection so start <= end
static void stb_text_sortselection(STB_TexteditState *state)
{
   if (state->select_end < state->select_start) {
      int temp = state->select_end;
      state->select_end = state->select_start;
      state->select_start = temp;
   }
}

// move cursor to first character of selection
static void stb_text_move_to_first(STB_TexteditState *state)
{
   if (STB_TEXT_HAS_SELECTION(state)) {
      stb_text_sortselection(state);
      state->cursor = state->select_start;
      state->select_end = state->select_start;
      state->has_preferred_x = 0;
   }
}

// move cursor to last character of selection
static void stb_text_move_to_last(STB_TexteditState *state)
{
   if (STB_TEXT_HAS_SELECTION(state)) {
      stb_text_sortselection(state);
      state->cursor = state->select_end;
      state->select_start = state->select_end;
      state->has_preferred_x = 0;
   }
}

#ifdef STB_TEXTEDIT_IS_SPACE
static int is_word_boundary( STB_TEXTEDIT_STRING *str, int idx )
{
   return idx > 0 ? (STB_TEXTEDIT_IS_SPACE( STB_TEXTEDIT_GETCHAR(str,idx-1) ) && !STB_TEXTEDIT_IS_SPACE( STB_TEXTEDIT_GETCHAR(str, idx) ) ) : 1;
}

#ifndef STB_TEXTEDIT_MOVEWORDLEFT
static int stb_textedit_move_word_left_internal(STB_TEXTEDIT_STRING *str, int idx)
{
   idx--;
   while(idx >= 0 && !is_word_boundary(str,idx))
      idx--;

   if (idx < 0)
      idx=0;

   return idx;
}
#define STB_TEXTEDIT_MOVEWORDLEFT(str,idx) stb_textedit_move_word_left_internal(str,idx)
#endif

#ifndef STB_TEXTEDIT_MOVEWORDRIGHT
static int stb_textedit_move_word_right_internal(STB_TEXTEDIT_STRING *str, int idx)
{
   int len = STB_TEXTEDIT_STRINGLEN(str);
   idx++;
   while(idx < len && !is_word_boundary(str,idx))
      idx++;

   if (idx > len)
      idx=len;

   return idx;
}
#define STB_TEXTEDIT_MOVEWORDRIGHT(str,idx) stb_textedit_move_word_right_internal(str,idx)
#endif

#endif

// update the preferred x value for the cursor
static void stb_text_prep_selection_at_cursor(STB_TexteditState *state)
{
   if (!STB_TEXT_HAS_SELECTION(state))
      state->select_start = state->select_end = state->cursor;
   else
      state->cursor = state->select_end;
}

// API key: process a keyboard input
static int stb_textedit_key(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int key)
{
   switch (key) {
      default: {
         int c = STB_TEXTEDIT_KEYTOTEXT(key);
         if (c > 0) {
            STB_TEXTEDIT_CHARTYPE ch = (STB_TEXTEDIT_CHARTYPE) c;

            // can't add newline in single-line mode
            if (ch == STB_TEXTEDIT_NEWLINE && state->single_line)
               break;

            if (state->insert_mode && !STB_TEXT_HAS_SELECTION(state) && state->cursor < STB_TEXTEDIT_STRINGLEN(str)) {
               stb_text_makeundo_replace(str, state, state->cursor, 1, 1);
               STB_TEXTEDIT_DELETECHARS(str, state->cursor, 1);
               STB_TEXTEDIT_INSERTCHARS(str, state->cursor, &ch, 1);
               ++state->cursor;
               state->has_preferred_x = 0;
               return 1;
            } else {
               stb_text_delete_selection(str,state);
               stb_text_makeundo_insert(str, state, state->cursor, 1);
               if (STB_TEXTEDIT_INSERTCHARS(str, state->cursor, &ch, 1)) {
                  ++state->cursor;
                  state->has_preferred_x = 0;
                  return 1;
               }
            }
         }
         break;
      }

      #ifdef STB_TEXTEDIT_K_PGUP
      case STB_TEXTEDIT_K_PGUP: {
         StbFindState find;
         stb_text_find_charpos(&find, str, state->cursor, state->single_line);
         stb_textedit_click(str, state, find.x, find.y - STB_TEXTEDIT_GETHEIGHT(str));
         if (STB_TEXT_HAS_SELECTION(state))
            state->select_end = state->cursor;
         return 1;
      }
      #endif

      #ifdef STB_TEXTEDIT_K_PGDOWN
      case STB_TEXTEDIT_K_PGDOWN: {
         StbFindState find;
         stb_text_find_charpos(&find, str, state->cursor, state->single_line);
         stb_textedit_click(str, state, find.x, find.y + STB_TEXTEDIT_GETHEIGHT(str));
         if (STB_TEXT_HAS_SELECTION(state))
            state->select_end = state->cursor;
         return 1;
      }
      #endif

      #ifdef STB_TEXTEDIT_K_LEFT
      case STB_TEXTEDIT_K_LEFT:
         if (STB_TEXT_HAS_SELECTION(state) && !(key & STB_TEXTEDIT_K_SHIFT))
            stb_text_move_to_first(state);
         else if (state->cursor > 0)
            --state->cursor;
         if (! (key & STB_TEXTEDIT_K_SHIFT))
            state->select_start = state->select_end = state->cursor;
         state->has_preferred_x = 0;
         return 1;
      #endif

      #ifdef STB_TEXTEDIT_K_RIGHT
      case STB_TEXTEDIT_K_RIGHT:
         if (STB_TEXT_HAS_SELECTION(state) && !(key & STB_TEXTEDIT_K_SHIFT))
            stb_text_move_to_last(state);
         else {
            int n = STB_TEXTEDIT_STRINGLEN(str);
            if (state->cursor < n)
               ++state->cursor;
         }
         if (! (key & STB_TEXTEDIT_K_SHIFT))
            state->select_start = state->select_end = state->cursor;
         state->has_preferred_x = 0;
         return 1;
      #endif

      #ifdef STB_TEXTEDIT_K_WORDLEFT
      case STB_TEXTEDIT_K_WORDLEFT:
         if (STB_TEXT_HAS_SELECTION(state) && !(key & STB_TEXTEDIT_K_SHIFT))
            stb_text_move_to_first(state);
         else {
            state->cursor = STB_TEXTEDIT_MOVEWORDLEFT(str, state->cursor);
            stb_text_clamp(str, state);
         }
         if (! (key & STB_TEXTEDIT_K_SHIFT))
            state->select_start = state->select_end = state->cursor;
         state->has_preferred_x = 0;
         return 1;
      #endif

      #ifdef STB_TEXTEDIT_K_WORDRIGHT
      case STB_TEXTEDIT_K_WORDRIGHT:
         if (STB_TEXT_HAS_SELECTION(state) && !(key & STB_TEXTEDIT_K_SHIFT))
            stb_text_move_to_last(state);
         else {
            state->cursor = STB_TEXTEDIT_MOVEWORDRIGHT(str, state->cursor);
            stb_text_clamp(str, state);
         }
         if (! (key & STB_TEXTEDIT_K_SHIFT))
            state->select_start = state->select_end = state->cursor;
         state->has_preferred_x = 0;
         return 1;
      #endif

      #ifdef STB_TEXTEDIT_K_LINESTART
      case STB_TEXTEDIT_K_LINESTART: {
         StbFindState find;
         stb_text_find_charpos(&find, str, state->cursor, state->single_line);
         state->cursor = find.first_char;
         if (! (key & STB_TEXTEDIT_K_SHIFT))
            state->select_start = state->select_end = state->cursor;
         state->has_preferred_x = 0;
         return 1;
      }
      #endif

      #ifdef STB_TEXTEDIT_K_LINEEND
      case STB_TEXTEDIT_K_LINEEND: {
         StbFindState find;
         stb_text_find_charpos(&find, str, state->cursor, state->single_line);
         state->cursor = find.first_char + find.length;
         if (! (key & STB_TEXTEDIT_K_SHIFT))
            state->select_start = state->select_end = state->cursor;
         state->has_preferred_x = 0;
         return 1;
      }
      #endif

      #ifdef STB_TEXTEDIT_K_TEXTSTART
      case STB_TEXTEDIT_K_TEXTSTART:
         state->cursor = state->select_start = state->select_end = 0;
         state->has_preferred_x = 0;
         return 1;
      #endif

      #ifdef STB_TEXTEDIT_K_TEXTEND
      case STB_TEXTEDIT_K_TEXTEND:
         state->cursor = STB_TEXTEDIT_STRINGLEN(str);
         if (! (key & STB_TEXTEDIT_K_SHIFT))
            state->select_start = state->select_end = state->cursor;
         state->has_preferred_x = 0;
         return 1;
      #endif

      #ifdef STB_TEXTEDIT_K_DELETE
      case STB_TEXTEDIT_K_DELETE:
         if (STB_TEXT_HAS_SELECTION(state))
            stb_text_delete_selection(str,state);
         else {
            int n = STB_TEXTEDIT_STRINGLEN(str);
            if (state->cursor < n) {
               stb_text_makeundo_delete(str, state, state->cursor, 1);
               STB_TEXTEDIT_DELETECHARS(str, state->cursor, 1);
            }
         }
         state->has_preferred_x = 0;
         return 1;
      #endif

      #ifdef STB_TEXTEDIT_K_BACKSPACE
      case STB_TEXTEDIT_K_BACKSPACE:
         if (STB_TEXT_HAS_SELECTION(state))
            stb_text_delete_selection(str,state);
         else {
            stb_text_clamp(str, state);
            if (state->cursor > 0) {
               stb_text_makeundo_delete(str, state, state->cursor-1, 1);
               STB_TEXTEDIT_DELETECHARS(str, state->cursor-1, 1);
               --state->cursor;
            }
         }
         state->has_preferred_x = 0;
         return 1;
      #endif

      #ifdef STB_TEXTEDIT_K_UNDO
      case STB_TEXTEDIT_K_UNDO:
         stb_text_undo(str, state);
         state->has_preferred_x = 0;
         return 1;
      #endif

      #ifdef STB_TEXTEDIT_K_REDO
      case STB_TEXTEDIT_K_REDO:
         stb_text_redo(str, state);
         state->has_preferred_x = 0;
         return 1;
      #endif

      case STB_TEXTEDIT_K_INSERT:
         state->insert_mode = !state->insert_mode;
         return 1;

      case STB_TEXTEDIT_K_UP: {
         StbFindState find;
         stb_text_find_charpos(&find, str, state->cursor, state->single_line);
         if (find.line > 0) {
            if (state->has_preferred_x)
               stb_textedit_click(str, state, state->preferred_x, find.y - find.height);
            else {
               stb_textedit_click(str, state, find.x, find.y - find.height);
               state->preferred_x = find.x;
            }
            state->has_preferred_x = 1;
         }
         if (!(key & STB_TEXTEDIT_K_SHIFT))
            state->select_end = state->cursor;
         return 1;
      }

      case STB_TEXTEDIT_K_DOWN: {
         StbFindState find;
         stb_text_find_charpos(&find, str, state->cursor, state->single_line);
         if (find.line < STB_TEXTEDIT_STRINGLEN(str)-1) {
            if (state->has_preferred_x)
               stb_textedit_click(str, state, state->preferred_x, find.y + find.height);
            else {
               stb_textedit_click(str, state, find.x, find.y + find.height);
               state->preferred_x = find.x;
            }
            state->has_preferred_x = 1;
         }
         if (! (key & STB_TEXTEDIT_K_SHIFT))
            state->select_end = state->cursor;
         return 1;
      }
   }
   return 0;
}

static void stb_textedit_flush_redo(StbUndoState *state)
{
   state->redo_point = STB_TEXTEDIT_UNDOSTATECOUNT;
   state->redo_char_point = STB_TEXTEDIT_UNDOCHARCOUNT;
}

// create a new undo point
static void stb_textedit_create_undo_record(StbUndoState *state, int numchars)
{
   stb_textedit_flush_redo(state);

   // if we're going to overflow the undo buffer, we have to slide everything down
   if (state->undo_point == STB_TEXTEDIT_UNDOSTATECOUNT) {
      // if we're just going to discard the oldest one, this is easy
      if (state->undo_rec[0].char_storage + numchars <= STB_TEXTEDIT_UNDOCHARCOUNT) {
         int i;
         // slide all the records down
         for (i=0; i < STB_TEXTEDIT_UNDOSTATECOUNT-1; ++i)
            state->undo_rec[i] = state->undo_rec[i+1];
         // now slide the character buffer down
         STB_TEXTEDIT_memmove(state->undo_char, state->undo_char + state->undo_rec[0].char_storage, (STB_TEXTEDIT_UNDOCHARCOUNT - state->undo_rec[0].char_storage) * sizeof(STB_TEXTEDIT_CHARTYPE));
         state->undo_char_point -= state->undo_rec[0].char_storage;
         state->undo_point--;
      } else {
         // otherwise, we have to be more clever
         stb_textedit_flush_redo(state); // basically, we have to flush everything
         state->undo_point = 0;
         state->undo_char_point = 0;
      }
   }

   // if there's no room, crush the oldest one
   if (numchars > STB_TEXTEDIT_UNDOCHARCOUNT) {
      state->undo_point = 0;
      state->undo_char_point = 0;
      numchars = STB_TEXTEDIT_UNDOCHARCOUNT;
   }

   // if the buffer is too full, crush the oldest one
   while (state->undo_char_point + numchars > STB_TEXTEDIT_UNDOCHARCOUNT) {
      int i;
      // slide all the records down
      for (i=0; i < state->undo_point-1; ++i)
         state->undo_rec[i] = state->undo_rec[i+1];
      // now slide the character buffer down
      STB_TEXTEDIT_memmove(state->undo_char, state->undo_char + state->undo_rec[0].char_storage, (STB_TEXTEDIT_UNDOCHARCOUNT - state->undo_rec[0].char_storage) * sizeof(STB_TEXTEDIT_CHARTYPE));
      state->undo_char_point -= state->undo_rec[0].char_storage;
      state->undo_point--;
   }
}

static StbUndoRecord *stb_text_create_undo(StbUndoState *state, int where, int insert_length, int delete_length)
{
   stb_textedit_create_undo_record(state, delete_length);

   state->undo_rec[state->undo_point].where = where;
   state->undo_rec[state->undo_point].insert_length = insert_length;
   state->undo_rec[state->undo_point].delete_length = delete_length;
   state->undo_rec[state->undo_point].char_storage = state->undo_char_point;
   state->undo_point++;
   return &state->undo_rec[state->undo_point-1];
}

static void stb_text_undo(STB_TEXTEDIT_STRING *str, STB_TexteditState *state)
{
   StbUndoState *s = &state->undostate;
   StbUndoRecord *u;
   if (s->undo_point > 0) {
      // fetch up the undo record
      u = &s->undo_rec[--s->undo_point];

      // if the redo buffer is full, crush the oldest
      if (s->redo_point == STB_TEXTEDIT_UNDOSTATECOUNT) {
         int i;
         // slide all the records down
         for (i=0; i < STB_TEXTEDIT_UNDOSTATECOUNT-1; ++i)
            s->undo_rec[i] = s->undo_rec[i+1];
         // now slide the character buffer down
         STB_TEXTEDIT_memmove(s->undo_char, s->undo_char + s->undo_rec[0].char_storage, (STB_TEXTEDIT_UNDOCHARCOUNT - s->undo_rec[0].char_storage) * sizeof(STB_TEXTEDIT_CHARTYPE));
         s->redo_char_point -= s->undo_rec[0].char_storage;
         s->redo_point--;
      }
      // if the buffer is too full, crush the oldest one
      while (s->redo_char_point + u->insert_length > STB_TEXTEDIT_UNDOCHARCOUNT) {
         int i;
         // slide all the records down
         for (i=0; i < s->redo_point-1; ++i)
            s->undo_rec[i] = s->undo_rec[i+1];
         // now slide the character buffer down
         STB_TEXTEDIT_memmove(s->undo_char, s->undo_char + s->undo_rec[0].char_storage, (STB_TEXTEDIT_UNDOCHARCOUNT - s->undo_rec[0].char_storage) * sizeof(STB_TEXTEDIT_CHARTYPE));
         s->redo_char_point -= s->undo_rec[0].char_storage;
         s->redo_point--;
      }

      // now create a redo record
      s->undo_rec[s->redo_point] = *u;
      s->undo_rec[s->redo_point].char_storage = s->redo_char_point;
      s->redo_point++;

      // now do the actual undo
      if (u->delete_length)
         STB_TEXTEDIT_INSERTCHARS(str, u->where, &s->undo_char[u->char_storage], u->delete_length);
      if (u->insert_length)
         STB_TEXTEDIT_DELETECHARS(str, u->where, u->insert_length);
      state->cursor = u->where + u->delete_length;
   }
}

static void stb_text_redo(STB_TEXTEDIT_STRING *str, STB_TexteditState *state)
{
   StbUndoState *s = &state->undostate;
   StbUndoRecord *u;
   if (s->redo_point < STB_TEXTEDIT_UNDOSTATECOUNT) {
      // fetch up the redo record
      u = &s->undo_rec[s->redo_point++];

      // if the undo buffer is full, crush the oldest
      if (s->undo_point == STB_TEXTEDIT_UNDOSTATECOUNT) {
         int i;
         // slide all the records down
         for (i=0; i < STB_TEXTEDIT_UNDOSTATECOUNT-1; ++i)
            s->undo_rec[i] = s->undo_rec[i+1];
         // now slide the character buffer down
         STB_TEXTEDIT_memmove(s->undo_char, s->undo_char + s->undo_rec[0].char_storage, (STB_TEXTEDIT_UNDOCHARCOUNT - s->undo_rec[0].char_storage) * sizeof(STB_TEXTEDIT_CHARTYPE));
         s->undo_char_point -= s->undo_rec[0].char_storage;
         s->undo_point--;
      }
      // if the buffer is too full, crush the oldest one
      while (s->undo_char_point + u->delete_length > STB_TEXTEDIT_UNDOCHARCOUNT) {
         int i;
         // slide all the records down
         for (i=0; i < s->undo_point-1; ++i)
            s->undo_rec[i] = s->undo_rec[i+1];
         // now slide the character buffer down
         STB_TEXTEDIT_memmove(s->undo_char, s->undo_char + s->undo_rec[0].char_storage, (STB_TEXTEDIT_UNDOCHARCOUNT - s->undo_rec[0].char_storage) * sizeof(STB_TEXTEDIT_CHARTYPE));
         s->undo_char_point -= s->undo_rec[0].char_storage;
         s->undo_point--;
      }

      // now create an undo record
      s->undo_rec[s->undo_point] = *u;
      s->undo_rec[s->undo_point].char_storage = s->undo_char_point;
      s->undo_point++;

      // now do the actual redo
      if (u->insert_length)
         STB_TEXTEDIT_INSERTCHARS(str, u->where, &s->undo_char[u->char_storage], u->insert_length);
      if (u->delete_length)
         STB_TEXTEDIT_DELETECHARS(str, u->where, u->delete_length);
      state->cursor = u->where + u->insert_length;
   }
}

static void stb_text_makeundo_insert(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int length)
{
   stb_text_create_undo(&state->undostate, where, length, 0);
}

static void stb_text_makeundo_delete(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int length)
{
   int i;
   StbUndoRecord *u = stb_text_create_undo(&state->undostate, where, 0, length);
   for (i=0; i < length; ++i)
      u->undo_char[i] = STB_TEXTEDIT_GETCHAR(str, where+i);
}

static void stb_text_makeundo_replace(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int old_length, int new_length)
{
   int i;
   StbUndoRecord *u = stb_text_create_undo(&state->undostate, where, new_length, old_length);
   for (i=0; i < old_length; ++i)
      u->undo_char[i] = STB_TEXTEDIT_GETCHAR(str, where+i);
}

//
//
/////////////////////////////////////////////////////////////////////////////
#endif // STB_TEXTEDIT_IMPLEMENTATION
