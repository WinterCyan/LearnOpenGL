## LearnOpenGL online tutorial implementation
*original website:* https://learnopengl.com

Ubuntu env configuration:
1. glfw
```shell
sudo apt-get install libglfw3-dev
git clone https://github.com/glfw/glfw
mkdir build && cd build && cmake ../ && make -j4 && sudo make install
```

2. glad [online service](https://glad.dav1d.de/), C/C++ -> OpenGL -> Version 4.5 -> Core -> Generate a Loader -> Generate -> Download & Unzip glad.zip

```shell
cp -r glad/include/* (glad & KHR) /usr/local/include/
```

3. glew
```shell
sudo apt-get install -y libglew-dev
```

4. glm & assimp
