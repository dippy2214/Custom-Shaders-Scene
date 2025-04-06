# Custom Shaders Scene 🖥

This project involved using C++ and HLSL to write a program which showed off custom shaders, built on top of a DirectX 3D
framework the university provided, which was intended to be a slice out of their own custom skateboard engine (for learning 
purposes). Learning about how shaders are made was very interesting to make, offloading processing to the graphics card 
and working with the DirectX shader pipeline. I really enjoyed working on what felt like a lower level compared to the 
rest of my uni projects so far, and even though this was the hardest module of my first term in third year (it wasn't 
even close), I am pleased to say I had an excellent time.

PLEASE NOTE: THIS IS NOT A COMPLETE BUILDABLE PROJECT! 

This repo contains all the code I wrote myself and a build of the final version of the project which I handed in for 
this module. This code is here for showing off my work only: due to the framework abertay uni provided to make this 
coursework, the full project is too large to upload to a single github repo (and potentially not allowed anyway).

## The Development Process 🛠

### 🖥 A Graphical Overhaul
To begin with, I will say that this module was hard. Because we were using the university's framework it was difficult to 
research the specific things we wanted, since often we couldn't find the specific ways to do stuff. This forced us down 
the much more difficult and much more interesting path of researching the how-tos and working out the implementations 
ourselves from scratch. A lot of my classmates struggled with this shift, but I think that my experience with the wiimote
project really aided me when adapting to this shift in how the university expected us to learn. I felt quite good 
throughout this module, keeping on top of learning and getting the hang of implementing all the new features we were taught.

The features that ended up in the final project were as follows:
- A lighting shader with support for shadows and specular highlights which includes point, directional and spot lighting
- Vertex manipulation with a heightmap + normal map shader for the terrain.
- Tessellation with different LODs based on distance to the camera
- A bloom post processing effect
- A 3D model showing off lighting shadows (I did not make the model loader)

These features presented many interesting challenges, and quite a few features I struggled with (such as a shader based 
minimap) didn't make it to the final project.

### ❓ What Are Shaders
Shaders are small programs which run on the GPU rather than the CPU, which can be used to make a variety of effects. There
are many different parts to shaders which are run at different parts of the render pipeline. The render pipeline defines 
exactly when each shader section is run. A single shader program is technically made up of each step, but for the purposes
of this write up I will be calling each of the parts 'shaders' and the full shader a 'shader program'.

![image](https://github.com/user-attachments/assets/43340ea9-ed24-427f-9b9b-7b0d76e2d5de)


