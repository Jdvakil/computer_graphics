/*
 *  Projections
 *
 *  Draw 27 cubes to demonstrate orthogonal & prespective projections
 *
 *  Key bindings:
 *  m          Toggle between perspective and orthogonal
 *  +/-        Changes field of view for perspective
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
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

int th=20;          // Azimuth of overhead view
int ph=30;          // Elevation of overhead view
int fov=55;         // Field of view (for perspective)
int mode=0;         // Projection mode: 0=orthogonal, 1=perspective, 2=first person
double asp=1;      // Aspect ratio
double dim=11;     // Size of world; initially fits the entire pitch
double eyeX=0;     // First-person eye position
double eyeY=1.7;
double eyeZ=8;
int yaw=0;         // First-person heading
int look=-8;       // First-person look elevation
double t=0;        // Time within HW3's six-second passing loop
int lastTime=0;    // Previous GLUT elapsed time, in milliseconds
int paused=0;      // Pause the animation

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.14159265/180))
#define Sin(x) (sin((x)*3.14159265/180))

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
 *  Check for OpenGL errors
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
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
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
   {
      //  Match HW3's portrait-window framing by keeping the shorter side fixed.
      double viewFov=fov;
      if (asp<1) viewFov=360/3.14159265*atan(tan(fov*3.14159265/360)/asp);
      //  A small near plane lets us walk close to the objects.
      gluPerspective(viewFov,asp,0.1,200);
   }
   //  Orthogonal projection, with HW3's aspect-ratio handling
   else if (asp>1)
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, 0.1,200);
   else
      glOrtho(-dim,+dim, -dim/asp,+dim/asp, 0.1,200);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,double r,double g,double b)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3d(1.0*r,1.0*g,1.0*b);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3d(0.75*r,0.75*g,0.75*b);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3d(0.85*r,0.85*g,0.85*b);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3d(0.65*r,0.65*g,0.65*b);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3d(1.1*r,1.1*g,1.1*b);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3d(0.55*r,0.55*g,0.55*b);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
}


/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph,int color)
{
   if (color)
      glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

static void sphere(double x,double y,double z,double r,int color)
{
   glPushMatrix();
   glTranslated(x,y,z);
   glScaled(r,r,r);
   for (int ph=-90;ph<90;ph+=30)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=30)
      {
         Vertex(th,ph,color);
         Vertex(th,ph+30,color);
      }
      glEnd();
   }
   glPopMatrix();
}

static void cylinder(double x,double y,double z,double r,double length,double angle)
{
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(angle,0,0,1);
   glScaled(r,length,r);

   glBegin(GL_QUAD_STRIP);
   for (int a=0;a<=360;a+=15)
   {
      glVertex3d(Cos(a),0,Sin(a));
      glVertex3d(Cos(a),-1,Sin(a));
   }
   glEnd();

   for (int end=0;end<2;end++)
   {
      glBegin(GL_TRIANGLE_FAN);
      glVertex3d(0,-end,0);
      for (int a=0;a<=360;a+=15)
         glVertex3d(Cos(a),-end,Sin(end?a:-a));
      glEnd();
   }
   glPopMatrix();
}


static void shoe(double z,double angle)
{
   glPushMatrix();
   glTranslated(0,1.5,z);
   glRotated(angle,0,0,1);
   cube(0.10,-1.40,0, 0.20,0.10,0.18, 0, 0.18,0.20,0.24);
   glPopMatrix();
}

static void human(double x,double z,double size,double angle,
                  double leftLeg,double rightLeg,int shirt)
{
   glPushMatrix();
   glTranslated(x,0,z);
   glRotated(angle,0,1,0);
   glScaled(size,size,size);

 
   const double colors[3][3]={{0.18,0.42,0.80},{0.80,0.22,0.18},{0.55,0.57,0.60}};
   cube(0,1.9,0, 0.25,0.4,0.35, 0, colors[shirt][0],colors[shirt][1],colors[shirt][2]);
   glColor3f(0.85,0.62,0.43);
   sphere(0,2.6,0,0.3,0);

   glColor3f(0.85,0.62,0.43);
   cylinder(0,2.2,0.48, 0.12,0.8,-leftLeg);
   cylinder(0,2.2,-0.48, 0.12,0.8,leftLeg);
   glColor3f(0.20,0.25,0.34);
   cylinder(0,1.5,0.22, 0.15,1.5,leftLeg);
   cylinder(0,1.5,-0.22, 0.15,1.5,rightLeg);
   shoe(0.22,leftLeg);
   shoe(-0.22,rightLeg);
   glPopMatrix();
}

static void pitch(void)
{
   glColor3f(0.12,0.36,0.20);
   glBegin(GL_QUADS);
   glVertex3d(-7,0,-6);
   glVertex3d(-7,0,6);
   glVertex3d(7,0,6);
   glVertex3d(7,0,-6);
   glEnd();

}


static void cone(double x,double z)
{
   glPushMatrix();
   glTranslated(x,0,z);
   glColor3f(1,0.4,0);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0,0.45,0);
   for (int a=0;a<=360;a+=30)
      glVertex3d(0.2*Cos(a),0,-0.2*Sin(a));
   glEnd();

   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0,0,0);
   for (int a=0;a<=360;a+=30)
      glVertex3d(0.2*Cos(a),0,0.2*Sin(a));
   glEnd();
   glPopMatrix();
}

static void straps(double z)
{
   const double rx[4]={0.38,0.33,0.33,0.38};
   const double ry[4]={0.40,0.35,0.35,0.40};
   const double depth[4]={-0.04,-0.04,0.04,0.04};
   double handle[13][4][3];

   for (int a=0;a<=12;a++)
      for (int j=0;j<4;j++)
      {
         handle[a][j][0]=rx[j]*Cos(15*a);
         handle[a][j][1]=0.45+ry[j]*Sin(15*a);
         handle[a][j][2]=z+depth[j];
      }

   glColor3f(0.8,0.65,0.35);
   glBegin(GL_QUADS);
   for (int a=0;a<12;a++)
      for (int j=0;j<4;j++)
      {
         int k=(j+1)%4;
         glVertex3dv(handle[a][j]);
         glVertex3dv(handle[a+1][j]);
         glVertex3dv(handle[a+1][k]);
         glVertex3dv(handle[a][k]);
      }
   for (int j=3;j>=0;j--) glVertex3dv(handle[0][j]);
   for (int j=0;j<4;j++) glVertex3dv(handle[12][j]);
   glEnd();
}


static void duffel(double x,double z,double size,double angle)
{
   const double profile[9][2]={
      {-0.6,0},{0.6,0},{0.8,0.2},{0.8,0.6},{0.5,0.9},
      {0,1},{-0.5,0.9},{-0.8,0.6},{-0.8,0.2}
   };
   const double along[5]={-0.75,-0.55,0,0.55,0.75};
   const double width[5]={0.65,1,1.05,1,0.65};
   const double height[5]={0.65,0.9,1,0.9,0.65};
   double v[5][9][3];
   for (int i=0;i<5;i++)
      for (int j=0;j<9;j++)
      {
         v[i][j][0]=along[i];
         v[i][j][1]=0.65*height[i]*profile[j][1];
         v[i][j][2]=0.42*width[i]*profile[j][0];
      }

   glPushMatrix();
   glTranslated(x,0,z);
   glRotated(angle,0,1,0);
   glScaled(size,size,size);
   glColor3f(0.55,0.22,0.12);
   glBegin(GL_TRIANGLES);
   for (int i=0;i<4;i++)
      for (int j=0;j<9;j++)
      {
         int k=(j+1)%9;
         glVertex3dv(v[i][j]);
         glVertex3dv(v[i+1][j]);
         glVertex3dv(v[i+1][k]);
         glVertex3dv(v[i][j]);
         glVertex3dv(v[i+1][k]);
         glVertex3dv(v[i][k]);
      }
   glEnd();
   glColor3f(0.42,0.16,0.08);
   for (int end=0;end<2;end++)
   {
      int i=end?4:0;
      glBegin(GL_TRIANGLE_FAN);
      glVertex3d(along[i],0.2,0);
      for (int j=0;j<=9;j++) glVertex3dv(v[i][(end?9-j:j)%9]);
      glEnd();
   }

   straps(-0.20);
   straps(0.20);
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display(void)
{
   double right=0,otherRight=0;
   double ballX;
   double phase=fmod(t,3);
   double travel=(phase-0.25)/2;
   double kick=0;

   if (phase<0.5)
      kick=90*phase;
   else if (phase<1)
      kick=90*(1-phase);
   if (travel<0) travel=0;
   if (travel>1) travel=1;
   if (t<3)
   {
      right=kick;
      ballX=-1.8+3.55*travel;
   }

   else
   {
      otherRight=kick;
      ballX=1.75-3.55*travel;
   }

   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   if (mode==2)
   {
      double dx=Sin(yaw)*Cos(look);
      double dy=Sin(look);
      double dz=-Cos(yaw)*Cos(look);
      gluLookAt(eyeX,eyeY,eyeZ, eyeX+dx,eyeY+dy,eyeZ+dz, 0,1,0);
   }
   else
   {
      double Ex=-2*dim*Sin(th)*Cos(ph);
      double Ey=1.3+2*dim*Sin(ph);
      double Ez=+2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez, 0,1.3,0, 0,Cos(ph),0);
   }

   pitch();
   cone(-3.5,-1.4);
   cone(0.6,-1.6);
   cone(-0.8,1.3);
   cone(3.4,1.5);

   human(-2.7,0,1,0,0,right,0);
   human(2.7,-0.45,1.05,180,0,otherRight,1);
   human(-2,-5,1,-90,0,0,2);
   duffel(-0.7,-2.7,0.9,20);
   sphere(ballX,0.3,-0.22,0.3,1);

   glDisable(GL_DEPTH_TEST);
   glColor3f(1,1,1);
   glWindowPos2i(10,50);
   Print("Projection=%s",
         mode==0 ? "Orthogonal" : mode==1 ? "Perspective" : "First person",
         mode==2 ? yaw : th,mode==2 ? look : ph,dim,fov,
         paused ? "Paused" : "Playing");
   glWindowPos2i(10,30);
   if (mode==2)
      Print("WASD: walk, Arrows: look around, m: switch mode, 0: reset camera, r: restart, Space: Pause/Play ");
   else
      Print("Arrows: change view");
   if (paused)
      Print("Paused! press Space again to continue");
   else
      Print("Playing! press Space again to pause");
   glWindowPos2i(10,10);
   Print("m: mode  Space: pause/play  +/- : zoom  r: restart  0: reset cameras  Esc: quit");

   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  Move on the ground relative to the first-person heading
 */
