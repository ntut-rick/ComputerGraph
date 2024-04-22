# Niu-Bi de CG Midterm Work

This project demonstrates basic OpenGL rendering with user interaction functionalities such as object manipulation and camera control.

## Features

- **Object Rendering:** Load and render 3D objects in various formats.
- **User Interaction:** Control object rotation, translation, and camera movement.
- **Texture Mapping:** Apply textures to objects for enhanced visual appeal.
- **Menu System:** Utilize menus to switch between different rendering modes and color modes.

## Dependencies

- **OpenGL:** The project utilizes OpenGL for graphics rendering.
- **FreeGLUT:** FreeGLUT is used for window management and user input handling.
- **STB Image:** STB Image library is used for loading textures.
- **Filesystem:** C++17 Filesystem library is used for file manipulation.

## Installation

1. Ensure you have the necessary dependencies installed.

2. Compile the project:

   ```
   cmake --build .\build-msvc\       
   ```

3. Run the executable:

   ```
   .\build-msvc\Debug\ComputerGraph.exe .\TestFile\gourd.obj .\TestFile\octahedron.obj .\TestFile\teapot.obj .\TestFile\teddy.obj
   ```

## Usage

- **Navigation:** Use mouse drag to rotate the object and mouse wheel to zoom in/out.
- **Object Control:** Press 'w', 's', 'a', 'd' keys to translate the object.
- **Menu Options:** Right-click on the window to access menu options for color modes, render modes, and transform modes.


## Credits

- Developed by Myself

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
