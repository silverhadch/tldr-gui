#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>
#include <iostream>

void onSearch(Fl_Widget *widget, void *inputField) {
  Fl_Input *input = (Fl_Input *)inputField;
  std::cout << "Search: " << input->value() << std::endl;
}

int main() {
  Fl_Window *window = new Fl_Window(400, 200, "Simple GUI");

  Fl_Input *searchField = new Fl_Input(50, 50, 200, 30, "Search:");
  Fl_Button *searchButton = new Fl_Button(260, 50, 80, 30, "Search");

  searchButton->callback(onSearch, searchField);

  window->end();
  window->show();
  return Fl::run();
}
