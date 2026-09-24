# Jay Vakil Homework 4 - Projections and first-person navigation

HW4 keeps HW3's pitch, two animated players, coach, four cones, duffel bag,
and moving ball. Their geometry, colors, placements, and six-second passing
animation are unchanged. The initial camera is pulled back to show the whole pitch.

## Build and run

Use the same OpenGL, GLU, and GLUT development libraries as HW3 and exercise 9
(on Debian/Ubuntu: `freeglut3-dev`, `libglu1-mesa-dev`, and `libgl1-mesa-dev`).
GLFW is no longer required.

```sh
cd HW4
make
./hw4
```

## Controls

| Key | Action |
| --- | --- |
| `m` | Cycle orthogonal → perspective → first person → orthogonal |
| Left / Right (overhead) | Orbit around the scene |
| Up / Down (overhead) | Raise / lower the viewing angle |
| Page Up / Page Down (overhead) | Zoom out / in, as in ex9 |
| `+` / `-` | Increase / decrease perspective field of view, as in ex9 |
| Up / Down or `W` / `S` (first person) | Walk forward / backward |
| Left / Right (first person) | Turn left / right |
| `A` / `D` (first person) | Strafe left / right |
| Page Up / Page Down (first person) | Look up / down |
| `0` | Reset both cameras and field of view, keeping the current mode |
| Space | Pause / resume animation |
| `r` | Restart the passing animation |
| Esc | Exit |

Controls, the current mode, and animation status appear on screen using HW3's
`Print()` helper. Increasing FOV widens the perspective view; it does not affect
orthogonal projection. FOV is limited to 15–100 degrees.

Both overhead modes share ex9's orbital eye position, looking toward `(0,1.3,0)`.
Switching between them changes the projection while preserving the camera.
Elevation is limited to 5–85 degrees and zoom to 2–40 units. Reset restores a
slanted view of the whole scene; portrait windows retain the same horizontal framing.

First person starts at `(0,1.7,8)`, facing the pitch. Walking follows the current
heading at a fixed height, with X/Z limited to ±20 units. Its heading and look
angle are separate from the overhead camera. There is no object collision detection.

## Source structure and exercise references

- HW3 supplies the object routines, scene instances, passing animation, `Print()`,
  `ErrCheck()`, `Fatal()`, `idle()`, and GLUT window/callback setup in `main()`.
- Exercise 9 supplies the `Project()` structure (`gluPerspective` / `glOrtho`),
  the orbital `Ex`, `Ey`, `Ez` calculation and `gluLookAt` in `display()`, and the
  `special()`, `key()`, and `reshape()` callback patterns. The mode switch is
  extended to three modes, with a separate first-person eye and heading.
- Exercise 10 supplies the small polygon depth offset applied to the pitch to
  avoid coplanar ground/object-base artifacts.

GLUT is used for the window, input, timing, and text; GLU is used for camera and
projection utilities. All objects are manually constructed with OpenGL vertices.
No GLU/GLUT object generators, imported objects, or CSCIx229 library are used.
`hw3.c` remains the original reference; the makefile builds `hw4.c`.
