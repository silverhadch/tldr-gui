# TLDR GUI  

A simple graphical interface for the TLDR command-line tool, built using FLTK.  
This project is a work in progress, and contributions are welcome!  

## Features  
- **Search TLDR pages:** Enter a command to search and display its TLDR page.  
- **Update TLDR cache:** Update the local TLDR cache with a single click.  
- **Non-blocking UI:** Both search and update operations run in the background to keep the interface responsive.  

## Dependencies  
Make sure the following dependencies are installed on your system:  
1. **FLTK (Fast Light Toolkit):** For building the GUI.  
   - On Debian-based systems:  
     ```bash  
     sudo apt-get install libfltk1.3-dev  
     ```  
   - On Arch-based systems:  
     ```bash  
     sudo pacman -S fltk  
     ```  
2. **TLDR CLI Tool:**  
   - Install via `npm` (Node.js is required):  
     ```bash  
     npm install -g tldr  
     ```  

## How to Compile  
1. Clone the repository:  
   ```bash  
   git clone <repository_url>  
   cd <repository_name>

2. Compile the source code:

g++ -o tldr_gui src/main.cpp -lfltk


3. Run the program:

./tldr_gui



License

This project is licensed under the GNU General Public License v3.0. See the LICENSE file for more details.

Notes

This project is work in progress; features and improvements are ongoing.

If you encounter any issues or have suggestions, feel free to open an issue or submit a pull request!


To-Do

Improve error handling for command execution.

Add a settings menu for user preferences.

Support additional TLDR languages (e.g., localized pages).

Add unit tests for core functionalities.




