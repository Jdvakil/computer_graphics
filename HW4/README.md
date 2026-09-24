# Jay Vakil Homework 4 - Projections

This homework keeps the objects, scene, and animations from HW3, along with projections between `orthogonal`, `perspective`, and `first-person mode`.
The projection code is built on top of in-class exercises, especially exercise 9. I have reused code from HW3 (`hw3.c`) and `ex9.c`to complete this homework. 

## Build and run

```sh
cd HW4
make
./hw4
```

## Controls

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

