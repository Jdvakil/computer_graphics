/*
 * Football practice, built from ex8's matrix-stack objects and GLUT callbacks.
 * Original example: Willem A. (Vlakkies) Schreuder, CSCI4229/5229.
 * All solid geometry is generated here; no GLUT/GLU solid objects or assets.
 * Arrows / left drag: orbit; +/-: zoom; space: pause; R: replay; 0: view; A: axes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif
#define Cos(x) cos((x)*3.141592653589793/180)
#define Sin(x) sin((x)*3.141592653589793/180)

static double th=25,ph=22,distance=16,asp=1,timeScene=0;
static int axes=0,paused=0,lastTime=0,drag=0,mx,my,width=1100,height=720;
static const double contact=4.22,ballRadius=.32;

void Print(const char* format,...)
{
   char buf[1024];
   va_list args;
   va_start(args,format);
   vsnprintf(buf,sizeof(buf),format,args);
   va_end(args);
   for (char* ch=buf;*ch;ch++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch);
}
void ErrCheck(const char* where)
{
   GLenum err=glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}
static double clamp(double v,double lo,double hi) { return fmax(lo,fmin(hi,v)); }
static double blend(double a,double b,double u)
{
   u=clamp(u,0,1); return a+(b-a)*u*u*(3-2*u);
}

/* Generic unit cube from ex8, now with surface normals and inherited color. */
static void cube(double x,double y,double z,double dx,double dy,double dz,double angle)
{
   static const double v[8][3]={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
                               {-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
   static const int f[6][4]={{4,5,6,7},{1,0,3,2},{5,1,2,6},{0,4,7,3},{7,6,2,3},{0,1,5,4}};
   static const double n[6][3]={{0,0,1},{0,0,-1},{1,0,0},{-1,0,0},{0,1,0},{0,-1,0}};
   glPushMatrix();
   glTranslated(x,y,z); glRotated(angle,0,1,0); glScaled(dx,dy,dz);
   glBegin(GL_QUADS);
   for (int i=0;i<6;i++)
   {
      glNormal3dv(n[i]);
      for (int j=0;j<4;j++) { glTexCoord2d(j==1 || j==2,j>=2); glVertex3dv(v[f[i][j]]); }
   }
   glEnd(); glPopMatrix();
}
/* Ex8 latitude/longitude sphere, with normals and UVs for later texturing. */
static void Vertex(double longitude,double latitude)
{
   double x=Sin(longitude)*Cos(latitude),y=Sin(latitude),z=Cos(longitude)*Cos(latitude);
   glNormal3d(x,y,z); glTexCoord2d(longitude/360,(latitude+90)/180); glVertex3d(x,y,z);
}
static void sphere(double x,double y,double z,double r,int football)
{
   glPushMatrix(); glTranslated(x,y,z); glScaled(r,r,r);
   for (int p=-90;p<90;p+=10)
      for (int t=0;t<360;t+=10)
      {
         if (football)
         {
            /* Alternating stitched-looking panels on the generated sphere. */
            int dark=((t/30+(p+90)/30)%3==0);
            glColor3f(dark?.055:.94,dark?.075:.96,dark?.10:.90);
         }
         glBegin(GL_QUADS);
         Vertex(t,p); Vertex(t,p+10); Vertex(t+10,p+10); Vertex(t+10,p);
         glEnd();
      }
   glPopMatrix();
}
/* Closed cylinder: radius one, runs from y=0 to y=-1. */
static void cylinder(double radius,double length)
{
   glPushMatrix(); glScaled(radius,length,radius);
   glBegin(GL_QUAD_STRIP);
   for (int a=0;a<=360;a+=15)
   {
      glNormal3d(Cos(a),0,Sin(a));
      glTexCoord2d(a/360.,0); glVertex3d(Cos(a),0,Sin(a));
      glTexCoord2d(a/360.,1); glVertex3d(Cos(a),-1,Sin(a));
   }
   glEnd();
   for (int end=0;end<2;end++)
   {
      glNormal3d(0,end?-1:1,0);
      glBegin(GL_TRIANGLE_FAN); glTexCoord2d(.5,.5); glVertex3d(0,-end,0);
      for (int a=0;a<=360;a+=15)
      {
         double z=(end?1:-1)*Sin(a);
         glTexCoord2d(.5+.5*Cos(a),.5+.5*z); glVertex3d(Cos(a),-end,z);
      }
      glEnd();
   }
   glPopMatrix();
}
/* Custom closed cleat: lofted elliptical cross sections from heel to toe.
 * This asymmetric, changing-height mesh is not a scaled primitive.
 * End caps and analytic ring normals keep it solid and ready for texture UVs.
 */
static void cleat(void)
{
   const double x[]={-.19,-.13,.02,.20,.40,.47};
   const double w[]={.07,.145,.15,.145,.11,.025};
   const double h[]={.06,.15,.16,.10,.065,.025};
   const double y[]={.095,.16,.17,.11,.075,.07};
   for (int i=0;i<5;i++)
   {
      glBegin(GL_QUAD_STRIP);
      for (int a=0;a<=360;a+=20)
         for (int j=i;j<=i+1;j++)
         {
            double ny=Cos(a)/h[j],nz=Sin(a)/w[j];
            double nx=-((y[i+1]-y[i])*ny+(h[i+1]-h[i])*Cos(a)*ny+
                        (w[i+1]-w[i])*Sin(a)*nz)/(x[i+1]-x[i]);
            glNormal3d(nx,ny,nz); glTexCoord2d(j/5.,a/360.);
            glVertex3d(x[j],y[j]+h[j]*Cos(a),w[j]*Sin(a));
         }
      glEnd();
   }
   for (int k=0;k<2;k++)
   {
      int j=k?5:0;
      glNormal3d(k?1:-1,0,0); glBegin(GL_TRIANGLE_FAN);
      glVertex3d(x[j],y[j],0);
      for (int a=0;a<=360;a+=20) glVertex3d(x[j],y[j]+h[j]*Cos(a),w[j]*Sin(k?-a:a));
      glEnd();
   }
   glColor3f(.9,.95,.25);
   for (int i=0;i<3;i++) cube(.04+i*.08,.245-i*.03,0,.018,.012,.115,0);
}
static void leg(double z,double hip,double knee)
{
   glPushMatrix(); glTranslated(0,1.82,z); glRotated(hip,0,0,1);
   glColor3f(.035,.075,.15); sphere(0,0,0,.18,0); cylinder(.16,.78);
   glTranslated(0,-.78,0); glRotated(knee,0,0,1);
   glColor3f(.77,.49,.31); sphere(0,0,0,.145,0); cylinder(.115,.78);
   glColor3f(.92,.94,.90);
   glPushMatrix(); glTranslated(0,-.35,0); cylinder(.122,.43); glPopMatrix();
   glTranslated(0,-.88,0); glColor3f(.10,.13,.19); cleat();
   glPopMatrix();
}
static void arm(double z,double swing)
{
   glPushMatrix(); glTranslated(0,2.72,z); glRotated(swing,0,0,1);
   sphere(0,0,0,.19,0); cylinder(.16,.30);
   glTranslated(0,-.30,0); glColor3f(.77,.49,.31); cylinder(.115,.30);
   glTranslated(0,-.30,0); sphere(0,0,0,.12,0); glRotated(18,0,0,1);
   cylinder(.10,.46); sphere(0,-.49,0,.13,0); glPopMatrix();
}
/* Generic articulated human, reused with different position, scale and yaw. */
static void human(double x,double z,double scale,double yaw,int active)
{
   double walk=0,left=0,right=0,knee=0;
   if (active && timeScene<3.6)
   {
      walk=sin(timeScene*2*3.141592653589793/0.9)*26;
      walk*=clamp((3.6-timeScene)/.4,0,1); left=-walk; right=walk;
   }
   else if (active)
   {
      if (timeScene<4) right=blend(0,-28,(timeScene-3.6)/.4);
      else if (timeScene<contact) right=blend(-28,24,(timeScene-4)/.22);
      else if (timeScene<4.48) right=blend(24,65,(timeScene-contact)/.26);
      else right=blend(65,0,(timeScene-4.48)/.85);
      knee=timeScene<4 ? -12*Sin(180*clamp((timeScene-3.6)/.4,0,1)) : 0;
   }
   glPushMatrix(); glTranslated(x,0,z); glRotated(yaw,0,1,0); glScaled(scale,scale,scale);
   leg(.23,left,0); leg(-.23,right,knee);
   glColor3f(.035,.075,.15); cube(0,1.85,0,.22,.20,.40,0);
   glColor3f(active?.12:.95,active?.38:.38,active?.86:.12);
   cube(0,2.35,0,.24,.43,.39,0);
   arm(.48,walk); glColor3f(active?.12:.95,active?.38:.38,active?.86:.12); arm(-.48,-walk);
   glColor3f(.77,.49,.31); sphere(0,2.90,0,.16,0); sphere(0,3.22,0,.31,0);
   sphere(.29,3.21,0,.075,0);
   glColor3f(.04,.045,.055); sphere(.274,3.30,.12,.038,0); sphere(.274,3.30,-.12,.038,0);
   /* Jersey stripe makes facing direction visible. */
   glColor3f(.95,.95,.88); cube(.247,2.4,0,.009,.30,.045,0);
   glPopMatrix();
}
static void ballPosition(double* x,double* y,double* rotation)
{
   double t=fmax(0,timeScene-contact),remain=t,v=3.1,bounce=0;
   *x=.8+3.6*(1-exp(-.65*t))/.65; *y=ballRadius; *rotation=-(*x-.8)/ballRadius*180/3.141592653589793;
   /* Piecewise ballistic arcs: each bounce loses vertical energy. */
   for (int i=0;i<6;i++)
   {
      double flight=2*v/9.81;
      if (remain<=flight) { bounce=v*remain-4.905*remain*remain; break; }
      remain-=flight; v*=.52;
   }
   *y+=fmax(0,bounce);
}
static void field(void)
{
   for (int i=0;i<12;i++)
   {
      glColor3f(.10f,.30f+.025f*(i%2),.17f);
      cube(-5.5+i,-.12,0,.5,.12,3.8,0);
   }
   glColor3f(.83,.90,.79);
   cube(.0,.006,-3.35,5.6,.006,.025,0); cube(.0,.006,3.35,5.6,.006,.025,0);
   cube(-5.6,.006,0,.025,.006,3.35,0); cube(5.6,.006,0,.025,.006,3.35,0);
   cube(0,.006,0,.025,.006,3.35,0);
   glNormal3d(0,1,0); glBegin(GL_QUAD_STRIP);
   for (int a=0;a<=360;a+=5)
   {
      glVertex3d(1.20*Cos(a),.014,1.20*Sin(a)); glVertex3d(1.24*Cos(a),.014,1.24*Sin(a));
   }
   glEnd();
}
void display(void)
{
   double bx,by,roll;
   GLfloat light[]={-3,8,5,1};
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   gluLookAt(distance*Sin(th)*Cos(ph),1+distance*Sin(ph),distance*Cos(th)*Cos(ph),0,1,0,0,1,0);
   glLightfv(GL_LIGHT0,GL_POSITION,light); glEnable(GL_LIGHTING);
   field(); human(-4.5+3.95*clamp(timeScene/3.6,0,1),0,1,0,1);
   human(-3.0,2.2,.78,145,0);
   ballPosition(&bx,&by,&roll);
   glPushMatrix(); glTranslated(bx,by,-.23); glRotated(roll,0,0,1); sphere(0,0,0,ballRadius,1); glPopMatrix();
   glDisable(GL_LIGHTING); glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0,.03,0); glVertex3d(2,.03,0);
      glVertex3d(0,.03,0); glVertex3d(0,4,0);
      glVertex3d(0,.03,0); glVertex3d(0,.03,2); glEnd();
      glRasterPos3d(2,.03,0); Print("X"); glRasterPos3d(0,4,0); Print("Y"); glRasterPos3d(0,.03,2); Print("Z");
   }
   /* Screen-space HUD, independent of camera and depth buffer. */
   glDisable(GL_DEPTH_TEST); glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); glOrtho(0,width,0,height,-1,1);
   glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
   glColor3f(.04,.09,.12); glBegin(GL_QUADS);
   glVertex2i(0,0); glVertex2i(width,0); glVertex2i(width,76); glVertex2i(0,76); glEnd();
   glColor3f(.96,.97,.92); glRasterPos2i(18,50);
   Print("FOOTBALL PRACTICE   |   %s%s",timeScene<3.6?"Approach":timeScene<contact?"Backswing":timeScene<5.33?"Kick!":"Ball in play",paused?"  [paused]":"");
   glRasterPos2i(18,20); Print("Arrows / drag: orbit   +/-: zoom   Space: pause   R: replay   0: reset view   A: axes   Esc: exit");
   glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
   ErrCheck("display"); glutSwapBuffers();
}
void reshape(int w,int h)
{
   width=w>0?w:1; height=h>0?h:1; asp=(double)width/height;
   glViewport(0,0,width,height); glMatrixMode(GL_PROJECTION); glLoadIdentity();
   gluPerspective(48,asp,.1,100); glMatrixMode(GL_MODELVIEW);
}
void special(int key,int x,int y)
{
   (void)x; (void)y;
   if (key==GLUT_KEY_RIGHT) th+=5;
   if (key==GLUT_KEY_LEFT) th-=5;
   if (key==GLUT_KEY_UP) ph+=5;
   if (key==GLUT_KEY_DOWN) ph-=5;
   ph=clamp(ph,5,85); th=fmod(th,360); glutPostRedisplay();
}
void key(unsigned char ch,int x,int y)
{
   (void)x; (void)y;
   if (ch==27) exit(0);
   if (ch=='0') { th=25; ph=22; distance=16; }
   if (ch=='a' || ch=='A') axes=!axes;
   if (ch==' ') { paused=!paused; lastTime=glutGet(GLUT_ELAPSED_TIME); }
   if (ch=='r' || ch=='R') { timeScene=0; lastTime=glutGet(GLUT_ELAPSED_TIME); }
   if (ch=='+' || ch=='=') distance=clamp(distance-1,7,30);
   if (ch=='-' || ch=='_') distance=clamp(distance+1,7,30);
   glutPostRedisplay();
}
static void mouse(int button,int state,int x,int y)
{
   if (button==GLUT_LEFT_BUTTON) { drag=state==GLUT_DOWN; mx=x; my=y; }
   if (state==GLUT_DOWN && (button==3 || button==4))
      distance=clamp(distance+(button==3?-1:1),7,30);
   glutPostRedisplay();
}
static void motion(int x,int y)
{
   if (drag) { th=fmod(th+(x-mx)*.4,360); ph=clamp(ph+(y-my)*.4,5,85); mx=x; my=y; glutPostRedisplay(); }
}
static void tick(int value)
{
   int now=glutGet(GLUT_ELAPSED_TIME);
   (void)value;
   if (!paused) timeScene=fmod(timeScene+(now-lastTime)/1000.,10);
   lastTime=now; glutPostRedisplay(); glutTimerFunc(16,tick,0);
}
int main(int argc,char* argv[])
{
   glutInit(&argc,argv); glutInitWindowSize(width,height);
   glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE); glutCreateWindow("Ex8 - Football Practice");
#ifdef USEGLEW
   if (glewInit()!=GLEW_OK) { fprintf(stderr,"Error initializing GLEW\n"); return 1; }
#endif
   glClearColor(.49,.69,.80,1); glEnable(GL_NORMALIZE); glEnable(GL_COLOR_MATERIAL);
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE); glEnable(GL_LIGHT0);
   { const GLfloat ambient[]={.35,.35,.35,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient); }
   glutDisplayFunc(display); glutReshapeFunc(reshape); glutSpecialFunc(special); glutKeyboardFunc(key);
   glutMouseFunc(mouse); glutMotionFunc(motion);
   lastTime=glutGet(GLUT_ELAPSED_TIME); glutTimerFunc(16,tick,0);
   glutMainLoop(); return 0;
}
