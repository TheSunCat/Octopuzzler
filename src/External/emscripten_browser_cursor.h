#ifndef EMSCRIPTEN_BROWSER_CURSOR_H_INCLUDED
#define EMSCRIPTEN_BROWSER_CURSOR_H_INCLUDED

#include <emscripten.h>

namespace emscripten_browser_cursor {

////////////////////////////////// Interface ///////////////////////////////////

enum class cursor {
  // General
  cursor_auto,                                                                  // The UA will determine the cursor to display based on the current context. E.g., equivalent to text when hovering text.
  cursor_default,                                                               // The platform-dependent default cursor. Typically an arrow.
  none,                                                                         // No cursor is rendered.

  // Links & status
  context_menu,                                                                 // cursor slightly obscuring a menu icon - A context menu is available.
  help,                                                                         // cursor next to a question mark - Help information is available.
  pointer,                                                                      // right hand with an index finger pointing up - The cursor is a pointer that indicates a link. Typically an image of a pointing hand.
  progress,                                                                     // cursor and hour glass - The program is busy in the background, but the user can still interact with the interface (in contrast to wait).
  wait,                                                                         // hour glass - The program is busy, and the user can't interact with the interface (in contrast to progress). Sometimes an image of an hourglass or a watch.

  // Selection
  cell,                                                                         // plus symbol - The table cell or set of cells can be selected.
  crosshair,                                                                    // crosshair  - Cross cursor, often used to indicate selection in a bitmap.
  text,                                                                         // vertical i-beam - The text can be selected. Typically the shape of an I-beam.
  vertical_text,                                                                // horizontal i-beam - The vertical text can be selected. Typically the shape of a sideways I-beam.

  // Drag & drop
  alias,                                                                        // cursor next to a folder icon with a curved arrow pointing up and to the right - An alias or shortcut is to be created.
  copy,                                                                         // cursor next to a smaller folder icon with a plus sign - Something is to be copied.
  move,                                                                         // plus sign made of two thin lines, with small arrows facing out - Something is to be moved.
  no_drop,                                                                      // cursor next to circle with a line through it - An item may not be dropped at the current location.
  not_allowed,                                                                  // Circle with a line through it - The requested action will not be carried out.
  grab,                                                                         // fully opened hand - Something can be grabbed (dragged to be moved).
  grabbing,                                                                     // closed hand - Something is being grabbed (dragged to be moved).

  // Resizing & scrolling
  all_scroll,                                                                   // dot with four triangles around it - Something can be scrolled in any direction (panned).
  col_resize,                                                                   // The item/column can be resized horizontally. Often rendered as arrows pointing left and right with a vertical bar separating them.
  row_resize,                                                                   // The item/row can be resized vertically. Often rendered as arrows pointing up and down with a horizontal bar separating them.
  n_resize,                                                                     // arrow pointing up - Some edge is to be moved. For example, the se-resize cursor is used when the movement starts from the south-east corner of the box.
  e_resize,                                                                     // arrow pointing right
  s_resize,                                                                     // arrow pointing down
  w_resize,                                                                     // arrow pointing left
  ne_resize,                                                                    // arrow pointing top-right
  nw_resize,                                                                    // arrow pointing top-left
  se_resize,                                                                    // arrow pointing bottom-right
  sw_resize,                                                                    // arrow pointing bottom-left
  ew_resize,                                                                    // arrow pointing left and right - Bidirectional resize cursor.
  ns_resize,                                                                    // arrow pointing up and down
  nesw_resize,                                                                  // arrow pointing both to the top-right and bottom-left
  nwse_resize,                                                                  // arrow pointing both to the top-left and bottom-right

