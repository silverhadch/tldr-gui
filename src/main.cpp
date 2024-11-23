#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Window.H>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <thread>

using namespace std;

// Struct to hold data for the callback
struct CallbackData {
  Fl_Input *searchField;
  Fl_Text_Buffer *outputBuffer;
};

// Helper function to execute commands and capture output
string executeCommand(const string &command) {
  array<char, 128> buffer;
  string result;
  shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
  if (!pipe) {
    throw runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

// Function to show the "Updating, please wait..." message
void showUpdatingPopup() {
  // Create a popup window for showing the "please wait" message
  Fl_Window *popup = new Fl_Window(300, 100, "Updating");
  popup->begin();

  Fl_Text_Display *popupMessage = new Fl_Text_Display(20, 30, 260, 40);
  Fl_Text_Buffer *buffer = new Fl_Text_Buffer();
  popupMessage->buffer(buffer);
  buffer->text("Updating, please wait...");

  popup->end();
  popup->set_modal();
  popup->show();

  // Process events to make the popup visible
  Fl::check();

  // Wait for the popup to be closed (it will be closed after the update
  // finishes)
  while (popup->shown()) {
    Fl::wait();
  }

  // Clean up
  delete popup;
}

// Update button callback
void onUpdate(Fl_Widget *widget, void *data) {
  Fl_Text_Buffer *outputBuffer = static_cast<Fl_Text_Buffer *>(data);

  // Show the "please wait" message
  thread popupThread(showUpdatingPopup);

  // Run the TLDR update command
  string command = "tldr -u";
  try {
    string output = executeCommand(command);
    outputBuffer->text(output.c_str()); // Display output in the text display
  } catch (const exception &e) {
    outputBuffer->text(("Error: " + string(e.what())).c_str());
  }

  // Close the popup once the update is done
  popupThread.join();
}

// Search button callback
void onSearch(Fl_Widget *widget, void *data) {
  // Unpack the input and output buffer from CallbackData struct
  CallbackData *callbackData = static_cast<CallbackData *>(data);
  Fl_Input *searchField = callbackData->searchField;
  Fl_Text_Buffer *outputBuffer = callbackData->outputBuffer;

  // Run the TLDR command with the input text
  string command = "tldr " + string(searchField->value());
  try {
    string output = executeCommand(command);
    outputBuffer->text(output.c_str()); // Display output in the text display
  } catch (const exception &e) {
    outputBuffer->text(("Error: " + string(e.what())).c_str());
  }
}

int main() {
  // Create the main window
  Fl_Window *window = new Fl_Window(600, 400, "TLDR GUI");

  // Create input field for search
  Fl_Input *searchField = new Fl_Input(50, 50, 300, 30, "Search:");

  // Create text display for output
  Fl_Text_Buffer *textBuffer = new Fl_Text_Buffer();
  Fl_Text_Display *textDisplay = new Fl_Text_Display(50, 100, 500, 250);
  textDisplay->buffer(textBuffer);

  // Create search button
  Fl_Button *searchButton = new Fl_Button(370, 50, 80, 30, "Search");

  // Create a struct to pass input and output buffer to the callback
  CallbackData *callbackData = new CallbackData{searchField, textBuffer};
  searchButton->callback(onSearch, callbackData);

  // Create update button
  Fl_Button *updateButton = new Fl_Button(460, 50, 80, 30, "Update");
  updateButton->callback(onUpdate, textBuffer);

  // Finalize and show the window
  window->end();
  window->show();
  return Fl::run();
}

