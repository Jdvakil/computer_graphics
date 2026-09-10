/*
 * 
 * Simple program to demonstrate generating coordinates
 * using the Lorenz Attractor
 *
 *  Key bindings:
 *  s/S    Decrease/increase s (sigma)
 *  b/B    Decrease/increase b (beta)
 *  r/R    Decrease/increase r (rho)
 *  arrows Change view angle
 *  +/-    Zoom in/out
 *  a      Toggle axes
 *  c      Cycle color scheme
 *  0      Reset everything (view, zoom, parameters, axes)
 *  ESC    Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif

int th=-20;      //  Azimuth of view angle
int ph=20;       //  Elevation of view angle
int axes=1;      //  Display axes
int color=0;     //  Color scheme
const char* text[] = {"Grey-Gold","Blue-Orange","Blue-Red","White"};
double dim=50;

/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;

#define N 50000
double pt[N+1][3];

void lorenz_attractor()
{
   int i;
   /*  Coordinates  */
   double x = 1;
   double y = 1;
   double z = 1;
   /*  Time step  */
   double dt = 0.001;

   pt[0][0]=x; pt[0][1]=y; pt[0][2]=z;
   /*printf("%5d %8.3f %8.3f %8.3f\n",0,x,y,z);*/
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (i=0;i<N;i++)
   {
      double dx = s*(y-x);
      double dy = x*(r-z)-y;
      double dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;
      pt[i+1][0]=x; pt[i+1][1]=y; pt[i+1][2]=z;
      /*printf("%5d %8.3f %8.3f %8.3f\n",i+1,x,y,z);*/
   }
}

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Print message to stderr and exit
 */
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

/*
 *  Check for OpenGL errors
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   int i;
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Set view angle
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);
   //  Lorenz z is up, OpenGL y is up
   glRotatef(-90,1,0,0);
   //  The attractor sits around z=25, move it to the middle
   glTranslated(0,0,-25);

   //  Draw the attractor as one long line, colored by the current scheme
   glBegin(GL_LINE_STRIP);
   for (i=0;i<=N;i++)
   {
      double t = pt[i][2]/50;   //  0 at z=0, 1 at z=50
      if (color==0)             //  Grey at the bottom to gold at the top
         glColor3d(0.5+0.5*t, 0.5+0.34*t, 0.5-0.5*t);
      else if (color==1)        //  Blue at the bottom to orange at the top
         glColor3d(t, 0.6, 1-t);
      else if (color==2)        //  Blue at the start to red at the end (time)
         glColor3d((double)i/N, 0.2, 1-(double)i/N);
      else                      //  Plain white
         glColor3f(1,1,1);
      glVertex3dv(pt[i]);
   }
   glEnd();

   //  Draw axes
   if (axes)
   {
      const double len=25.0;  //  Length of axes
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

   //  Current parameters, five pixels from the lower left corner
   glColor3f(1,1,1);
   glWindowPos2i(5,5);
   Print("s=%.1f  b=%.4f  r=%.1f   Angle=%d,%d   Dim=%.0f   Color=%s",s,b,r,th,ph,dim,text[color]);
   //  How to use it, top left corner
   glWindowPos2i(5,glutGet(GLUT_WINDOW_HEIGHT)-25);
   Print("Lorenz Attractor  dx/dt=s(y-x)  dy/dt=x(r-z)-y  dz/dt=xy-bz");
   glWindowPos2i(5,glutGet(GLUT_WINDOW_HEIGHT)-50);
   Print("Arrows: rotate view   +/-: zoom   0: reset all   a: axes   c: color   ESC: exit");
   glWindowPos2i(5,glutGet(GLUT_WINDOW_HEIGHT)-75);
   Print("s/S: sigma -/+   b/B: beta -/+   r/R: rho -/+");
   //  Render the scene
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   double asp = (height>0) ? (double)width/height : 1;
   // zooming in does not clip the depth
   if (asp>1)
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -600,+600);
   else
      glOrtho(-dim,+dim, -dim/asp,+dim/asp, -600,+600);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle and reset the parameters, i was too lazy to add two different resets
   else if (ch == '0')
   {
      th = -20; ph = 20; dim = 50;
      s = 10; b = 2.6666; r = 28;
      axes = 1; color = 0;
   }
   //  Zoom in and out by changing the size of the world
   else if (ch == '+')
      dim -= 5;
   else if (ch == '-')
      dim += 5;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Cycle color scheme
   else if (ch == 'c' || ch == 'C')
      color = (color+1)%4;
   //  Change Lorenz parameters
   else if (ch == 's') s -= 0.5;
   else if (ch == 'S') s += 0.5;
   else if (ch == 'b') b -= 0.1;
   else if (ch == 'B') b += 0.1;
   else if (ch == 'r') r -= 1;
   else if (ch == 'R') r += 1;
   //  Negative s or b makes the integration blow up, so stop at zero
   if (s < 0) s = 0;
   if (b < 0) b = 0;
   if (dim < 5) dim = 5;
   //  Recompute the points with the new parameters
   lorenz_attractor();
   //  Redo the projection in case the zoom changed
   reshape(glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT));
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Compute the attractor
   lorenz_attractor();
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(600,600);
   //  Create the window
   glutCreateWindow("Jay Vakil: CSCI 5229 Homework 2 Lorenz Attractor");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
