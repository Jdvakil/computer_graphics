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

Submit `hw6.c`, `makefile`, this README, and the eight runtime BMP files together:
`crate.bmp`, `block.bmp`, `ball.bmp`, `img5.bmp`, `grass.bmp`, `sun.bmp`, `leather.bmp`,
and `argentina_jersey.bmp`.
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

Three runtime BMPs are unchanged copies from the exercises; grass, ball, sun,
leather, and the Argentina jersey use the supplied images. All are uncompressed 24-bit images with power-of-two dimensions, and none
exceeds 512 by 512 pixels.

| File | Original source | Dimensions | Use |
| --- | --- | --- | --- |
| `grass.bmp` | Supplied grass JPEG, resized and converted to BMP | 512 x 512 | Grass tiled across the pitch |
| `sun.bmp` | `sun.jpg`, converted with `img2bmp.py` | 512 x 512 | Surface of the sun |
| `leather.bmp` | Supplied `leather.jpg`, converted with `img2bmp.py` | 512 x 512 | Duffel body, both ends, and underside |
| `argentina_jersey.bmp` | Supplied Argentina shirt BMP | 512 x 512 | Shirts on both players, the coach, and the player inspection view |
| `crate.bmp` | `exercises/ex14/crate.bmp` | 256 x 256 | Grain detail on trousers, shoes, and tan bag straps |
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
- Player and coach shirts use a separate jersey texture. The chest faces local +X and
  samples the shirt photograph's central torso, keeping its badges upright and
  excluding the background. The back, sides, top, and bottom sample plain fabric
  below the badges. A neutral matte material preserves the blue and white under
  lighting. All three people wear the jersey; `t` restores their original
  blue/red/gray shirt colors when textures are disabled.
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
- The bag uses HW5's original nine-point profile, five tapered sections,
  triangles, face normals, and solid end caps. Its faceted silhouette matches
  that earlier implementation. The separate corner-cutting and smooth-normal
  algorithms have been removed to keep the geometry directly based on HW5.
- Bag coordinates map its length to 0..1 and its profile to 0..2 using the
  per-vertex texture approach in ex14/ex15. Adjacent triangles share coordinates;
  the profile's final vertex reaches 2 instead of restarting at 0. Two mirrored
  repeats make the texture meet at the profile seam. This simple mapping follows
  profile indices, so texture scale varies on differently sized profile edges.
  The source BMP is unchanged.
- Bag ends use planar coordinates across their width and height. Their texture
  orientation is independent of the side mapping.
  Leather uses a neutral material and a modest specular reflection so its brown
  image remains visible under the moving light. It is applied only to the body.
  The straps retain their original tan color, grain texture, solid geometry, and
  texture coordinates. Their original material settings are restored before drawing them.
  Textures off restores the bag's original brown colors and HW5 geometry.

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

Most of the runtime implementation is reused from the earlier homework and
exercises. HW5 is the immediate source for the scene built through the earlier
assignments. The table distinguishes unchanged functions from adaptations and
HW6-specific additions; using the same OpenGL API alone does not make new code
a verbatim copy of an exercise.

| HW6 implementation | Local source | Reuse and changes |
| --- | --- | --- |
| `Print`, `ErrCheck`, `Fatal` | [HW5/hw5.c](../HW5/hw5.c) | Function definitions unchanged apart from comments/formatting. |
| `Project`, `Normal`, `walk`, `special`, `reshape`, `idle` | [HW5/hw5.c](../HW5/hw5.c) | Function definitions unchanged apart from comments/formatting. |
| `key`, `display`, `main` | [HW5/hw5.c](../HW5/hw5.c) | Existing controls, animation, camera setup, lights, scene arrangement, and callbacks retained; texture toggle, loading, materials, and textured draw calls added. |
| `cube` | [HW5/hw5.c](../HW5/hw5.c), [ex15/ex15.c](../exercises/ex15/ex15.c) | HW5 vertices/normals with ex15's four-corner UV layout; jersey selection and cloth material added. |
| `Vertex`, `sphere` | [HW5/hw5.c](../HW5/hw5.c) | Original manually generated sphere and radial normals; texture coordinates, finer ball steps, ball material, and rolling rotation added. |
| `cylinder`, `cone` | [HW5/hw5.c](../HW5/hw5.c), [ex14/ex14.c](../exercises/ex14/ex14.c) | HW5 geometry/normals with adapted wrap and radial texture coordinates. |
| `shoe`, `human`, `straps` | [HW5/hw5.c](../HW5/hw5.c) | Original assembly, dimensions, and geometry; texture IDs and explicit strap coordinates added. |
| `pitch` | [HW5/hw5.c](../HW5/hw5.c), [ex14/ex14.c](../exercises/ex14/ex14.c) | Original ground dimensions/normal and polygon offset; ex14's textured-square pattern repeated over unit patches with a grass material. |
| `duffel` | [HW5/hw5.c](../HW5/hw5.c) | Original profile, sections, triangles, `Normal` calls, and cap geometry restored; leather coordinates and material added. |
| `Reverse`, `LoadTexBMP` | [ex14/loadtexbmp.c](../exercises/ex14/loadtexbmp.c) | Both function definitions copied unchanged. Credit: Willem A. (Vlakkies) Schreuder, CSCIx229. |
| `Texture`, `LoadTextures` | [ex14/ex14.c](../exercises/ex14/ex14.c), [ex15/ex15.c](../exercises/ex15/ex15.c) | Adapted enabling, binding, `GL_MODULATE`, loading after context creation, and disabling; filenames, texture IDs, helper organization, and wrap choices are specific to HW6. |
| Material setup | [ex13/ex13.c](../exercises/ex13/ex13.c), [ex15/ex15.c](../exercises/ex15/ex15.c) | Uses the exercises' `glMaterialfv` approach; per-material values and `glPushAttrib`/`glPopAttrib` isolation are HW6 additions. |

The remaining scene-specific implementation consists of `MaterialCoord` and
`CubeCoord` image crops, UV formulas for the actual surfaces, the ball photograph
projection/rolling angle, material values, and texture configuration. These are
adaptations needed to apply the selected images to this scene and are not claimed
as code copied from the exercises. All three Argentina jerseys, the grass, ball,
sun, leather bag, existing lighting, and camera controls are retained.

Source comparison on 2026-10-14: ignoring comments, whitespace, and lines with
only punctuation, 572 of 778 substantive lines (about 74%) have an ordered match
in C/C++ source/header files under HW1-HW5 or `exercises`. Requiring matching runs
of at least three substantive lines gives 487 of 778 (about 63%). This measures
textual overlap, not authorship or an instructor's grading rule; adapted calls
with extra texture arguments can differ even when their geometry comes from HW5.
The nine unchanged HW5 functions and the two loader functions above were also
compared as complete function definitions. A forced rebuild with `make -B`
completed without warnings; interactive appearance still needs a visual check.

`img2bmp.py` is a separate, HW6-specific asset conversion helper. It is not part
of the OpenGL runtime, is not included in those counts, and is not needed to run
the submitted BMP files.

The sphere drawing remains HW5's handmade mesh; ex15's `glutSolidSphere()` is not
used. There are no GLU/GLUT object generators, pyramids, imported models, or
CSCIx229 archive dependencies. All geometry and the BMP loader are in `hw6.c`.
