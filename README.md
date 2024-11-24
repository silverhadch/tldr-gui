# **TLDR GUI**

A simple and user-friendly graphical interface for the TLDR command-line tool, built using FLTK. Contributions are highly encouraged to make it even better!

---

## **Features**
- **Search TLDR Pages:** Quickly search for a command and view its TLDR page with ease.
- **Update TLDR Cache:** Refresh your local TLDR cache with a single click.
- **Responsive UI:** Non-blocking operations ensure the interface remains smooth and responsive while running tasks in the background.
## This GUI only works with the npm-version of tldr!!!

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

---

## **Dependencies**
Ensure the following dependencies are installed on your system:

### 1. **FLTK (Fast Light Toolkit)**
Used for creating the GUI.
- On **Debian-based systems**:
  ```bash
  sudo apt-get install libfltk1.3-dev
  ```
- On **Arch-based systems**:
  ```bash
  sudo pacman -S fltk
  ```

### 2. **TLDR CLI Tool**
Provides the command-line interface functionality.
- Install via `npm` (requires Node.js):
  ```bash
  npm install -g tldr
  ```

---

## **Installation**
If you’re using a Debian-based system, you can download the `.deb` package provided in the repository for easy installation.

---

## **How to Compile**
Follow these steps to build the program from source:

1. **Clone the repository:**
   ```bash
   git clone https://github.com/silverhadch/tldr-gui
   cd tldr-gui
   ```

2. **Compile the source code:**
   ```bash
   g++ -o tldr_gui src/main.cpp -lfltk
   ```

3. **Run the program:**
   ```bash
   ./tldr_gui
   ```

---

## **License**
This project is licensed under the [GNU General Public License v3.0](LICENSE).

---

## **Contributing & Support**
Encounter an issue? Have a suggestion?  
Feel free to [open an issue](https://github.com/silverhadch/tldr-gui/issues) or submit a pull request!

We welcome contributions to improve and expand this project.
