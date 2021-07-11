
LearnOpenGL Guide
========================

## Getting Started

### Project Structure
Main project folders:
 - include: third party headers (.h files)
 - libs: third party libraries (.lib files)
 - src: source files (.c/.cpp files)

### Third Party Dependencies 

#### GLAD
1. Generate loader at http://glad.dav1d.de/ with options:
 - Language: C/C++
 - Specification: OpenGL
 - API: __gl__ -> any version over 3.3
 - Profile: Core
 - Extensions: None
 - Options: Generate a loader

2. Add glad and KHR folders to project *include* folder
3. Add glad.c file to *src* folder

#### GLFW
1. Download pre-compiled library from http://www.glfw.org/download.html or download from source and 
compile with CMake (instructions at https://learnopengl.com/Getting-started/Creating-a-window)

2. Add compiled glfw3.lib to *libs* folder


### Setup Project
On solution view, right click on solution and select Properties.
On Configuration Properties -> VC++ Directories:
 - Include Directories: add *include* folder
 - Library Directories: add *libs* folder

On Linker -> Input
 - Additional Dependencies: add glfw3.lib and opengl32.lib (Included in VS2019)

