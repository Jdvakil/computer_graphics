/*
 * Homework 4: HW3's passing practice scene with three camera modes.
 * Based on ex9's projections/camera controls and ex10's polygon offset.
 * No CSCIx229, GLU, GLUT, or imported objects are used.
 *
 * m       Cycle orthogonal / perspective / first person
 * arrows  Orbit overhead; in first person, walk (up/down) and turn (left/right)
 * W/S     Walk forward/backward in first person
 * A/D     Strafe left/right in first person
 * PgUp/Dn Look up/down in first person
 * +/-     Zoom overhead views
 * 0       Reset cameras (keep current mode)
 * space   Pause/resume animation
 * r       Restart animation
 * Esc     Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef __APPLE__
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <GLFW/glfw3.h>

#define Cos(x) (cos((x)*3.14159265/180))
#define Sin(x) (sin((x)*3.14159265/180))

enum { ORTHOGONAL, PERSPECTIVE, FIRST_PERSON };
int mode=ORTHOGONAL;
int th=20;          // Shared overhead azimuth
int ph=30;          // Shared overhead elevation
const double fov=55;
double asp=1;
double dim=11;      // Half-size of the shorter side: fits the entire pitch
const double eyeHeight=1.7;
double eyeX=0,eyeZ=8;
int yaw=0,look=-8;  // First-person heading and pitch, in degrees
double t=0;        // Time within HW3's six-second passing loop
int paused=0;

/* ex9's projection setup, using glFrustum instead of gluPerspective. */
static void Project(void)
{
   double halfWidth=dim*(asp>1 ? asp : 1);
   double halfHeight=dim*(asp<1 ? 1/asp : 1);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (mode==ORTHOGONAL)
      glOrtho(-halfWidth,halfWidth,-halfHeight,halfHeight,0.1,200);
   else
   {
      const double near=0.1;
      double top=near*tan(fov*3.14159265/360);
      // Keep the entire scene framed even in a portrait-shaped window.
      if (asp<1) top/=asp;
      glFrustum(-top*asp,top*asp,-top,top,near,200);
   }
   glMatrixMode(GL_MODELVIEW);
}

/* Both overhead projections use exactly the same eye and target.
 * Inverse camera transforms replace ex9's gluLookAt. The eye is
 * (-distance*Sin(th)*Cos(ph), 1.3+distance*Sin(ph),
 *   distance*Cos(th)*Cos(ph)), looking at (0,1.3,0).
 */
static void View(void)
{
   glLoadIdentity();
   if (mode==FIRST_PERSON)
   {
      glRotated(-look,1,0,0);
      glRotated(yaw,0,1,0);
      glTranslated(-eyeX,-eyeHeight,-eyeZ);
   }
   else
   {
      double distance=dim/tan(fov*3.14159265/360);
      glTranslated(0,0,-distance);
      glRotated(ph,1,0,0);
      glRotated(th,0,1,0);
      glTranslated(0,-1.3,0);
   }
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
   // ex10: push the ground slightly back in depth without moving its vertices.
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   glColor3f(0.12,0.36,0.20);
   glBegin(GL_QUADS);
   glVertex3d(-7,0,-6);
   glVertex3d(-7,0,6);
   glVertex3d(7,0,6);
   glVertex3d(7,0,-6);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);

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

/* HW3's animation and scene, with only the camera setup changed. */
static void display(void)
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
   View();

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

   GLenum err=glGetError();
   if (err!=GL_NO_ERROR)
      fprintf(stderr,"OpenGL error 0x%04x [display]\n",(unsigned int)err);
}

/* GLFW uses the framebuffer size so resizing also works on HiDPI screens. */
static void reshape(GLFWwindow* window,int width,int height)
{
   (void)window;
   if (width<1) width=1;
   if (height<1) height=1;
   asp=(double)width/height;
   glViewport(0,0,width,height);
   Project();
}

static void title(GLFWwindow* window)
{
   const char* names[]={"Orthogonal", "Perspective", "First person"};
   char text[512];
   snprintf(text,sizeof(text),
      "Jay Vakil - HW4 | %s | m: mode | %s | 0: camera | Space: %s | r: restart | Esc: quit",
      names[mode],mode==FIRST_PERSON ? "Arrows: walk/turn, A/D: strafe, PgUp/Dn: look" :
      "Arrows: orbit, +/-: zoom",paused ? "play" : "pause");
   glfwSetWindowTitle(window,text);
}

