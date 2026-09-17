/*
 * HW3 - Two players passing a ball back and forth.
 * Based on ex8 by Willem A. (Vlakkies) Schreuder.
 * Arrows: view. +/-: zoom. Space: pause.
 * R: restart. 0: reset view. Esc: exit.
 */
#include <stdio.h>
#include <stdlib.h>
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


int th=20;
int ph=20;
double t=0;
int lastTime=0;
int paused=0;
int windowWidth=1000,windowHeight=750;
double dim=3.8;
void reshape(int width,int height);

#define Cos(x) (cos((x)*3.14159265/180))
#define Sin(x) (sin((x)*3.14159265/180))

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
   glColor3d(.75*r,.75*g,.75*b);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3d(.85*r,.85*g,.85*b);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3d(.65*r,.65*g,.65*b);
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
   glColor3d(.55*r,.55*g,.55*b);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
}


/* Same sphere idea as ex8, with larger steps for a coarse shape. */
static void Vertex(double th,double ph)
{
   glVertex3d(Sin(th)*Cos(ph),Sin(ph),Cos(th)*Cos(ph));
}

static void sphere(double x,double y,double z,double r)
{
   glPushMatrix();
   glTranslated(x,y,z);
   glScaled(r,r,r);
   for (int ph=-90;ph<90;ph+=30)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=30)
      {
         Vertex(th,ph);
         Vertex(th,ph+30);
      }
      glEnd();
   }
   glPopMatrix();
}

/* A cylinder hanging down from (x,y,z). Angle swings it forward. */
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

   // Close the top and bottom.
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

/* A small cube at the end of each leg, using the same hip rotation. */
static void shoe(double z,double angle)
{
   glPushMatrix();
   glTranslated(0,1.5,z);
   glRotated(angle,0,0,1);
   cube(.10,-1.40,0, .20,.10,.18, 0, .18,.20,.24);
   glPopMatrix();
}

/* Swing forward and back over one second. */
static double kickAngle(double time)
{
   if (time<0 || time>=1) return 0;
   return time<.5 ? 90*time : 90*(1-time);
}

/* Generic human reused with translation, rotation, and scaling. */
static void human(double x,double z,double size,double angle,
                  double leftLeg,double rightLeg,int shirt)
{
   glPushMatrix();
   glTranslated(x,0,z);
   glRotated(angle,0,1,0);
   glScaled(size,size,size);

   // Simple shirt colors distinguish the players and viewer.
   const double colors[3][3]={{.18,.42,.80},{.80,.22,.18},{.55,.57,.60}};
   cube(0,1.9,0, .25,.4,.35, 0, colors[shirt][0],colors[shirt][1],colors[shirt][2]);
   glColor3f(.85,.62,.43);
   sphere(0,2.6,0,.3);

   // Two arms and two legs, with the shoes following the leg rotations.
   glColor3f(.85,.62,.43);
   cylinder(0,2.2,.48, .12,.8,-leftLeg);
   cylinder(0,2.2,-.48, .12,.8,leftLeg);
   glColor3f(.20,.25,.34);
   cylinder(0,1.5,.22, .15,1.5,leftLeg);
   cylinder(0,1.5,-.22, .15,1.5,rightLeg);
   shoe(.22,leftLeg);
   shoe(-.22,rightLeg);
   glPopMatrix();
}

/* A plain flat rectangle for the pitch. */
static void pitch(void)
{
   glColor3f(.12,.36,.20);
   glBegin(GL_QUADS);
   glVertex3d(-4.3,-.02,-2);
   glVertex3d(-4.3,-.02,2);
   glVertex3d(4.3,-.02,2);
   glVertex3d(4.3,-.02,-2);
   glEnd();

}

