# Jay Vakil Homework 6 - Textures and Lighting

HW6 builds directly on the current `HW5/hw5.c`. It keeps the same players, coach,
ball, cones, duffel bag, pitch, sun, lighting values, animation, three camera modes,
and navigation controls. Textures and their coordinates are added to the existing
manually constructed objects, using the examples in exercises 14 and 15.

## Implementation checklist

Work through these items in order. This is a plan for future changes, not a list
of completed features. It focuses on the code and rendered scene; README and
time-spent requirements are outside this checklist. The existing program already
has lighting, explicit texture coordinates, custom geometry, and three camera
modes. Preserve those features while improving the materials and presentation.
Optional polish below is not an additional assignment requirement.

### 1. Replace the placeholder materials

- [ ] Download suitable textures for grass, clothing fabric, bag fabric/leather,
  straps, shoe rubber, cone plastic, skin, and the ball. Reuse an image where the
  material actually matches; a unique image for every object is unnecessary.
  Prefer seamless images for repeating surfaces and a ball image designed for
  the chosen sphere or panel mapping, rather than an ordinary photograph.
- [ ] Export each image as an **uncompressed 24-bit BMP** supported by
  `LoadTexBMP()`. Use power-of-two width and height, preferably 128 or 256 pixels
  per side, and no dimension above 512. Check the actual format and dimensions,
  not just the filename extension.
- [ ] Expand the texture enum and `LoadTextures()` to load separate materials,
  for example `GRASS`, `FABRIC`, `BAG`, `STRAP`, `RUBBER`, `CONE`, `BALL`, and
  `SKIN`. Keep `NTEX` as the array count. Replacing `crate.bmp` alone cannot give
  the pitch and clothing different materials because both currently use `GRAIN`.
- [ ] Pass a texture ID into `cube()` and use it in both `Texture()` and every
  `MaterialCoord()` call. Select fabric for torsos and rubber for shoes; update
  `pitch()`, `human()`, `duffel()`, `straps()`, and `cone()` to use their intended
  materials. The cylinder and sphere helpers already accept texture IDs.

### 2. Fit the texture coordinates to the new images

- [ ] Remove the crate/forehead crop formulas from `MaterialCoord()` when those
  images are replaced. For full material images, start with
  `glTexCoord2d(s,t)`. Keep a crop only if the new image intentionally needs one.
- [ ] Set texture wrapping explicitly in the loader: use `GL_REPEAT` on axes
  that tile. Use `GL_CLAMP_TO_EDGE` where an image should end without repeating,
  such as the latitude axis of a full-sphere ball image. Check both S and T for
  each material rather than applying one setting indiscriminately.
- [ ] Choose repeat counts by surface size. Grass should have a believable scale
  across the pitch; fabric weave should stay similarly sized across torsos,
  trousers, bags, and straps. Use world dimensions or distance along a surface
  where useful instead of stretching one image over every differently sized part.
- [ ] Inspect seams in the `o` views. Cylinder and sphere seam vertices should
  share positions while their S coordinates differ by the full repeat count.
  Preserve the bag's final profile coordinate at the end of the range, and check
  caps and strap ends for stretching, orientation, and coverage. Some distortion
  at sphere poles and cone tips is unavoidable; keep conspicuous details away
  from those points.
- [ ] Match the cone mapping to its new texture. The current radial mapping is
  suitable for a surface pattern; horizontal traffic-cone bands need coordinates
  based on angle around the cone and height along it. Keep the bottom textured.

### 3. Make lighting show the intended materials

- [ ] Retune `glColor` values for the new images. Start with white for images
  that already contain their intended colors, then add deliberate tints. Keep
  `GL_MODULATE` so lighting still affects textures. Check especially the current
  dark green pitch tint and the skin tint, which multiply the image colors.
- [x] Remove the ball's procedural rainbow coloring when using a realistic ball
  texture: set a neutral color before drawing it and use `color=0` in both ball
  `sphere()` calls. Otherwise `Vertex()` changes the color at every vertex.
- [ ] Remove or reduce `cube()`'s fixed face-darkening multipliers when lighting
  is enabled. They currently bake directional shading into the colors in
  addition to the actual lighting. Let normals and light positions determine
  which faces are bright.
- [ ] Set specular response and shininess per material instead of using the same
  white specular material everywhere. Give grass and cloth little specular
  reflection; reserve stronger highlights for suitable plastic, leather, or ball
  surfaces. Set material state before drawing the affected geometry so it does
  not accidentally carry over to the next object.
