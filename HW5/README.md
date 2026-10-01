# Jay Vakil Homework 5 - Lighting

HW5 adds lighting to the existing HW4 passing-practice scene. The pitch, players,
coach, cones, duffel bag, ball, colors, placements, and passing animation are
preserved. The three projection modes and WASD/arrow controls are also retained.

## Build and run

Requires the same OpenGL, GLU, and GLUT development libraries as HW4 and ex13.
All homework code is in `hw5.c`; no CSCIx229 library or external assets are needed.

```sh
cd HW5
make
./hw5
```

`make clean` removes the generated `hw5` executable and `hw5.o` object file.

## Existing controls

| Key | Action |
| --- | --- |
| `m` | Cycle orthogonal, perspective, and first person |
| Arrow keys | Orbit in overhead modes; look around in first person |
| `W` / `S` | Walk forward / backward in first person |
| `A` / `D` | Strafe left / right in first person |
| `+` / `-` | Zoom in / out |
| `0` | Reset cameras and field of view |
| Space | Pause / resume the passing animation |
| `r` | Restart the passing animation |
| Esc | Exit |

## Lighting controls

| Key | Action |
| --- | --- |
| `l` | Turn all scene lighting on / off |
| `u` | Toggle the visible sun and its sunlight |
| `p` | Stop / resume the automatic light orbit |
| `<` / `>` | Move the light by 5 degrees; also stop automatic orbit |
| `[` / `]` | Lower / raise the light, from 0.5 to 10 units |
| `b` / `B` | Decrease / increase ambient intensity |
| `v` / `V` | Decrease / increase diffuse intensity |
| `c` / `C` | Decrease / increase specular intensity |
| `n` / `N` | Decrease / increase material shininess |
| F1 | Toggle smooth / flat shading |
| F2 | Toggle the local-viewer specular lighting model |
| F3 | Toggle light orbit radius between 5 and 1 units, as in ex13 |
| `o` / `O` | Next / previous inspection view |

A golden sun above the far side of the pitch adds warm directional sunlight
through `GL_LIGHT1`. It uses the same light-setup pattern as ex13, with the
position's fourth component set to zero so the incoming rays are parallel.
The visible sun is drawn with the existing handmade sphere. It stays fixed while
the original point light moves. `u` toggles the sun and its contribution; `l`
disables all lighting while leaving the enabled sun visible. Sunlight also applies
to isolated inspection objects; use `u` to compare them with only the moving light.
The sun marker itself is drawn only in the complete scene.

The white marker shows the point light's position. The light starts 4 units above
the pitch and orbits at 45 degrees per second. Space stops only the players/ball;
`p` stops only the light. Use both to freeze the scene, then `<`, `>`, `[`, and `]`
to compare illumination from different positions. Ambient, diffuse, and specular
intensities range from 0 to 100 percent. Shininess ranges from 0 to 128.

Inspection views cycle through **scene, player, bag, cone, ball**. Selecting a
view switches to orthogonal projection and frames that object; the smaller bag,
cone, and ball are enlarged for inspection. The orbit radius is halved in these
views. Cycle back to Scene to restore the complete layout and its default zoom.
The camera and lighting controls remain available while inspecting an object.

For a clear normal-vector demonstration from startup, press `u` to turn off the
sun, press `o` twice to inspect the bag, press `p` to stop the light, and use
`<` / `>` to compare its sides and handles under the moving point light alone.
Use `l` to compare the lit result with the original vertex colors.

## Exercise code and normals

Most additions follow **exercise 13**: the light variables, intensity arrays,
`GL_LIGHT0`, `GL_NORMALIZE`, color materials, specular/shininess settings,
`glShadeModel`, the light marker, orbital motion, and scene/object selection.
Lighting is configured after the camera transform and disabled before drawing text.

The values come from the source appropriate to each part of the program:

| Values | Source or reason |
| --- | --- |
| `th=20`, `ph=30`, `fov=55`, `dim=11`, eye `(0,1.7,8)` | Unchanged HW4 camera defaults |
| Object dimensions, positions, colors, and six-second passing loop | Unchanged HW4 scene |
| `light=1`, `move=1`, `distance=5`, `smooth=1`, `local=0`, `zh=90` | ex13 defaults |
| F3 distance choices `1` / `5` | ex13's distance control |
| Ambient/diffuse/specular `20/80/30` | Adjusted from ex13's `10/50/0` for this scene's appearance and visible highlights |
| `shininess=4`, `shiny=16` | ex13's `2^shininess` rule, using a more concentrated highlight than its default `1` |
| `ylight=4` | Places the light above the players; ex13's `0` would place it at ground level |
| Orbit speed `45` degrees/second, manual steps `5` degrees and `0.25` height units | Slower automatic motion and larger manual steps for this larger scene; ex13 uses `90`, `1`, and `0.1` |
| Sun direction and warm light colors | Additional sunlight requested for HW5, using the same OpenGL light-setting calls |

These scene-specific values preserve HW5's appearance while retaining ex13's
lighting calculations. The bag, straps, and cone require normals derived from
their own vertices and dimensions; copying normals for unrelated exercise objects
would not give the correct illumination.

Exercise 13's cube and sphere normals are applied to the existing geometry. Its
`SolidPlane()` fuselage and nose provide the radial-cylinder and sloped-cone normal
patterns. Its `triangle()` cross-product calculation is adapted by `Normal()`
for the bag's triangles and strap faces. The required scene-specific adaptations
are the cone's radius/height, the bag/strap vertex arrays, and outward orientation.
The original strap side winding faces inward, so the normal calculation reverses
those edges. Cylinder, cone, and bag caps have separate outward planar normals.
`GL_NORMALIZE` keeps normals at unit length after the existing object scales.
The cone uses the same triangles as its original fan, with a separate apex normal
for each sector, following ex13's nose.

The light's ex13 controls are remapped where they would conflict with HW4:
`p` replaces the light-movement `m`, and `b/B`, `v/V`, `c/C` replace `a/A`, `d/D`,
`s/S`. Its orbit uses HW4's elapsed-time update so the light and passing animation
can stop independently and resume without jumping.

**Exercise 10** supplies the polygon offset on the pitch to avoid coplanar
object-base artifacts. Exercises 11 (GPS traces) and 12 (color spaces) were
reviewed; their unrelated demonstrations are not added to this scene.

The scene includes cylindrical limbs, conical markers, and a tapered polygonal
bag with solid curved straps, beyond its cube and sphere components. All objects
are built directly from OpenGL vertices. There are no GLU/GLUT-generated objects,
imported models, pyramids, or snowmen. GLU/GLUT are used for the camera, window,
input, and text, as in the exercises. No CSCIx229 dependency is used.

## Approximate time spent

**[Enter your approximate total time spent on HW5 before submitting.]**