static void walk(double forward,double sideways)
{
   eyeX+=forward*Sin(yaw)+sideways*Cos(yaw);
   eyeZ-=forward*Cos(yaw)-sideways*Sin(yaw);
   //  Keep navigation near the pitch and inside the viewing depth range.
   if (eyeX>20) eyeX=20;
   if (eyeX< -20) eyeX=-20;
   if (eyeZ>20) eyeZ=20;
   if (eyeZ< -20) eyeZ=-20;
}

/*
 *  GLUT calls this routine when an arrow key is pressed (from ex9)
 */
void special(int key,int x,int y)
{
   (void)x;
   (void)y;
   if (mode==2)
   {
      if (key==GLUT_KEY_RIGHT) yaw+=5;
      else if (key==GLUT_KEY_LEFT) yaw-=5;      
      else if (key==GLUT_KEY_UP) look+=5;
      else if (key==GLUT_KEY_DOWN) look-=5;
      yaw%=360;
      if (look>85) look=85;
      if (look< -85) look=-85;
   }
   else
   {
      //  ex9's overhead angle and zoom controls
      if (key==GLUT_KEY_RIGHT) th+=5;
      else if (key==GLUT_KEY_LEFT) th-=5;
      else if (key==GLUT_KEY_UP) ph+=5;
      else if (key==GLUT_KEY_DOWN) ph-=5;
      th%=360;
      //  Keep an overhead view without flipping the camera at the poles.
      if (ph<5) ph=5;
      if (ph>85) ph=85;
      if (dim<2) dim=2;
      if (dim>40) dim=40;
   }
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed (HW3 and ex9)
 */
void key(unsigned char ch,int x,int y)
{
   (void)x;
   (void)y;
   //  Exit on ESC
   if (ch==27)
      exit(0);
   //  Reset both cameras, keeping the current mode
   else if (ch=='0')
   {
      th=20; ph=30; dim=11; fov=55;
      eyeX=0; eyeY=1.7; eyeZ=8;
      yaw=0; look=-8;
   }
   //  ex9's mode switch, extended to three modes
   else if (ch=='m')
      mode=(mode+1)%3;
   //  ex9's field-of-view controls, checking fov rather than the key value
   else if (ch=='-'  && fov>15)
      fov--;
   else if ((ch=='+') && fov<100)
      fov++;
   //  HW3's animation controls
   else if (ch==' ')
   {
      paused=1-paused;
      lastTime=glutGet(GLUT_ELAPSED_TIME);
   }
   else if (ch=='r' || ch=='R')
   {
      t=0;
      lastTime=glutGet(GLUT_ELAPSED_TIME);
   }
   else if (mode==2)
   {
      if (ch=='w' || ch=='W') walk(0.25,0);
      else if (ch=='s' || ch=='S') walk(-0.25,0);
      else if (ch=='a' || ch=='A') walk(0,-0.25);
      else if (ch=='d' || ch=='D') walk(0,0.25);
   }
   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  GLUT calls this routine when there is nothing else to do
 */
void idle()
{
   int now=glutGet(GLUT_ELAPSED_TIME);
   double dt=(now-lastTime)/1000.0;
   lastTime=now;
   if (!paused) t=fmod(t+dt,6);
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
 //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Request 1000 x 600 pixel window
   glutInitWindowSize(1000,600);
   //  Create the window
   glutCreateWindow("Jay Vakil - HW4");
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
   //  Tell GLUT to call "idle" when the program is idle
   glutIdleFunc(idle);
   lastTime=glutGet(GLUT_ELAPSED_TIME);
   glutMainLoop();
   return 0;
}
