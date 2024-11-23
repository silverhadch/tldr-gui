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
#include <iostream>
#include <memory>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace std;

atomic<bool> timeoutOccurred(false);
string searchTerm;
Fl_Text_Buffer *outputBuffer;
pid_t pid = getpid(); // Get the current process ID

// Forward declarations for functions
void showPopupError();
void hidePopup(Fl_Window *popup);
void checkForSearchTerm(void *);

// Helper function to execute commands in the background and capture output
void executeCommandInBackground(const string &command) {
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
}

// Function to check if the output contains the search term
void checkForSearchTerm(void *) {
  if (timeoutOccurred)
    return; // Exit if popup already triggered

  // Check if the search term exists in the output buffer
  string outputText = outputBuffer->text();
  if (outputText.find(searchTerm) == string::npos) {
    timeoutOccurred = true; // Mark timeout occurred
    Fl::lock();
    showPopupError();
    Fl::unlock();
  }
}

// Function to show a custom error popup
void showPopupError() {
  Fl_Window *popup = new Fl_Window(300, 100, "Error");
  popup->begin();
  Fl_Box *box =
      new Fl_Box(20, 30, 260, 40, "Page not found!"); // Correct use of Fl_Box
  popup->end();
  popup->show();
}

// Function to kill existing tldr processes (except the current one)
void killTldrProcesses() {
  // Use the old termination logic to kill processes
  string killCommand =
      "ps aux | grep '[t]ldr' | awk '{if ($2 != " + to_string(pid) +
      ") system(\"kill -9 \" $2)}'";
  system(killCommand.c_str());
}

// Search button callback
void onSearch(Fl_Widget *widget, void *data) {
  auto *searchField = static_cast<Fl_Input *>(static_cast<void **>(data)[0]);
  outputBuffer = static_cast<Fl_Text_Buffer *>(static_cast<void **>(data)[1]);

  // Get the search term from the input field
  searchTerm = searchField->value();

  // Kill any existing "tldr" processes (except the GUI process)
  killTldrProcesses();

  // Run the TLDR command with the input text in the background
  string command = "tldr " + string(searchField->value()) + " &";
  thread commandThread(executeCommandInBackground, command);
  commandThread.detach(); // Detach the thread to run asynchronously

  // Reset timeout flag
  timeoutOccurred = false;

  // Add a timeout to check after 7 seconds using a lambda
  Fl::add_timeout(1.0, [](void *) { checkForSearchTerm(nullptr); });
}

// Update button callback
void onUpdate(Fl_Widget *widget, void *data) {
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
    executeCommandInBackground(command);

    // After update finishes, close the popup
    Fl::awake((Fl_Awake_Handler)hidePopup, popup);
  });

  // Let the user close the popup manually while waiting for the update
  Fl::run();

  // Clean up after the update
  updateThread.join();
  delete popup;
}

// Function to hide the popup window
void hidePopup(Fl_Window *popup) { popup->hide(); }

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
