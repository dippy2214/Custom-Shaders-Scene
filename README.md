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
exactly when each shader section is run. A single shader is technically made up of each individual step, but for the purposes 
of this write up I will be calling each of the segments 'shaders' and the full shader a 'shader program'.

![shader pipeline](https://github.com/user-attachments/assets/43340ea9-ed24-427f-9b9b-7b0d76e2d5de)

The above image shows off the DirectX shader pipeline and each of the steps I could interact with. The only truly essential
steps were vertex and pixel shaders, since the rest could be automatically filled in to do nothing, but every step you see
that can take an input other than the previous shader were ones which I could edit myself. It is also worth noting that I
wrote these in HLSL, a high level language designed for making shaders, rather than the C++ that the rest of the program was
written in. I actually liked some of the features of HLSL a lot, especially the way it handled floats. However, I personally
think that allowing you to cast a float4 to a float and making the float the x value of the float4 is absolutely insane and 
should be banned (this caused me a lot of pain when I was being careless 😅).

### 💡 Lighting And Shadows
Making the lighting was an interesting task. This mostly happened in the pixel shader, to make sure that the visual effects 
looked detailed and nice. Making lighting work on a per vertex basis can look nice for more stylised projects, but part of
this being a uni project meant that I needed to meet certain requirements, and doing this in a more realistic way was one of
them.

#### 🧭 Directional Light
I started out with a simple directional light, which is a light that constantly shines on everything in the scene from a 
specific direction. This lighting was calculated by comparing the angle of the light to the pixel and normal of that pixel. 
Normals are an essential part of lighting, which it important when you start doing vector manipulation in the vertex shader, 
but I will talk about that more with heightmaps.

![image](https://github.com/user-attachments/assets/18b41f16-c990-4f50-80de-0285c9be97c0)

the angle between the light and normal is what determines the brightness, and the closer the angle is to 0 the brighter the
lighting effect will be, while everything beyond 90 will havce no light and be left black. The lighting shaders change color
by multiplying by a number between 0 and 1, meaning the original colors of the object are preserved.

#### 👉 Point Light
Point lights work on a very similar principle to directional light, but instead of the light coming from one specific direction
it comes from one specific point in space. This doesn't actually require much change from directional lights until we get to
shadows, but it is worth mentioning that we can start thinking about specular highlights now. 

This is an effect designed to mimic the perfect reflection that can occur only when an object is at a specific angle between the 
light and the viewer. For this effect I needed to perform a separate specular calculation to work out the half angle, which could 
then be used to calculate a specular highlight - something that can be applied additively at the end of all lighting calculations.

#### 🔦 Spot Light
Spot lights are lights that shine only within a specific cone. They expand on the principles of point lighting, with their own
known formulas to limit the cone they shine in applied on top. There is a lot you can tweak with these with the internal and
external cones, but I don't want to dwell on these too long as it is mostly known formulas which I didn't work out myself. The 
specular highlight was also built upon to limit the cone which it would function in.










