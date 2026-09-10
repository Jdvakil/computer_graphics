# Jay Vakil Homework 2 - Lorenz Attractor

In this homework, we went over the basics of OpenGL by rendering a simple scene and adding basic user interaction. To make this happen, we implemented a 3D Lorenz Attractor using OpenGL/GLUT.  

A Lorenz attractor system consists of a set of three ordinary differential equations (ODEs), and it looks like a butterfly. It was originally used in Atmospheric studies, created by Edward Lorenz, and thus the name. It models how fluids move when they are heated and cooled, and thus useful in weather prediction models. While the equations and the attractor itself was created in 1963, the fundamentals are still used in academic papers, with this interesting [paper](https://arxiv.org/pdf/2410.06452v1) modeling the system using modern machine learning methods. Another interesting find that I came across was building this system using an [analog electronic circuit](https://seti.harvard.edu/unusual_stuff/misc/lorenz.htm). 

## Instructions

```
make
./hw2
```

Once you `make` in the folder, you should see `hw2 and hw2.o` files. Once it finishes "making", which should be pretty fast, you can run the program with `./hw2` and it should open up an OpenGL window showing the Lorenz Attractor built, it looks like a butterfly. Click the window to interact with the system. 


These are the instructions to interact and play around with this program:

NOTE: uppercase parameters means increase and lowercase parameters mean decrease. For example the `sigma` starts at 10, so two entries of s `s + s` would change the sigma from 10 to 9 (-0.5, -0.5). 

| Keystroke | Action |
| --- | --- |
| Arrow keys (up/down/left/right)| Move the camera in the corresponding direction |
| `+` / `-` | Zoom in / out |
| `s` / `S` | Decrease / increase sigma by 0.5 |
| `b` / `B` | Decrease / increase beta by 0.1 |
| `r` / `R` | Decrease / increase rho by 1 |
| `a` | Toggle axes |
| `0` | Reset the camera view and the parameters|
| `c` | Change the color of the system, it cycles through|
| Esc | Exit |


The source code is provided in `hw2.c`. This homework was initially easy to setup with the provided examples and template files, `ex7.c` and `lorenz.c`, which made it really easy for me to understand most of the functions and how to interact with the program. Setting up my own interactive commands was just changing the keystrokes to change the parameters (such as Sigma, Beta, Rho) in the code. 
Overall this assignment took me around 2-3 hours to complete. 


### References
https://arxiv.org/pdf/2410.06452v1
https://seti.harvard.edu/unusual_stuff/misc/lorenz.htm
https://en.wikipedia.org/wiki/Lorenz_system