static void resetView(void)
{
   th=20;
   ph=30;
   dim=11;
   eyeX=0;
   eyeZ=8;
   yaw=0;
   look=-8;
}

static void key(GLFWwindow* window,int code,int scancode,int action,int mods)
{
   (void)scancode;
   (void)mods;
   if (action!=GLFW_PRESS && action!=GLFW_REPEAT) return;

   // Toggles fire once per press, not on keyboard auto-repeat.
   if (action==GLFW_PRESS)
   {
      if (code==GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window,GLFW_TRUE);
      else if (code==GLFW_KEY_M) mode=(mode+1)%3;
      else if (code==GLFW_KEY_0) resetView();
      else if (code==GLFW_KEY_SPACE) paused=1-paused;
      else if (code==GLFW_KEY_R) t=0;
   }

   if (mode==FIRST_PERSON)
   {
      const double step=0.25;
      if (code==GLFW_KEY_LEFT) yaw-=5;
      else if (code==GLFW_KEY_RIGHT) yaw+=5;
      else if (code==GLFW_KEY_W)
      {
         eyeX+=step*Sin(yaw);
         eyeZ-=step*Cos(yaw);
      }
      else if (code==GLFW_KEY_S)
      {
         eyeX-=step*Sin(yaw);
         eyeZ+=step*Cos(yaw);
      }
      else if (code==GLFW_KEY_A)
      {
         eyeX-=step*Cos(yaw);
         eyeZ-=step*Sin(yaw);
      }
      else if (code==GLFW_KEY_D)
      {
         eyeX+=step*Cos(yaw);
         eyeZ+=step*Sin(yaw);
      }
      else if (code==GLFW_KEY_UP) look+=5;
      else if (code==GLFW_KEY_DOWN) look-=5;
      yaw%=360;
      if (look>85) look=85;
      if (look< -85) look=-85;
      // Keep walking near the pitch, within the projection's depth range.
      if (eyeX>20) eyeX=20;
      if (eyeX< -20) eyeX=-20;
      if (eyeZ>20) eyeZ=20;
      if (eyeZ< -20) eyeZ=-20;
   }
   else
   {
      if (code==GLFW_KEY_RIGHT) th+=5;
      else if (code==GLFW_KEY_LEFT) th-=5;
      else if (code==GLFW_KEY_UP) ph+=5;
      else if (code==GLFW_KEY_DOWN) ph-=5;
      else if (code==GLFW_KEY_EQUAL || code==GLFW_KEY_KP_ADD) dim-=0.5;
      else if (code==GLFW_KEY_MINUS || code==GLFW_KEY_KP_SUBTRACT) dim+=0.5;
      th%=360;
      // Retain a slanted overhead view; avoid flipping over the poles.
      if (ph<5) ph=5;
      if (ph>85) ph=85;
      if (dim<2) dim=2;
      if (dim>40) dim=40;
   }
   Project();
   title(window);
}

static void error(int code,const char* description)
{
   fprintf(stderr,"GLFW error %d: %s\n",code,description);
}

int main(void)
{
   GLFWwindow* window;
   int width,height;
   glfwSetErrorCallback(error);
   if (!glfwInit()) return EXIT_FAILURE;
   // HW3 uses immediate mode, so request a legacy OpenGL context.
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
   glfwWindowHint(GLFW_DEPTH_BITS,24);
   window=glfwCreateWindow(1000,600,"Jay Vakil - HW4",NULL,NULL);
   if (!window)
   {
      glfwTerminate();
      return EXIT_FAILURE;
   }
   glfwMakeContextCurrent(window);
   glfwSwapInterval(1);
   glfwSetFramebufferSizeCallback(window,reshape);
   glfwSetKeyCallback(window,key);
   glfwGetFramebufferSize(window,&width,&height);
   reshape(window,width,height);
   title(window);
   puts("m: cycle orthogonal / perspective / first person");
   puts("Overhead: arrows orbit, +/- zoom. First person: arrows walk/turn,");
   puts("W/S walk, A/D strafe, PageUp/PageDown look up/down.");
   puts("0: reset cameras; Space: pause/play; r: restart animation; Esc: exit.");

   double lastTime=glfwGetTime();
   while (!glfwWindowShouldClose(window))
   {
      double now=glfwGetTime();
      if (!paused) t=fmod(t+now-lastTime,6);
      lastTime=now;
      display();
      glfwSwapBuffers(window);
      glfwPollEvents();
   }
   glfwDestroyWindow(window);
   glfwTerminate();
   return EXIT_SUCCESS;
}