- [ ] Verify outward normals with animation paused, sunlight disabled, and the
  point light moved around and above each object. Inspect bag triangles, strap
  sides, caps, and cones for incorrectly dark faces or abrupt unintended seams.
  Preserve `GL_NORMALIZE` because several objects use nonuniform scaling.

### 4. Keep the geometry compliant and improve silhouettes

- [ ] Resolve the standalone ball's interpretation under the ban on objects
  constructed entirely of spheroids. The humans include cylinders and the bag
  is a custom mesh, but the ball itself is spherical. Confirm that a natural
  spherical prop is acceptable in this mixed scene; if the rule is applied
  literally to each object, remove or replace it with a clearly non-spheroidal
  custom solid. More sphere subdivisions or a new texture do not resolve that
  wording. This is an interpretation concern, not a confirmed code defect.
- [ ] Preserve manually constructed geometry, solid end caps, and textured
  surfaces. Do not introduce GLU/GLUT object generators, pyramids, imported
  models, or new props assembled entirely from cuboids/spheroids. GLUT input and
  window functions and GLU camera utilities are already appropriate here.
- [ ] Optional polish: reduce the sphere's current 30-degree steps and the cone's
  30-degree steps to roughly 10-15 degrees if their silhouettes look visibly
  faceted at normal viewing distances. Keep sharp edges where they belong;
  smooth shading alone does not change a polygonal silhouette.
- [ ] Optional polish: make the scene easier to recognize with pitch markings,
  believable cone bands, and modest bag/shoe details. Add detail only after the
  existing materials, mapping, and lighting look convincing.

### 5. Run a final visual and build check

- [ ] Build from source with the provided makefile, using `make clean` followed
  by `make` in `HW6`, and resolve compiler warnings. Launch from that directory
  and verify every file named in `LoadTextures()` loads successfully. Check the
  terminal for messages from `ErrCheck()` while exercising the scene.
- [ ] Use `o` / `O` to inspect every object from multiple angles and zoom levels.
  Check bottoms, caps, seams, and strap ends. All physical surfaces should be
  textured, including the sun; the moving light marker retains its solid
  color. Check that objects meet the ground and animation does not introduce
  distracting intersections or floating parts.
- [ ] Test all four lighting/texture combinations with `l` and `t`, then leave
  both enabled by default. Pause the players with Space and the light with `p`;
  use `<` / `>` and `[` / `]` to verify that illumination moves across the
  textured surfaces. Test sunlight separately with `u` and verify the overlay
  remains readable and untextured.
- [ ] Exercise all three `m` camera modes, arrows, WASD in first person, zoom,
  reset, object inspection, pause/resume, and restart. Resize to wide and tall
  windows. Check for distorted proportions, clipped instructions, disappearing
  geometry at normal viewing distances, and unexpected state after switching
  between inspection and scene views.
- [ ] Optional polish: if textures shimmer while moving or zooming out, add
  mipmaps and an appropriate mipmap minification filter. Supply every required
  mip level before selecting that filter; the current `GL_LINEAR` filter works
  with the existing single-level textures.
- [ ] Reopen the program for a final default-view check: the scene should fit,
  materials should be recognizable, and both texture detail and lighting should
  be visible without first changing settings. Also inspect close-up views; a
  successful build alone does not verify the rendered result.

## Build and run

Use the same OpenGL, GLU, and GLUT development libraries as HW5.
Run from the HW6 directory so the exercise BMP loader can find the images:

```sh
cd HW6
make
./hw6
```

Submit `hw6.c`, `makefile`, this README, and the seven runtime BMP files together:
`crate.bmp`, `block.bmp`, `ball.bmp`, `img5.bmp`, `grass.bmp`, `sun.bmp`, and `leather.bmp`.
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

Three runtime BMPs are unchanged copies from the exercises; grass, ball, sun, and
leather use the supplied images. All are uncompressed 24-bit images with power-of-two dimensions, and none
exceeds 512 by 512 pixels.