/* Small orange cone, made from triangles and a circular bottom. */
static void cone(double x,double z)
{
   glPushMatrix();
   glTranslated(x,0,z);
   glColor3f(1,.4,0);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0,.45,0);
   for (int a=0;a<=360;a+=30)
      glVertex3d(.2*Cos(a),0,-.2*Sin(a));
   glEnd();

   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0,0,0);
   for (int a=0;a<=360;a+=30)
      glVertex3d(.2*Cos(a),0,.2*Sin(a));
   glEnd();
   glPopMatrix();
}

/* Original sports bag: five changing cross sections form a flat-bottomed,
 * bulging body with tapered ends. This is a custom mesh, not a scaled sphere
 * or cube. Both end caps are closed, and the handles have solid thickness. */
static void sportsBag(double x,double z,double size,double angle)
{
   const double profile[9][2]={
      {-.6,0},{.6,0},{.8,.2},{.8,.6},{.5,.9},
      {0,1},{-.5,.9},{-.8,.6},{-.8,.2}
   };
   const double along[5]={-.75,-.55,0,.55,.75};
   const double width[5]={.65,1,1.05,1,.65};
   const double height[5]={.65,.9,1,.9,.65};
   double v[5][9][3];
   for (int i=0;i<5;i++)
      for (int j=0;j<9;j++)
      {
         v[i][j][0]=along[i];
         v[i][j][1]=.65*height[i]*profile[j][1];
         v[i][j][2]=.42*width[i]*profile[j][0];
      }

   glPushMatrix();
   glTranslated(x,0,z);
   glRotated(angle,0,1,0);
   glScaled(size,size,size);
   glColor3f(.55,.22,.12);
   glBegin(GL_TRIANGLES);
   for (int i=0;i<4;i++)
      for (int j=0;j<9;j++)
      {
         int k=(j+1)%9;
         glVertex3dv(v[i][j]); glVertex3dv(v[i+1][j]); glVertex3dv(v[i+1][k]);
         glVertex3dv(v[i][j]); glVertex3dv(v[i+1][k]); glVertex3dv(v[i][k]);
      }
   glEnd();
   glColor3f(.42,.16,.08);
   for (int end=0;end<2;end++)
   {
      int i=end?4:0;
      glBegin(GL_TRIANGLE_FAN);
      glVertex3d(along[i],.2,0);
      for (int j=0;j<=9;j++) glVertex3dv(v[i][(end?9-j:j)%9]);
      glEnd();
   }

   // Two arched straps. Each has inner, outer, front, and back faces.
   glColor3f(.8,.65,.35);
   for (int side=-1;side<=1;side+=2)
   {
      double h[13][4][3];
      for (int a=0;a<=12;a++)
         for (int corner=0;corner<4;corner++)
         {
            double inset=(corner==1 || corner==2)?.055:0;
            h[a][corner][0]=(.38-inset)*Cos(15*a);
            h[a][corner][1]=.45+(.4-inset)*Sin(15*a);
            h[a][corner][2]=side*.20+(corner>=2?.035:-.035);
         }
      glBegin(GL_QUADS);
      for (int a=0;a<12;a++)
         for (int j=0;j<4;j++)
         {
            int k=(j+1)%4;
            glVertex3dv(h[a][j]); glVertex3dv(h[a+1][j]);
            glVertex3dv(h[a+1][k]); glVertex3dv(h[a][k]);
         }
      for (int j=3;j>=0;j--) glVertex3dv(h[0][j]);
      for (int j=0;j<4;j++) glVertex3dv(h[12][j]);
      glEnd();
   }
   glPopMatrix();
}

