# TLDR GUI  

A simple graphical interface for the TLDR command-line tool, built using FLTK.  
Contributions are welcome!  

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
   git clone https://github.com/silverhadch/tldr-gui 
   cd tldr-gui
   ```
2. Compile the source code:
   ```bash
   g++ -o tldr_gui src/main.cpp -lfltk
   ```

3. Run the program:
   ```bash
   ./tldr_gui
   ```


# License

This project is licensed under the GNU General Public License v3.0. See the LICENSE file for more details.

# Notes

If you encounter any issues or have suggestions, feel free to open an issue or submit a pull request!




