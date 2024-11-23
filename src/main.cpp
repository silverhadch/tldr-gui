#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Window.H>
#include <array>
#include <atomic>
#include <cstdlib>
#include <cstring> // for strcmp
#include <iostream>
#include <memory>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace std;

// Function to kill processes with "tldr" in the name, excluding the GUI
void killTldrProcesses() {
  // Get the current process ID
  pid_t pid = getpid();

  // Run the pkill command to kill processes with "tldr" in the name, but
  // exclude the current process
  string command =
      "ps aux | grep '[t]ldr' | awk '{if ($2 != " + to_string(pid) +
      ") system(\"kill -9 \" $2)}'";

  system(command.c_str());
}

// Function to hide the popup window
void hidePopup(Fl_Window *popup) {
  Fl::lock();
  popup->hide();
  Fl::unlock();
}

// Helper function to execute commands in the background and capture output
void executeCommandInBackground(const string &command, atomic<bool> &cancelFlag,
                                Fl_Text_Buffer *outputBuffer,
                                const string &searchTerm) {
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
    // Update the output buffer with new output
    Fl::lock(); // Lock the UI thread for safe text buffer update
    outputBuffer->text(result.c_str());
    Fl::unlock(); // Unlock after updating the UI
    Fl::flush();  // Flush to update the UI immediately
  }

  // Check if the search term was found in the result
  if (result.find(searchTerm) == string::npos) {
    // If not found, show a custom popup error
    Fl::lock();
    Fl_Window *popup = new Fl_Window(300, 100, "Error");
    Fl_Box *box = new Fl_Box(20, 30, 260, 40,
                             "Page not found for the given search term.");
    popup->end();
    popup->show();
    Fl::unlock();

    // Process events to show the popup immediately
    Fl::awake((Fl_Awake_Handler)hidePopup, popup);
  }
}

// Search button callback
void onSearch(Fl_Widget *widget, void *data) {
  auto *searchField = static_cast<Fl_Input *>(static_cast<void **>(data)[0]);
  auto *outputBuffer =
      static_cast<Fl_Text_Buffer *>(static_cast<void **>(data)[1]);

  // Kill any existing "tldr" processes (except the GUI process)
  killTldrProcesses();

  // Run the TLDR command with the input text in the background
  string command = "tldr " + string(searchField->value()) + " &";

  // Atomic flag to control process cancellation
  atomic<bool> cancelFlag(false);

  // Get the search term from the input field
  string searchTerm = searchField->value();

  // Run the command in a separate thread
  thread commandThread(executeCommandInBackground, command, ref(cancelFlag),
                       outputBuffer, searchTerm);
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
  popup->show(); // Show the popup window

  // Atomic flag to control process cancellation
  atomic<bool> cancelFlag(false);

  // Run the TLDR update command in the background
  thread updateThread([&]() {
    string command = "tldr -u &";
    executeCommandInBackground(command, cancelFlag, buffer, "Updating");

    // After update finishes, close the popup
    Fl::awake((Fl_Awake_Handler)hidePopup, popup);
  });

  // Let the user close the popup manually while waiting for the update
  Fl::run();

  // Clean up after the update
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

