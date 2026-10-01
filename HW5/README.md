# Jay Vakil Homework 5 - Lighting

This homework (HW5) builds on top of my previous homeworks and exercises that we went over in class(es). I am working on building this towards my final project as well. 

In this homework, we add lighting to a scene with the assets from the previous homeworks: pitch, players, coach, cones, duffel bag, ball, colors, placements, and passing animation are
preserved. 

The three projection modes and WASD/arrow controls are also retained.
New controls have been added from `ex13.c` that allows changing the lighting parameters and the movement of the orbitting ball. I also added a sun object that can be turned on and off.  

## Build and run

```sh
cd HW5
make
./hw5
```

## Existing controls (from HW4)

| Key | Action |
| --- | --- |
| `m` | Cycle orthogonal -> perspective -> first person |
| Arrow keys | Move the view around |
| `+` / `-` | Zoom in / out in all three modes |
| `WASD` | Walk around in first person|
| Arrow keys | Look around in first person |
| `0` | Reset both cameras and field of view, keeping the current mode |
| Space | Pause / resume animation |
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

Most of the code has been reused from `ex10.c`, `ex12.c`, `ex13.c`, and `hw4.c`. 

## Approximate time spent

I spent around 5-6 hours on this homework which included looking at the code from the exercises, adding the functionalities into my scene and then debugging. 