  // Zooming
  zoom_in,                                                                      // magnifying glass with a plus sign - Something can be zoomed (magnified) in or out.
  zoom_out
};

bool is_set();                                                                  // returns whether the cursor is currently set

std::optional<cursor> get();                                                    // return the currently set cursor as a cursor enum, or std::nullopt if no cursor is set, or if an unknown cursor is set
std::string get_string();                                                       // return the currently set cursor as a string value

void set(cursor new_cursor);                                                    // set a new cursor from a cursor enum
void set(std::optional<cursor> new_cursor);                                     // set a new cursor from an optional - passing std::nullopt unsets the cursor
void set(std::string const &new_cursor);                                        // set a new cursor from an arbitrary string value

void unset();                                                                   // clear the current cursor setting

//////////////////////////////// Implementation ////////////////////////////////

bool is_set() {
  /// Returns whether the cursor is currently set
  return EM_ASM_INT(
    return !(!document.body.style.cursor || document.body.style.cursor.length === 0 );
  );
}

std::string get_string() {
  /// Return the current cursor setting as a string
  auto cursor_str_ptr{reinterpret_cast<char*>(EM_ASM_PTR(
    cursor_str_ptr = allocateUTF8(document.body.style.cursor);
    return cursor_str_ptr;
  ))};
  std::string const cursor_str{cursor_str_ptr};
  free(cursor_str_ptr);
  return cursor_str;
}

void set(std::optional<cursor> new_cursor) {
  /// Set a new cursor from an optional - passing std::nullopt unsets the cursor
  if(new_cursor) {
    set(new_cursor.value());
  } else {
    unset();
  }
}

void set(std::string const &new_cursor) {
  /// Set the cursor from an arbitrary string
  EM_ASM({
    document.body.style.cursor = UTF8ToString($0);
  }, new_cursor.c_str());
}

void unset() {
  /// Remove the cursor setting
  EM_ASM(
    document.body.style.cursor = "";
  );
}

std::optional<cursor> get() {
  /// Return the current cursor setting as an optional enum - returns nullopt for unknown types
  auto cursor_str{get_string()};
  if(cursor_str.empty()) {
    return std::nullopt;                                                        // no cursor set
  } else {
    if(cursor_str == "auto") {
      return cursor::cursor_auto;
    } else if(cursor_str == "default") {
      return cursor::cursor_default;
    } else if(cursor_str == "none") {
      return cursor::none;
    } else if(cursor_str == "context-menu") {
      return cursor::context_menu;
    } else if(cursor_str == "help") {
      return cursor::help;
    } else if(cursor_str == "pointer") {
      return cursor::pointer;
    } else if(cursor_str == "progress") {
      return cursor::progress;
    } else if(cursor_str == "wait") {
      return cursor::wait;
    } else if(cursor_str == "cell") {
      return cursor::cell;
    } else if(cursor_str == "crosshair") {
      return cursor::crosshair;
    } else if(cursor_str == "text") {
      return cursor::text;
    } else if(cursor_str == "vertical-text") {
      return cursor::vertical_text;
    } else if(cursor_str == "alias") {
      return cursor::alias;
    } else if(cursor_str == "copy") {
      return cursor::copy;
    } else if(cursor_str == "move") {
      return cursor::move;
    } else if(cursor_str == "no-drop") {
      return cursor::no_drop;
    } else if(cursor_str == "not-allowed") {
      return cursor::not_allowed;
    } else if(cursor_str == "grab") {
      return cursor::grab;
    } else if(cursor_str == "grabbing") {
      return cursor::grabbing;
    } else if(cursor_str == "all-scroll") {
      return cursor::all_scroll;
    } else if(cursor_str == "col-resize") {
      return cursor::col_resize;
    } else if(cursor_str == "row-resize") {
      return cursor::row_resize;
    } else if(cursor_str == "n-resize") {
      return cursor::n_resize;
    } else if(cursor_str == "e-resize") {
      return cursor::e_resize;
    } else if(cursor_str == "s-resize") {
      return cursor::s_resize;
    } else if(cursor_str == "w-resize") {
      return cursor::w_resize;
    } else if(cursor_str == "ne-resize") {
      return cursor::ne_resize;
    } else if(cursor_str == "nw-resize") {
      return cursor::nw_resize;
    } else if(cursor_str == "se-resize") {
      return cursor::se_resize;
    } else if(cursor_str == "sw-resize") {
      return cursor::sw_resize;
    } else if(cursor_str == "ew-resize") {
      return cursor::ew_resize;
    } else if(cursor_str == "ns-resize") {
      return cursor::ns_resize;
    } else if(cursor_str == "nesw-resize") {
      return cursor::nesw_resize;
    } else if(cursor_str == "nwse-resize") {
      return cursor::nwse_resize;
    } else if(cursor_str == "zoom-in") {
      return cursor::zoom_in;
    } else if(cursor_str == "zoom-out") {
      return cursor::zoom_out;
    } else {
      return std::nullopt;                                                      // unknown cursor type
    }
  }
}

void set(cursor new_cursor) {
  /// Set the cursor according to the given enum
  switch(new_cursor) {
  case cursor::cursor_auto:
    EM_ASM(document.body.style.cursor = 'auto';);
    break;
  case cursor::cursor_default:
    EM_ASM(document.body.style.cursor = 'default';);
    break;
  case cursor::none:
    EM_ASM(document.body.style.cursor = 'none';);
    break;
  case cursor::context_menu:
    EM_ASM(document.body.style.cursor = 'context-menu';);
    break;
  case cursor::help:
    EM_ASM(document.body.style.cursor = 'help';);
    break;
  case cursor::pointer:
    EM_ASM(document.body.style.cursor = 'pointer';);
    break;
  case cursor::progress:
    EM_ASM(document.body.style.cursor = 'progress';);
    break;
  case cursor::wait:
    EM_ASM(document.body.style.cursor = 'wait';);
    break;
  case cursor::cell:
    EM_ASM(document.body.style.cursor = 'cell';);
    break;
  case cursor::crosshair:
    EM_ASM(document.body.style.cursor = 'crosshair';);
    break;
  case cursor::text:
    EM_ASM(document.body.style.cursor = 'text';);
    break;
  case cursor::vertical_text:
    EM_ASM(document.body.style.cursor = 'vertical-text';);
    break;
  case cursor::alias:
    EM_ASM(document.body.style.cursor = 'alias';);
    break;
  case cursor::copy:
    EM_ASM(document.body.style.cursor = 'copy';);
    break;
  case cursor::move:
    EM_ASM(document.body.style.cursor = 'move';);
    break;
  case cursor::no_drop:
    EM_ASM(document.body.style.cursor = 'no-drop';);
    break;
  case cursor::not_allowed:
    EM_ASM(document.body.style.cursor = 'not-allowed';);
    break;
  case cursor::grab:
    EM_ASM(document.body.style.cursor = 'grab';);
    break;
  case cursor::grabbing:
    EM_ASM(document.body.style.cursor = 'grabbing';);
    break;
  case cursor::all_scroll:
    EM_ASM(document.body.style.cursor = 'all-scroll';);
    break;
  case cursor::col_resize:
    EM_ASM(document.body.style.cursor = 'col-resize';);
    break;
  case cursor::row_resize:
    EM_ASM(document.body.style.cursor = 'row-resize';);
    break;
  case cursor::n_resize:
    EM_ASM(document.body.style.cursor = 'n-resize';);
    break;
  case cursor::e_resize:
    EM_ASM(document.body.style.cursor = 'e-resize';);
    break;
  case cursor::s_resize:
    EM_ASM(document.body.style.cursor = 's-resize';);
    break;
  case cursor::w_resize:
    EM_ASM(document.body.style.cursor = 'w-resize';);
    break;
  case cursor::ne_resize:
    EM_ASM(document.body.style.cursor = 'ne-resize';);
    break;
  case cursor::nw_resize:
    EM_ASM(document.body.style.cursor = 'nw-resize';);
    break;
  case cursor::se_resize:
    EM_ASM(document.body.style.cursor = 'se-resize';);
    break;
  case cursor::sw_resize:
    EM_ASM(document.body.style.cursor = 'sw-resize';);
    break;
  case cursor::ew_resize:
    EM_ASM(document.body.style.cursor = 'ew-resize';);
    break;
  case cursor::ns_resize:
    EM_ASM(document.body.style.cursor = 'ns-resize';);
    break;
  case cursor::nesw_resize:
    EM_ASM(document.body.style.cursor = 'nesw-resize';);
    break;
  case cursor::nwse_resize:
    EM_ASM(document.body.style.cursor = 'nwse-resize';);
    break;
  case cursor::zoom_in:
    EM_ASM(document.body.style.cursor = 'zoom-in';);
    break;
  case cursor::zoom_out:
    EM_ASM(document.body.style.cursor = 'zoom-out';);
    break;
  }
}


}

#endif // EMSCRIPTEN_BROWSER_CURSOR_H_INCLUDED
