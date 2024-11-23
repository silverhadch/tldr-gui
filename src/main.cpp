#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Window.H>
#include <array>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace std;

// Helper function to execute commands in the background and capture output
void executeCommandInBackground(const string &command, atomic<bool> &cancelFlag,
                                Fl_Text_Buffer *outputBuffer) {
  array<char, 128> buffer;
  string result;

  // Open a pipe to run the command in the background
  shared_ptr<FILE> pipe(popen((command + " 2>&1").c_str(), "r"), pclose);
  if (!pipe) {
    outputBuffer->text("Error: Failed to start the command!");
    return;
  }

  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
    // Check for the "✔ Page not found" message
    if (result.find("✔ Page not found") != string::npos) {
      cancelFlag.store(true); // Flag to cancel the command
      break;
    }

    // Update the output buffer with new output
    Fl::lock(); // Lock the UI thread for safe text buffer update
    outputBuffer->text(result.c_str());
    Fl::unlock(); // Unlock after updating the UI
    Fl::flush();  // Flush to update the UI immediately
  }

  if (cancelFlag.load()) {
    Fl::lock();
    outputBuffer->text("Error: ✔ Page not found.");
    Fl::unlock();
  }
}

// Function to hide the popup window
void hidePopup(Fl_Window *popup) { popup->hide(); }

// Search button callback
void onSearch(Fl_Widget *widget, void *data) {
  auto *searchField = static_cast<Fl_Input *>(static_cast<void **>(data)[0]);
  auto *outputBuffer =
      static_cast<Fl_Text_Buffer *>(static_cast<void **>(data)[1]);

  // Run the TLDR command with the input text in the background
  string command = "tldr " + string(searchField->value()) + " &";

  // Atomic flag to control process cancellation
  atomic<bool> cancelFlag(false);

  // Run the command in a separate thread
  thread commandThread(executeCommandInBackground, command, ref(cancelFlag),
                       outputBuffer);
  commandThread.detach(); // Detach the thread to allow it to run asynchronously
}

// Update button callback
void onUpdate(Fl_Widget *widget, void *data) {
  auto *outputBuffer = static_cast<Fl_Text_Buffer *>(data);

  // Create a popup to show "Updating..."
  Fl_Window *popup = new Fl_Window(300, 100, "Updating");
  popup->begin();
  Fl_Text_Display *popupMessage = new Fl_Text_Display(20, 30, 260, 40);
  Fl_Text_Buffer *buffer = new Fl_Text_Buffer();
  popupMessage->buffer(buffer);
  buffer->text("Updating, please wait...");
  popup->end();
  popup->set_modal();
  popup->show();

  // Atomic flag to control process cancellation
  atomic<bool> cancelFlag(false);

  // Run the TLDR update command in the background
  thread updateThread([&]() {
    string command = "tldr -u &";
    executeCommandInBackground(command, cancelFlag, buffer);

    // Call Fl::awake with the hidePopup function to hide the popup after
    // updating
    Fl::awake((Fl_Awake_Handler)hidePopup, popup);
  });

  // Wait until the process finishes
  while (popup->shown()) {
    Fl::wait(); // Wait for events (keeps UI responsive)
  }

  // Clean up
  updateThread.join();
  delete popup;
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

  // Combine input and output buffer for callback
  void *callbackData[] = {searchField, textBuffer};
  searchButton->callback(onSearch, callbackData);

  // Create update button
  Fl_Button *updateButton = new Fl_Button(460, 50, 80, 30, "Update");
  updateButton->callback(onUpdate, textBuffer);

  // Finalize and show the window
  window->end();
  window->show();
  return Fl::run();
}

