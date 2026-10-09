# Jay Vakil Homework 6 - Textures and Lighting

HW6 builds directly on the current `HW5/hw5.c`. It keeps the same players, coach,
ball, cones, duffel bag, pitch, sun, lighting values, animation, three camera modes,
and navigation controls. Textures and their coordinates are added to the existing
manually constructed objects, using the examples in exercises 14 and 15.

## Build and run

Use the same OpenGL, GLU, and GLUT development libraries as HW5.
Run from the HW6 directory so the exercise BMP loader can find the images:

```sh
cd HW6
make
./hw6
```

Submit `hw6.c`, `makefile`, this README, and all four BMP files together.
The makefile builds `hw6` directly from source. There are no external assets or
prebuilt libraries required beyond the system OpenGL/GLU/GLUT libraries.

## Texture controls

| Key | Action |
| --- | --- |
| `t` / `T` | Toggle textures on/off to compare with HW5 |

Textures start enabled. As in ex15, `GL_MODULATE` combines the texture image with
the existing colors and lighting. Lighting still changes the appearance of the
textured surfaces as the point light moves. `m` remains the camera-mode key.

## Existing controls

| Key | Action |
| --- | --- |
| `m` | Cycle orthogonal, perspective, and first person |
| Arrow keys | Orbit overhead; look around in first person |
| `W` / `S` | Walk forward/backward in first person |
| `A` / `D` | Strafe left/right in first person |
| `+` / `-` | Zoom in/out |
| `0` | Reset both cameras and field of view |
| Space | Pause/resume the passing animation |
| `r` | Restart the passing animation |
| `l` | Turn scene lighting on/off |
| `u` | Toggle the sun and directional sunlight |
| `p` | Stop/resume the point light's automatic orbit |
| `<` / `>` | Move the point light manually and stop its automatic orbit |
| `[` / `]` | Lower/raise the point light |
| `b` / `B` | Decrease/increase ambient intensity |
| `v` / `V` | Decrease/increase diffuse intensity |
| `c` / `C` | Decrease/increase specular intensity |
| `n` / `N` | Decrease/increase material shininess |
| F1 | Toggle smooth/flat shading |
| F2 | Toggle the local-viewer lighting model |
| F3 | Toggle light radius between 6 and 3 units, as in the current HW5 |
| `o` / `O` | Next/previous inspection view: scene, player, bag, cone, ball |
| Esc | Exit |

To inspect the mapping closely, cycle through the individual objects with `o`.
To check textures under different lighting, stop the players with Space and the
light with `p`, then move the light with `<` / `>` and `[` / `]`. The sun can be
turned off with `u` to isolate the point light. All these controls retain HW5's
behavior.

## BMP sources and mapping

Every BMP is an unchanged copy from the exercises. All are uncompressed 24-bit
images with power-of-two dimensions, and none exceeds 256 by 256 pixels.

| File | Original source | Dimensions | Use |
| --- | --- | --- | --- |
| `crate.bmp` | `exercises/ex14/crate.bmp` | 256 x 256 | Grain detail on the pitch, shirts, trousers, shoes, bag, and straps |
| `block.bmp` | `exercises/ex14/block.bmp` | 128 x 128 | Bright check pattern on the cones, including their bases |
| `img3.bmp` | `exercises/ex15/img3.bmp` | 64 x 64 | Pi graphic wrapped around the existing colored ball |
| `img5.bmp` | `exercises/ex15/img5.bmp` | 128 x 128 | A small skin-tone region for the heads and arms |

The provided exercise images do not include grass or fabric photographs. Texture
coordinates select a grain region inside the crate image without its border or
cross-brace, and a skin-tone region of img5 without its facial features. This
reuses the supplied images as material detail while keeping HW5's original colors.
No BMP pixels are generated, edited, or replaced.

Texture coordinates are supplied explicitly for every vertex of each textured
surface, including cylinder/cone/bag caps and the solid strap ends:

- Cube faces use ex15's four-corner square mapping.
- The pitch repeats ex14's square mapping over one-unit patches on the same
  14-by-12 ground plane. The texture-off view uses HW5's original single quad.
- Spheres map longitude around the image and latitude from pole to pole; the ball
  image is offset half a turn so the graphic faces the initial camera.
- Cylinders wrap coordinates around the sides and along their length; their caps
  use ex14's circular mapping.
- Cones use ex14's radial mapping, repeated twice across their diameter.
- Bag triangles share coordinates along the bag's length and around its profile;
  the profile seam reaches coordinate 1 instead of wrapping prematurely to 0.
  Their end caps use planar coordinates.
- Straps follow the arch and the four sides of each cross section, with separate
  coordinates on the two end caps.

The only untextured objects are the sun and moving light marker. They represent
light sources and remain solid colors so their positions are easy to identify.
All physical scene objects are textured by default. Texturing is disabled before
screen text is drawn.

## Code sources

- **HW5:** existing geometry, normals, scene values, animation, cameras, navigation,
  lighting controls, sun, and object-inspection modes.
- **Exercise 14:** `Reverse()` and `LoadTexBMP()` copied from `loadtexbmp.c`, texture
  binding/enabling, texture-coordinate repetition, and square/circle/cone patterns.
  The loader uses HW5's existing `Fatal()` and `ErrCheck()` directly.
- **Exercise 15:** combining textures with lighting using `GL_MODULATE`, cube-face
  coordinates, and disabling texturing after drawing an object.
- **Scene-specific additions:** choosing the source-image regions and extending
  the exercise coordinate patterns to HW5's spheres, cylinders, bag, and straps.

The sphere drawing remains HW5's handmade mesh; ex15's `glutSolidSphere()` is not
used. There are no GLU/GLUT object generators, pyramids, imported models, or
CSCIx229 archive dependencies. All geometry and the BMP loader are in `hw6.c`.