void display(void)
{
   double right=0,otherRight=0;
   double ballX;
   double phase=fmod(t,3);
   double travel=fmax(0,fmin(1,(phase-.25)/2));

   // Each player gets three seconds: kick, pass, and receive.
   // Ball endpoints are just ahead of the two kicking feet.
   if (t<3)
   {
      right=kickAngle(phase);
      ballX=-1.8+3.55*travel;
   }
   else
   {
      otherRight=kickAngle(phase);
      ballX=1.75-3.55*travel;
   }

   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);
   glTranslated(0,-1.3,0);

   pitch();
   cone(-3.5,-1.4);
   cone(.6,-1.6);
   cone(-.8,1.3);
   cone(3.4,1.5);

   human(-2.7,0,1,0,0,right,0);       // First passer
   human(2.7,-.451,1.05,180,0,otherRight,1); // Partner facing the first player
   human(-2,-2.8,.8,-90,0,0,2);     // Viewer beside the pitch
   sportsBag(-.7,-2.7,.9,20);    // Custom object beside the viewer
   glColor3f(1,1,1);
   sphere(ballX,.3,-.22,.3);

   // Simple instructions, like the text in ex8.
   glDisable(GL_DEPTH_TEST);
   glColor3f(1,1,1);
   glWindowPos2i(10,30);
   const char* help="Arrows: view  +/-: zoom  Space: pause  R: restart  0: view";
   for (const char* c=help;*c;c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*c);
   glWindowPos2i(10,10);
   help=paused ? "Paused - press Space to continue" :
      "Passing practice - blue and red take turns.";
   for (const char* c=help;*c;c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*c);

   {
      GLenum err=glGetError();
      if (err) fprintf(stderr,"OpenGL error: %s\n",gluErrorString(err));
   }
   glFlush();
   glutSwapBuffers();
}

void special(int key,int x,int y)
{
   (void)x;
   (void)y;
   if (key==GLUT_KEY_RIGHT) th+=5;
   else if (key==GLUT_KEY_LEFT) th-=5;
   else if (key==GLUT_KEY_UP) ph+=5;
   else if (key==GLUT_KEY_DOWN) ph-=5;
   th%=360;
   ph%=360;
   glutPostRedisplay();
}

/* Zoom changes the orthographic view, not the objects. */
static void zoom(double amount)
{
   dim=fmax(2,fmin(8,dim+amount));
   reshape(windowWidth,windowHeight);
   glutPostRedisplay();
}

static void restart(void)
{
   t=0;
   lastTime=glutGet(GLUT_ELAPSED_TIME);
}

void key(unsigned char ch,int x,int y)
{
   (void)x;
   (void)y;
   if (ch==27) exit(0);
   else if (ch=='0') { th=20; ph=20; dim=3.8; zoom(0); }
   else if (ch=='+' || ch=='=') zoom(-.3);
   else if (ch=='-' || ch=='_') zoom(.3);
   else if (ch==' ') { paused=!paused; lastTime=glutGet(GLUT_ELAPSED_TIME); }
   else if (ch=='r' || ch=='R') restart();
   glutPostRedisplay();
}

void reshape(int width,int height)
{
   windowWidth=width>0?width:1;
   windowHeight=height>0?height:1;
   double asp=(double)windowWidth/windowHeight;
   glViewport(0,0,windowWidth,windowHeight);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   // Keep the whole pitch visible when the window is narrow.
   if (asp<1)
      glOrtho(-dim,dim,-dim/asp,dim/asp,-10,10);
   else
      glOrtho(-asp*dim,asp*dim,-dim,dim,-10,10);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void idle(void)
{
   int now=glutGet(GLUT_ELAPSED_TIME);
   double dt=(now-lastTime)/1000.0;
   lastTime=now;
   if (!paused) t=fmod(t+dt,6);
   glutPostRedisplay();
}

int main(int argc,char* argv[])
{
   glutInit(&argc,argv);
   glutInitWindowSize(windowWidth,windowHeight);
   glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
   glutCreateWindow("HW3 - Passing Practice");
#ifdef USEGLEW
   if (glewInit()!=GLEW_OK)
   {
      fprintf(stderr,"Error initializing GLEW\n");
      return 1;
   }
#endif
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   lastTime=glutGet(GLUT_ELAPSED_TIME);
   glutIdleFunc(idle);
   glutMainLoop();
   return 0;
}
