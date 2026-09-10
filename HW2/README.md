# Jay Vakil Homework 2 - Lorenz Attractor

In this homework, we went over the basics of OpenGL by rendering a simple scene and adding basic user interaction. To make this happen, we implemented a 3D Lorenz Attractor using OpenGL/GLUT.  

A Lorenz attractor system consists of a set of three ordinary differential equations (ODEs), and it looks like a butterfly. It was originally used in Atmospheric studies, created by Edward Lorenz, and thus the name. It models how fluids move when they are heated and cooled, and thus useful in weather prediction models. While the equations and the attractor itself was created in 1963, the fundamentals are still used in academic papers, with this interesting [paper](https://arxiv.org/pdf/2410.06452v1) modeling the system using modern machine learning methods. Another interesting find that I came across was building this system using an [analog electronic circuit](https://seti.harvard.edu/unusual_stuff/misc/lorenz.htm). 

## Instructions

```
make -f makefile
./hw2
```

The supplied makefile now targets `hw2` and uses `gcc -Wall`. It retains its
Linux, macOS, and Windows/MinGW branches. Linux needs GCC, Make, OpenGL, GLU,
and GLUT development libraries (on Debian/Ubuntu: `build-essential freeglut3-dev
libglu1-mesa-dev`). Windows also needs GLEW. A graphical desktop is required.
Use `make -f makefile clean` to remove build products.

Click the window to give it keyboard focus. Uppercase parameter keys mean
Shift plus the letter; lowercase decreases and uppercase increases.

| Key / mouse | Action |
| --- | --- |
| Arrow keys | Orbit left/right and tilt up/down |
| Left mouse drag | Orbit freely |
| `+` / `-` | Zoom in / out (`=` also zooms in) |
| Mouse wheel | Zoom, where supported by GLUT |
| `s` / `S` | Decrease / increase sigma by 0.5 (0.5–30) |
| `p` / `P` | Decrease / increase rho by 1 (0–60) |
| `b` / `B` | Decrease / increase beta by 0.1 (0.1–10) |
| Space | Toggle slow camera rotation |
| `a` | Toggle axes and ground grid |
| `h` | Toggle control help |
| `0` | Restore initial view and zoom |
| `r` | Reset parameters, view, and toggles to startup defaults |
| Esc | Exit |

Try Space for a rotating view, then change rho with `p`/`P` to explore changes
in the trajectory. Press `r` to recover the initial butterfly at any time.
Parameter changes regenerate the trace immediately and update its framing.
Resizing preserves proportions; portrait windows expand the vertical field of
view to retain the trace. Small windows show a reduced text overlay.

The equations are

```text
dx/dt = sigma (y - x)
dy/dt = x (rho - z) - y
dz/dt = x y - beta z
```

The integrator follows the supplied `lorenz.c`: explicit Euler, simultaneous
derivatives evaluated at the old point, `dt = 0.001`, initial point `(1,1,1)`,
and 50,000 steps (50 time units, 50,001 vertices including the initial point).
Defaults match that file: sigma = 10, rho = 28, beta = 2.6666 (approximately
8/3). No transient is discarded. Camera motion changes only the view; Space
does not advance simulation time. The orthographic camera uses z as up and
orbits the trajectory's bounding-box center.

Euler is a first-order approximation. Chaotic trajectories are sensitive to
initial conditions and numerical error, so this finite trace illustrates the
system rather than predicting its exact long-term orbit. Parameter controls
are bounded; a finite-value/magnitude check stops integration and displays a
reset message if it fails. The original `lorenz.c` and `ex7/ex7.c` remain as
references; the standalone visualization is `hw2.c`.
