#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Window.H>
#include <array>
#include <cstdio>
#include <string>

using namespace std;

// Function to execute a shell command and capture the output
string executeCommand(const string &command) {
  array<char, 128> buffer{};
  string result;

  FILE *pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return "Error: Unable to execute command.";
  }

  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    result += buffer.data();
  }

  pclose(pipe);
  return result;
}

// Callback for the "Search" button
void onSearch(Fl_Widget *widget, void *data) {
  auto widgets = static_cast<pair<Fl_Input *, Fl_Text_Buffer *> *>(data);
  Fl_Input *searchField = widgets->first;
  Fl_Text_Buffer *textBuffer = widgets->second;

  string searchTerm = searchField->value();
  if (searchTerm.empty()) {
    textBuffer->text("Please enter a search term.");
    return;
  }

  textBuffer->text("Searching...");
  string command = "tldr " + searchTerm;
  string output = executeCommand(command);
  textBuffer->text(output.c_str());
}

// Callback for the "Update" button
void onUpdate(Fl_Widget *widget, void *data) {
  Fl_Text_Buffer *textBuffer = static_cast<Fl_Text_Buffer *>(data);

  textBuffer->text(
      "Updating, please have patience...\nThis might take some minutes.");
  string output = executeCommand("tldr -u");
  textBuffer->text(output.c_str());
}

int main() {
  Fl_Window *window = new Fl_Window(600, 400, "TLDR GUI");

  // Search field
  Fl_Input *searchField = new Fl_Input(50, 50, 300, 30, "Search:");

  // Text display for output
  Fl_Text_Buffer *textBuffer = new Fl_Text_Buffer();
  Fl_Text_Display *textDisplay = new Fl_Text_Display(50, 100, 500, 250);
  textDisplay->buffer(textBuffer);

  // Search button
  Fl_Button *searchButton = new Fl_Button(370, 50, 80, 30, "Search");
  auto searchWidgets = make_pair(searchField, textBuffer);
  searchButton->callback(onSearch, &searchWidgets);

  // Update button
  Fl_Button *updateButton = new Fl_Button(460, 50, 80, 30, "Update");
  updateButton->callback(onUpdate, textBuffer);

  // Show window
  window->end();
  window->show();
  return Fl::run();
}

