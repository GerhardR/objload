# objload - simple C++ obj 3D model loader	
Copyright Gerhard Reitmayr, 2012

objload is a simple, C++, single header file library for loading 3D models in the obj format. It can load an obj mesh and represent vertices, texure coordinates, normals and polygonal faces. objload uses only the C++ standard library and should be very portable. 

objload also transforms the obj datastructures into a structure more suitable for OpenGL rendering, tesselating any larger polygons into triangles and creating single index arrays for the vertex, texcoord and normal datasets. The tesselation only works for convex polygons. 

## Usage
Just include the file objload.h in your project and you are set.

## Limitations
- no support for grouping, materials, other geometric primitives
- no writer for any format
- no OpenGL code included (this is a feature, not a bug :)