| File | Original source | Dimensions | Use |
| --- | --- | --- | --- |
| `grass.bmp` | Supplied grass JPEG, resized and converted to BMP | 512 x 512 | Grass tiled across the pitch |
| `sun.bmp` | `sun.jpg`, converted with `img2bmp.py` | 512 x 512 | Surface of the sun |
| `leather.bmp` | Supplied `leather.jpg`, converted with `img2bmp.py` | 512 x 512 | Duffel body, both ends, and underside |
| `crate.bmp` | `exercises/ex14/crate.bmp` | 256 x 256 | Grain detail on shirts, trousers, shoes, and tan bag straps |
| `block.bmp` | `exercises/ex14/block.bmp` | 128 x 128 | Bright check pattern on the cones, including their bases |
| `ball.bmp` | Supplied `ball.png`, converted with `img2bmp.py` | 512 x 512 | Soccer ball surface in the scene and inspection view |
| `img5.bmp` | `exercises/ex15/img5.bmp` | 128 x 128 | A small skin-tone region for the heads and arms |

The provided exercise images do not include grass or fabric photographs. Texture
coordinates select a grain region inside the crate image without its border or
cross-brace, and a skin-tone region of img5 without its facial features. This
reuses the supplied images as material detail while keeping HW5's original colors.
The three runtime exercise BMPs retain their original pixels. `img3.bmp` is no
longer loaded or required.

Texture coordinates are supplied explicitly for every vertex of each textured
surface, including cylinder/cone/bag caps and the solid strap ends:

- Cube faces use ex15's four-corner square mapping.
- The pitch repeats ex14's square mapping over one-unit patches on the same
  14-by-12 ground plane. The texture-off view uses HW5's original single quad.
- Heads and the sun use longitude/latitude coordinates. The ball instead projects
  the circular surface photographed in `ball.bmp` onto both hemispheres. It samples
  within that disk to avoid the black background, with continuous coordinates at
  the hemisphere boundary and poles. Both texture axes clamp to the image edges.
  Since the source is a single photograph, the reverse repeats the design in
  mirror image; this is an approximation, not an exact full-surface map of the
  photographed ball. The source image also contains its original photographic
  shading. A purpose-made sphere texture would be needed to remove those limits.
- The ball uses a finer 5-degree handmade sphere mesh, radial normals, neutral
  material colors, and a modest specular highlight. `GL_MODULATE` combines the
  image with the scene lighting. It rolls as it travels between the players;
  the inspection view keeps it stationary so the arrow keys can inspect all sides.
- Cylinders wrap coordinates around the sides and along their length; their caps
  use ex14's circular mapping.
- Cones use ex14's radial mapping, repeated twice across their diameter.
- The bag body rounds HW5's original profile and taper with three passes of
  corner cutting, producing 72 points around 56 sections including the closed
  ends. Shared vertex normals come from the cross product of its two surface
  tangents. This smooths both the silhouette and lighting across the body and
  end rims; the F1 shading toggle remains available.
- Bag coordinates follow measured edge lengths along its length and around each
  cross section. Adjacent triangles share coordinates. Each profile covers two
  mirrored repeats, so coordinates 0 and 2 meet at the lower edge with matching
  texture values. `GL_MIRRORED_REPEAT` avoids abrupt joins between the photograph's
  different opposite edges. The source BMP is unchanged.
- Bag ends use planar coordinates at the central body's texture scale, meeting
  the body at the end-panel boundaries without pinching at the closed ends.
  Leather uses a neutral material and a modest specular reflection so its brown
  image remains visible under the moving light. It is applied only to the body.
  The straps retain their original tan color, grain texture, solid geometry, and
  texture coordinates. Their original material settings are restored before drawing them.
  Textures off restores the bag's original brown colors on the smoother body.

The sun uses the existing sphere's longitude/latitude texture coordinates,
sampling the central half of the image in each direction to exclude its baked-in
checkerboard background. The BMP remains unchanged. It is drawn with lighting disabled and white color
so its texture stays bright; the directional sunlight still illuminates the scene.
`t` toggles its texture along with the other textures, restoring the original
yellow sun when textures are off. `u` toggles both the sun and its sunlight.
Only the moving light marker remains untextured so its position is easy to identify.
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
  the exercise coordinate patterns to HW5's spheres, cylinders, bag, and straps;
  rounding the bag's control mesh and calculating its shared surface normals.

The sphere drawing remains HW5's handmade mesh; ex15's `glutSolidSphere()` is not
used. There are no GLU/GLUT object generators, pyramids, imported models, or
CSCIx229 archive dependencies. All geometry and the BMP loader are in `hw6.c`.
