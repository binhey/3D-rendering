Yuan Xiong, CPSC 605, project 5. Apr 1st, 2014.
xiongyuanxy@gmail.com
Press 'i' to initialize.
Press 'w' to write image file again.
Press 'p' to switch projection mode.
Press 'q' to quit.

Usage ./objtrace [nrays] [image_file_name].

I implemented the raytracer for objects loaded from .obj file.
There is an texture mapping render image using uv cordinate.
Bounding box speeding.
BIH tree structure.
Refraction and reflection for the objects including the one from obj file.
Soft shadow by using area lights.
Dark corner by using light with minus illumination.
Decay and attenuation for the light according to distance(leaner, quadratic, cubic).
Alpha blend for the textured objects.
Checkboard on the plane.
