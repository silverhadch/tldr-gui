#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>

int main() {
  Fl_Window *window = new Fl_Window(400, 300, "FLTK Hello World");

  // Title at the top with large font size
  Fl_Box *title_box = new Fl_Box(50, 10, 300, 40, "FLTK Text Formatting");
  title_box->labelsize(32);      // Big title font size
  title_box->labelfont(FL_BOLD); // Bold font for the title

  // Regular text
  Fl_Box *regular_box = new Fl_Box(50, 70, 200, 30, "Hello, World!");
  regular_box->labelsize(24);           // Set text size
  regular_box->labelfont(FL_HELVETICA); // Regular font (default)

  // Cursive-like text (simulated using italic font)
  Fl_Box *cursive_box = new Fl_Box(50, 110, 200, 30, "This is Cursive-like!");
  cursive_box->labelsize(24);        // Set text size
  cursive_box->labelfont(FL_ITALIC); // Italic for cursive-like effect

  // Bold text
  Fl_Box *bold_box = new Fl_Box(50, 150, 200, 30, "This is Bold text!");
  bold_box->labelsize(24);      // Set text size
  bold_box->labelfont(FL_BOLD); // Bold font

  window->end();
  window->show();

  return Fl::run();
}

