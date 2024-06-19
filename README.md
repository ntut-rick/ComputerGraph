# 110590049 final

## result

- load 3 different texture
- load 3 different `.obj` file
- animation
- shadow
- camera control
- animation control


![](https://imgur.com/JDM4Mvn.png)
![](https://imgur.com/2pWPjj2.png)

## difficulty 

- Explore and read an OBJ file, including its UV mapping, normals, and other attributes.
- Configure the GLTool program to ensure compatibility and seamless operation on both Linux and Windows platforms.
- Discover an OBJ file with a "futuristic world" aesthetic that is user-friendly and easy to work with.
- Aim to accomplish all the above objectives efficiently while maintaining a balanced and stress-free approach.

## require
* [cmake](https://github.com/Kitware/CMake/releases/download/v3.29.2/cmake-3.29.2-windows-x86_64.msi)

<!-- ###  ubuntu

* install [wsl](https://apps.microsoft.com/detail/9pdxgncfsczv?ocid=pdpshare&hl=en-us&gl=US)

```
sudo apt install g++
sudo apt install cmake

``` -->


## run
**recommend use linux**
### windows
<!-- install `cmake tools` in your vscode -->
```
./build/cg-course.exe
```


### linux
```bash
cmake -B build
cmake --build build
./build/cg-course
```
## key action

| Key Pressed | Action             |
| ----------- | ------------------ |
| 1           | switch to light 1  |
| 2           | switch to light 2  |
| 3           | switch to light 3  |
| 4           | switch to light 4  |
| space       | stop animation     |
| arrow up    | camera turn up     |
| arrow down  | camera turn down   |
| arrow right | camera turn  right |
| arrow left  | camera turn left   |
