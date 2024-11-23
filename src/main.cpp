#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>
#include <cstdlib>
#include <iostream>
#include <string>

void onSearch(Fl_Widget *widget, void *inputField) {
  Fl_Input *input = (Fl_Input *)inputField;
  std::string command = "tldr " + std::string(input->value());

  // Print the command for debug purposes
  std::cout << "Executing: " << command << std::endl;

  // Execute the tldr command
  int result = std::system(command.c_str());
  if (result != 0) {
    std::cerr
        << "Error: 'tldr' command failed. Ensure it is installed and in PATH."
        << std::endl;
  }
}

int main() {
  // Create the main window
  Fl_Window *window = new Fl_Window(400, 200, "TLDR Search");

  // Create the input field for search
  Fl_Input *searchField = new Fl_Input(50, 50, 200, 30, "Search:");

  // Create the search button
  Fl_Button *searchButton = new Fl_Button(260, 50, 80, 30, "Search");
  searchButton->callback(onSearch, searchField);

  // Finalize and show the window
  window->end();
  window->show();
  return Fl::run();
}

