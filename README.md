# Graphics Project
I have created this project for my independent study in computer graphics. I am following [Computer Graphics Programming in OpenGL with C++ (2nd Edition)](http://www.merclearning.com/titles/Computer-Graphics-Programming-in-OpenGL-with-C++-2E.html) for the basics of the project.

## Building
### Windows
```
git clone --recursive https://github.com/EliasWatson/Graphics-Project.git
cmake -S Graphics-Project -B Graphics-Project/build
cmake --build Graphics-Project/build
```
Then double-click the executable in Graphics-Project/build/Debug

### Linux
```
git clone --recursive https://github.com/EliasWatson/Graphics-Project.git
cd Graphics-Project
cmake .
make
./Graphics-Project
```