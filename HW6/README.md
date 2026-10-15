# Jay Vakil Homework 6 - Textures

This homework (HW6) builds on top of my previous homeworks and exercises that we went over in class(es). I am working on building this towards my final project as well. 

In this homework, we add textures to a scene with the assets from the previous homeworks: pitch, players, coach, cones, duffel bag, ball, colors, placements, and passing animation are
preserved. Apart from the assets the lighting aspects are also the same, the orbiting light source and sun provide lighting to the scene. 

The three projection modes and WASD/arrow controls are also retained.
New controls have been added from `ex14.c` and `ex15.c` that allows adding textures to the objects through the texture coordinates and material settings. 

I also write a python script, `img2bmp.py` which would convert the png/jpg files into 512x512 bmp texture files. 


## Build and run

```sh
cd HW6
make
./hw6
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
| F3 | Toggle light orbit radius between 5 and 1 units |
| `o` / `O` | Next / previous inspection view |

Most of the code has been reused from `ex10.c`, `ex12.c`, `ex13.c`, `ex14.c`, `ex15.c`, and `hw4.c`. 

## Approximate time spent

I spent around 6 hours on this homework which included looking at the code from the exercises, adding the functionalities into my scene, finding images, writing the code to convert images to bmp, and making sure the textures fit the objects. 

### References
I downloaded my textures from these sources:
- https://www.adidas.com/us/fifa-world-cup-26-trionda-finals-pro-ball/JY8928.html
- https://www.adidas.com/us/argentina-26-home-authentic-jersey/JM5897.html
- https://www.magnific.com/free-vector/seamless-green-grass-pattern_13187581.htm#fromView=search&page=1&position=1&uuid=9c49e65d-2287-48c9-b36e-3a3ed16818fa&track=ais_hybrid&query=grass
- https://www.magnific.com/free-psd/fiery-celestial-orb-glowing-lunar-display_408655936.htm#fromView=search&page=4&position=23&uuid=40947b3d-734d-446b-9916-00557251ac64&track=ais_hybrid&query=sun
- https://www.magnific.com/free-photo/original-brown-leather-texture-background_15340325.htm#fromView=search&page=1&position=3&uuid=17fc8947-2fdc-4fb7-a234-77562f0aef9b&track=ais_hybrid&query=leather