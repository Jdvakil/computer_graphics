# Jay Vakil Homework 3 - Scene in 3D

In this homework, we went over the basics of instantiating objects in 3D in OpenGL and glut. I created a scene of two players/humans passing a soccer ball on a pitch practicing before a game. Their coach is in the background watching them and their kit bag is on the pitch. There are also cones which are left on the pitch for guidance. The humans are  created using a cylinder, cube, and a sphere, along with two cubes for shoes. I also created a duffel bag to represent their kit bag. I made five cross-sections of the bag's outline, lined them up along its length with the end ones slightly smaller to create the taper, and joined them with triangles. For the straps, I placed small rectangles along an arch and joined them with quads, then drew it twice to make the two handles. The animation is timed to match the players kicking so it looks like they are playing pass. 


## Instructions

```
make
./hw3
```

Once you `make` in the folder, you should see `hw3 and hw3.o` files. Once it finishes "making", which should be pretty fast, you can run the program with `./hw3` and it should open up an OpenGL window showing the animation and objects. 


These are the instructions to interact and play around with this program:


| Keystroke | Action |
| --- | --- |
| Arrow keys (up/down/left/right)| Move the camera in the corresponding direction |
| `+` / `-` | Zoom in / out |
| `r` | Reset the animation |
| `0` | Reset the camera view |
| `space` | Pause/Play the animation|
| Esc | Exit |


A lot of the fundamentals were provided in `ex8.c` and `ex7.c` files provided by the professor. I also borrowed a bunch of code from my previous homework `hw2.c` and set up this new animation and 3D objects. Having the exercises and in-class walkthrough really helped because it taught me the fundamentals needed to complete this homework. 

Overall this assignment took me around 6-7 hours to complete. 