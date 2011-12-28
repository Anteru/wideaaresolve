Wide AA tester
==============

Compiling
---------

Use MSVC 2010 and compile as x64. If the Python Tools for Visual Studio are not installed, the "FilterGenerator" project will not load -- it is not required to build as the shader is already generated. Besides the June 2010 DirectX SDK, the application doesn't have any external dependencies.

Compiling as x86 doesn't work due to alignment issues with XNA Math.

Generating the shader
---------------------

Run `FilterGenerator\Program.py` and pipe the output to fw.hlsl.

Notes
-----

The generated geometry is pretty much the worst case for aliasing. It has high contrast everywhere and features edges at all angles. Looking top-down also trashes the z-Compression (by filling it with random noise.)

A larger image results in a larger grid, however, at that point triangle throughput may start to get problematic (with the default image, it's already 400k triangles that get rendered.)

It should be straightforward to include new AA algorithms into this application by adding it to the `Filters::Resolve` method.