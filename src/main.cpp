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
// Helper function to execute commands in the background and capture output
void executeCommandInBackground(const string &command, Fl_Text_Buffer *outputBuffer) {
  array<char, 128> buffer;
  string result;

  shared_ptr<FILE> pipe(popen((command + " 2>&1").c_str(), "r"), pclose);
  if (!pipe) {
    outputBuffer->text("Error: Failed to start the command!");
    return;
  }

  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }

  // Replace commas with newlines to separate the commands
  size_t pos = 0;
  while ((pos = result.find(',')) != string::npos) {
    result.replace(pos, 1, "\n");  // Replace comma with newline
  }

  Fl::lock();
  outputBuffer->text(result.c_str());
  Fl::unlock();
  Fl::flush();
}


// Function to check if the output contains the search term
void checkForSearchTerm(void *) {
  if (timeoutOccurred)
    return;

  string outputText = outputBuffer->text();
  if (outputText.find(searchTerm) == string::npos) {
    timeoutOccurred = true;
    Fl::lock();
    showPopupError();
    Fl::unlock();
  }
}

// Function to show a custom error popup
void showPopupError() {
  Fl_Window *popup = new Fl_Window(300, 100, "Error");
  popup->begin();
  Fl_Box *box = new Fl_Box(20, 30, 260, 40, "Page not found!");
  popup->end();
  popup->show();
}

// Function to kill existing tldr processes (except the current one)
void killTldrProcesses() {
  string killCommand =
      "ps aux | grep '[t]ldr' | awk '{if ($2 != " + to_string(pid) +
      ") system(\"kill -9 \" $2)}'";
  system(killCommand.c_str());
}

// Search button callback
void onSearch(Fl_Widget *widget, void *data) {
  auto *searchField = static_cast<Fl_Input *>(static_cast<void **>(data)[0]);
  outputBuffer = static_cast<Fl_Text_Buffer *>(static_cast<void **>(data)[1]);

  searchTerm = searchField->value();
  killTldrProcesses();

  string command = "tldr " + string(searchField->value()) + " &";
  thread commandThread(executeCommandInBackground, command, outputBuffer);
  commandThread.detach();

  timeoutOccurred = false;
  Fl::add_timeout(1.0, [](void *) { checkForSearchTerm(nullptr); });
}

// Static function to be used as a callback for Fl::awake
static void randomCommandWrapper(void *data) {
  string command = "tldr --random &";
  Fl_Text_Buffer *outputBuffer = static_cast<Fl_Text_Buffer *>(data);
  executeCommandInBackground(command, outputBuffer);
}

// Random button callback
void onRandom(Fl_Widget *widget, void *data) {
  Fl::awake(randomCommandWrapper, data);
}

// Update Database button callback
void onUpdate(Fl_Widget *widget, void *data) {
  Fl_Window *popup = new Fl_Window(300, 100, "Updating");
  popup->begin();
  Fl_Text_Display *popupMessage = new Fl_Text_Display(20, 30, 260, 40);
  Fl_Text_Buffer *buffer = new Fl_Text_Buffer();
  popupMessage->buffer(buffer);
  buffer->text("Updating, please wait...");
  popup->end();
  popup->show();

  atomic<bool> cancelFlag(false);

  thread updateThread([&]() {
    string command = "tldr -u &";
    executeCommandInBackground(command, buffer);

    Fl::awake((Fl_Awake_Handler)hidePopup, popup);
  });

  Fl::run();

  updateThread.join();
  delete popup;
}

// Function to hide the popup window
void hidePopup(Fl_Window *popup) { popup->hide(); }

// List All Commands button callback
void onListCommands(Fl_Widget *widget, void *data) {
  string command = "tldr -l &";
  Fl_Text_Buffer *outputBuffer = static_cast<Fl_Text_Buffer *>(data);
  executeCommandInBackground(command, outputBuffer);
}

int main() {
  // Calculate a nearly maximized window size
  int windowWidth = Fl::w() * 0.9;  // 90% of screen width
  int windowHeight = Fl::h() * 0.9; // 90% of screen height

  // Center the window on the screen
  int windowX = (Fl::w() - windowWidth) / 2;
  int windowY = (Fl::h() - windowHeight) / 2;

  // Create the window with calculated size and position
  Fl_Window *window =
      new Fl_Window(windowX, windowY, windowWidth, windowHeight, "TLDR GUI");

  // Add title at the top
  Fl_Box *title = new Fl_Box(0, 10, windowWidth, 50, "TLDR Command GUI");
  title->labelfont(FL_BOLD);
  title->labelsize(24);
  title->align(FL_ALIGN_CENTER);

  // Label for the search field
  Fl_Box *searchLabel = new Fl_Box(50, 70, 300, 30, "Search:");
  searchLabel->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

  // Search field
  Fl_Input *searchField = new Fl_Input(50, 100, 300, 30);

  // Text display
  Fl_Text_Buffer *textBuffer = new Fl_Text_Buffer();
  Fl_Text_Display *textDisplay =
      new Fl_Text_Display(50, 150, windowWidth - 100, windowHeight - 200);
  textDisplay->buffer(textBuffer);
  textDisplay->textsize(16); // Increase text size for better readability
  window->resizable(
      textDisplay); // Ensure the text display scales with the window

  // Buttons
  Fl_Button *searchButton = new Fl_Button(370, 100, 80, 30, "Search");
  Fl_Button *randomButton = new Fl_Button(460, 100, 80, 30, "Random");
  Fl_Button *updateButton = new Fl_Button(550, 100, 120, 30, "Update Database");
  Fl_Button *listCommandsButton = new Fl_Button(680, 100, 150, 30, "List All Commands");

  // Combine data for callbacks
  void *callbackData[] = {searchField, textBuffer};
  searchButton->callback(onSearch, callbackData);
  randomButton->callback(onRandom, textBuffer);
  updateButton->callback(onUpdate, textBuffer);
  listCommandsButton->callback(onListCommands, textBuffer);

  // Automatically show the "tldr tldr" page on startup
  string command = "tldr tldr &";
  Fl_Text_Buffer *outputBuffer = textBuffer;

  // Execute the command immediately upon startup in the background
  thread commandThread([command, outputBuffer]() {
    executeCommandInBackground(command, outputBuffer);
  });
  commandThread.detach();  // Detach the thread so it doesn't block further operations

  window->end();
  window->show();

  return Fl::run();
}
