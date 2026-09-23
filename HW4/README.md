# Jay Vakil Homework 4 - Projections and first-person navigation

This is the exact HW3 passing-practice scene: the same pitch, two animated players,
coach, four cones, duffel bag, and moving ball, with the original geometry, colors,
placements, and six-second animation. HW4 adds three camera modes. The default
view is pulled back to show the whole pitch.

## Build and run

Requires a C compiler, OpenGL development files, and GLFW 3 development files
(for example, `libglfw3-dev` and `libgl1-mesa-dev` on Debian/Ubuntu).

```sh
cd HW4
make
./hw4
```

GLFW handles the window, keyboard input, and timing. This implementation uses no
GLU, GLUT, CSCIx229 library, ready-made object routines, or imported objects.
All geometry is drawn directly with OpenGL, as in HW3. Controls and the current
mode appear in the window title; the full control list is also printed at launch.

## Controls

| Key | Action |
| --- | --- |
| `m` | Cycle orthogonal → perspective → first person → orthogonal |
| Left / Right (overhead) | Orbit around the scene |
| Up / Down (overhead) | Raise / lower the viewing angle |
| `+` / `-` (overhead) | Zoom in / out (`=` also zooms in) |
| Up / Down or `W` / `S` (first person) | Walk forward / backward along the current heading |
| Left / Right (first person) | Turn left / right |
| `A` / `D` (first person) | Strafe left / right |
| Page Up / Page Down (first person) | Look up / down |
| `0` | Reset both cameras and zoom, keeping the selected mode |
| Space | Pause / resume animation |
| `r` | Restart the passing animation |
| Esc | Exit |

Both overhead modes share the same eye position, target, angles, and zoom state.
Switching projection changes only the projection matrix. Reset restores a slanted
view of the whole scene, including in portrait windows. Overhead elevation is
limited to 5–85 degrees and zoom to 2–40 units to keep the camera valid.

First person starts at `(0, 1.7, 8)`, facing the pitch. Movement stays at eye height,
with X/Z limited to ±20 units. It has a separate heading and look angle, so visiting
this mode preserves the overhead view. There is no object collision detection.

## Exercise references

- Exercise 9 supplies the projection setup, aspect-ratio handling, orbital camera,
  and keyboard control pattern. `glFrustum` implements perspective without GLU;
  inverse translations and rotations implement the camera without `gluLookAt`.
- Exercise 10 supplies polygon offset: the pitch receives a small depth offset to
  prevent coplanar object bases from fighting with the ground. Its CSCIx229 helper
  library is not used.

`hw3.c` is retained as the original reference; the makefile builds only `hw4.c`.
