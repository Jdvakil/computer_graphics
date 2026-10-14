/*
 *  HW6 Jay Vakil - textures added to HW5 
 *
 *  
 *  Key bindings:
 *  m          Cycle orthogonal, perspective and first person
 *  +/-        Zoom in/out in all three modes
 *  arrows     Change view angle
 *  WASD       Walk in first-person mode
 *  0          Reset view angle
 *  r          Restart the animation
 *  t          Toggle textures (lighting and original HW5 colors stay active)
 *  l          Toggle lighting
 *  u          Toggle the sun and sunlight
 *  p          Pause/resume the light orbit
 *  < / >      Move light around the scene and stop its automatic orbit
 *  [ / ]      Lower/raise light
 *  b/B        Decrease/increase ambient light
 *  v/V        Decrease/increase diffuse light
 *  c/C        Decrease/increase specular light
 *  n/N        Decrease/increase shininess
 *  F1/F2/F3   Toggle smooth shading / local viewer / light radius
 *  o/O        Next/previous object inspection view; cycle back to the scene
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
 double asp=1;       // Aspect ratio
 double dim=11;      // Size of world; initially fits the entire pitch
 double eyeX=0;      // First-person eye position
 double eyeY=1.7;
 double eyeZ=8;
 int yaw=0;          // First-person heading
 int look=-8;        // First-person look elevation
 double t=0;         // Time within HW3's six-second passing loop
 int lastTime=0;     // Previous GLUT elapsed time, in milliseconds
 int paused=0;       // Pause the animation
 // Light values
 int light=1;        // Lighting on/off
 int sunlight=1;     // Sun and directional sunlight on/off
 const float sunPosition[]={-6,6,-8,0}; // w=0: parallel rays from the sun
 int move=1;         // Move the light independently of the passing animation
 int distance=6;     // Light orbit radius
 int smooth=1;       // Smooth/flat shading
 int local=1;        // Local viewer for specular calculations
 int ambient=20;
 int diffuse=80;
 int specular=30;
 int shininess=4;    // Shininess power of two
 float shiny=16;
 double zh=90;       // Light azimuth (fractional degrees for elapsed-time updates)
 float ylight=4;     // Light elevation
 int obj=0;          //  Scene/opbject selection

 //  Texture names and enable switch follow ex14/ex15.
 enum { GRAIN, CHECKER, BALL, SKIN, GRASS, SUN, LEATHER, NTEX };
 unsigned int texture[NTEX];
 int textured=1;
 
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
  *  BMP loader copied from exercises/ex14/loadtexbmp.c.
  *  Willem A. (Vlakkies) Schreuder, CSCIx229.
  *  Uses the existing local Fatal/ErrCheck instead of including CSCIx229.h.
  */
//
//  Reverse n bytes
//
static void Reverse(void* x,const int n)
{
   char* ch = (char*)x;
   for (int k=0;k<n/2;k++)
   {
      char tmp = ch[k];
      ch[k] = ch[n-1-k];
      ch[n-1-k] = tmp;
   }
}

//
//  Load texture from BMP file
//
unsigned int LoadTexBMP(const char* file)
{
   //  Open file
   FILE* f = fopen(file,"rb");
   if (!f) Fatal("Cannot open file %s\n",file);
   //  Check image magic
   unsigned short magic;
   if (fread(&magic,2,1,f)!=1) Fatal("Cannot read magic from %s\n",file);
   if (magic!=0x4D42 && magic!=0x424D) Fatal("Image magic not BMP in %s\n",file);
   //  Read header
   unsigned int dx,dy,off,k; // Image dimensions, offset and compression
   unsigned short nbp,bpp;   // Planes and bits per pixel
   if (fseek(f,8,SEEK_CUR) || fread(&off,4,1,f)!=1 ||
       fseek(f,4,SEEK_CUR) || fread(&dx,4,1,f)!=1 || fread(&dy,4,1,f)!=1 ||
       fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1 || fread(&k,4,1,f)!=1)
     Fatal("Cannot read header from %s\n",file);
   //  Reverse bytes on big endian hardware (detected by backwards magic)
   if (magic==0x424D)
   {
      Reverse(&off,4);
      Reverse(&dx,4);
      Reverse(&dy,4);
      Reverse(&nbp,2);
      Reverse(&bpp,2);
      Reverse(&k,4);
   }
   //  Check image parameters
   unsigned int max;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,(int*)&max);
   if (dx<1 || dx>max) Fatal("%s image width %d out of range 1-%d\n",file,dx,max);
   if (dy<1 || dy>max) Fatal("%s image height %d out of range 1-%d\n",file,dy,max);
   if (nbp!=1)  Fatal("%s bit planes is not 1: %d\n",file,nbp);
   if (bpp!=24) Fatal("%s bits per pixel is not 24: %d\n",file,bpp);
   if (k!=0)    Fatal("%s compressed files not supported\n",file);
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k=1;k<dx;k*=2);
   if (k!=dx) Fatal("%s image width not a power of two: %d\n",file,dx);
   for (k=1;k<dy;k*=2);
   if (k!=dy) Fatal("%s image height not a power of two: %d\n",file,dy);
#endif

   //  Allocate image memory
   unsigned int size = 3*dx*dy;
   unsigned char* image = (unsigned char*) malloc(size);
   if (!image) Fatal("Cannot allocate %d bytes of memory for image %s\n",size,file);
   //  Seek to and read image
   if (fseek(f,off,SEEK_SET) || fread(image,size,1,f)!=1) Fatal("Error reading data from image %s\n",file);
   fclose(f);
   //  Reverse colors (BGR -> RGB)
   for (k=0;k<size;k+=3)
   {
      unsigned char temp = image[k];
      image[k]   = image[k+2];
      image[k+2] = temp;
   }

   //  Sanity check
   ErrCheck("LoadTexBMP");
   //  Generate 2D texture
   unsigned int texture;
   glGenTextures(1,&texture);
   glBindTexture(GL_TEXTURE_2D,texture);
   //  Copy image
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
   if (glGetError()) Fatal("Error in glTexImage2D %s %dx%d\n",file,dx,dy);
   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //  Free image memory
   free(image);
   //  Return texture name
   return texture;
}

 /*
  *  ex14/ex15: bind a texture and modulate it with the lit object color.
  *  tex=-1 is reserved for the untextured moving light marker.
  */
 static void Texture(int tex)
 {
    if (textured && tex>=0)
    {
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glBindTexture(GL_TEXTURE_2D,texture[tex]);
    }
    else
       glDisable(GL_TEXTURE_2D);
 }

 /*
  *  Select material regions with coordinates; all exercise BMPs stay unchanged.
  *  Grain: a plain region inside ex14's crate, excluding its border/brace.
  *  Skin: a small forehead region of ex15's img5, excluding facial features.
  *  Sun: the central surface region, excluding the image's checkerboard backdrop.
  */
 static void MaterialCoord(int tex,double s,double t)
 {
    if (tex==GRAIN)
       glTexCoord2d((100+30*s)/256.0,(166+60*t)/256.0);
    else if (tex==SKIN)
       glTexCoord2d((45+15*s)/128.0,(83+20*t)/128.0);
    else if (tex==SUN)
       glTexCoord2d(0.25+0.5*s,0.25+0.5*t);
    else
       glTexCoord2d(s,t);
 }

 /* Load the BMP textures once, after the GLUT context exists. */
 static void LoadTextures(void)
 {
    texture[GRAIN]=LoadTexBMP("crate.bmp");
    texture[CHECKER]=LoadTexBMP("block.bmp");
    texture[BALL]=LoadTexBMP("ball.bmp");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    texture[SKIN]=LoadTexBMP("img5.bmp");
    texture[GRASS]=LoadTexBMP("grass.bmp");
    texture[SUN]=LoadTexBMP("sun.bmp");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    texture[LEATHER]=LoadTexBMP("leather.bmp");
    //  Mirror adjacent tiles so the photograph's opposite edges never meet.
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
    glBindTexture(GL_TEXTURE_2D,0);
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
  *  Face normal from two edges, adapted from ex13's triangle().
  */
 static void Normal(const double A[3],const double B[3],const double C[3])
 {
    double dx0=B[0]-A[0];
    double dy0=B[1]-A[1];
    double dz0=B[2]-A[2];
    double dx1=C[0]-A[0];
    double dy1=C[1]-A[1];
    double dz1=C[2]-A[2];
    double Nx=dy0*dz1-dy1*dz0;
    double Ny=dz0*dx1-dz1*dx0;
    double Nz=dx0*dy1-dx1*dy0;
    glNormal3d(Nx,Ny,Nz);
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
    //  ex15: one texture rectangle on each cube face.
    Texture(GRAIN);
    //  Cube
    glBegin(GL_QUADS);
    //  Front
    glNormal3f(0,0,1);
    glColor3d(1.0*r,1.0*g,1.0*b);
    MaterialCoord(GRAIN,0,0);
    glVertex3f(-1,-1, 1);
    MaterialCoord(GRAIN,1,0);
    glVertex3f(+1,-1, 1);
    MaterialCoord(GRAIN,1,1);
    glVertex3f(+1,+1, 1);
    MaterialCoord(GRAIN,0,1);
    glVertex3f(-1,+1, 1);
    //  Back
    glNormal3f(0,0,-1);
    glColor3d(0.75*r,0.75*g,0.75*b);
    MaterialCoord(GRAIN,0,0);
    glVertex3f(+1,-1,-1);
    MaterialCoord(GRAIN,1,0);
    glVertex3f(-1,-1,-1);
    MaterialCoord(GRAIN,1,1);
    glVertex3f(-1,+1,-1);
    MaterialCoord(GRAIN,0,1);
    glVertex3f(+1,+1,-1);
    //  Right
    glNormal3f(1,0,0);
    glColor3d(0.85*r,0.85*g,0.85*b);
    MaterialCoord(GRAIN,0,0);
    glVertex3f(+1,-1,+1);
    MaterialCoord(GRAIN,1,0);
    glVertex3f(+1,-1,-1);
    MaterialCoord(GRAIN,1,1);
    glVertex3f(+1,+1,-1);
    MaterialCoord(GRAIN,0,1);
    glVertex3f(+1,+1,+1);
    //  Left
    glNormal3f(-1,0,0);
    glColor3d(0.65*r,0.65*g,0.65*b);
    MaterialCoord(GRAIN,0,0);
    glVertex3f(-1,-1,-1);
    MaterialCoord(GRAIN,1,0);
    glVertex3f(-1,-1,+1);
    MaterialCoord(GRAIN,1,1);
    glVertex3f(-1,+1,+1);
    MaterialCoord(GRAIN,0,1);
    glVertex3f(-1,+1,-1);
    //  Top
    glNormal3f(0,1,0);
    glColor3d(1.1*r,1.1*g,1.1*b);
    MaterialCoord(GRAIN,0,0);
    glVertex3f(-1,+1,+1);
    MaterialCoord(GRAIN,1,0);
    glVertex3f(+1,+1,+1);
    MaterialCoord(GRAIN,1,1);
    glVertex3f(+1,+1,-1);
    MaterialCoord(GRAIN,0,1);
    glVertex3f(-1,+1,-1);
    //  Bottom
    glNormal3f(0,-1,0);
    glColor3d(0.55*r,0.55*g,0.55*b);
    MaterialCoord(GRAIN,0,0);
    glVertex3f(-1,-1,-1);
    MaterialCoord(GRAIN,1,0);
    glVertex3f(+1,-1,-1);
    MaterialCoord(GRAIN,1,1);
    glVertex3f(+1,-1,+1);
    MaterialCoord(GRAIN,0,1);
    glVertex3f(-1,-1,+1);
    //  End
    glEnd();
    //  Undo transformations
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
 }
 
 
 /*
  *  Draw vertex in polar coordinates
  */
 static void Vertex(double th,double ph,int color,int tex)
 {
    if (color)
       glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
    //  The ball image is a photograph: project its circular surface onto both
    //  hemispheres. Shared coordinates meet continuously around their boundary.
    //  Stay inside the photographed ball to exclude the black background.
    if (tex==BALL)
       glTexCoord2d(0.51+0.45*Sin(th)*Cos(ph),0.50+0.45*Sin(ph));
    else
       //  Longitude wraps 0..1; latitude maps the poles to 0 and 1.
       MaterialCoord(tex,th/360.0,(ph+90)/180.0);
    glNormal3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
    glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
 }
 
 static void sphere(double x,double y,double z,double r,int color,int tex)
 {
    //  Keep the existing handmade mesh, using finer steps for the soccer ball.
    int step=(tex==BALL) ? 5 : 30;
    if (tex==BALL)
    {
       //  ex15 material setup: neutral color and a modest plastic highlight.
       const float ballAmbient[]={0.20,0.20,0.20,1};
       const float ballDiffuse[]={0.70,0.70,0.70,1};
       const float ballSpecular[]={0.25,0.25,0.25,1};
       glPushAttrib(GL_LIGHTING_BIT);
       glDisable(GL_COLOR_MATERIAL);
       glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ballAmbient);
       glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,ballDiffuse);
       glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,ballSpecular);
       glColor3f(1,1,1);
    }
    Texture(tex);
    glPushMatrix();
    glTranslated(x,y,z);
    glScaled(r,r,r);
    //  Roll with the existing passing motion; inspection mode stays stationary.
    if (tex==BALL && obj==0)
       glRotated(-(x+1.8)*180/(3.14159265*r),0,0,1);
    for (int ph=-90;ph<90;ph+=step)
    {
       glBegin(GL_QUAD_STRIP);
       for (int th=0;th<=360;th+=step)
       {
          Vertex(th,ph,color,tex);
          Vertex(th,ph+step,color,tex);
       }
       glEnd();
    }
    glPopMatrix();
    if (tex==BALL)
       glPopAttrib();
    glDisable(GL_TEXTURE_2D);
 }
 
 static void cylinder(double x,double y,double z,double r,double length,double angle,int tex)
 {
    Texture(tex);
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(angle,0,0,1);
    glScaled(r,length,r);
 
    glBegin(GL_QUAD_STRIP);
    for (int a=0;a<=360;a+=15)
    {
       glNormal3d(Cos(a),0,Sin(a));
       MaterialCoord(tex,a/360.0,1);
       glVertex3d(Cos(a),0,Sin(a));
       MaterialCoord(tex,a/360.0,0);
       glVertex3d(Cos(a),-1,Sin(a));
    }
    glEnd();
 
    for (int end=0;end<2;end++)
    {
       glBegin(GL_TRIANGLE_FAN);
       glNormal3f(0,end ? -1 : 1,0);
       MaterialCoord(tex,0.5,0.5);
       glVertex3d(0,-end,0);
       for (int a=0;a<=360;a+=15)
       {
          //  ex14's radial disk texture coordinates.
          MaterialCoord(tex,0.5+0.5*Cos(a),0.5+0.5*Sin(end?a:-a));
          glVertex3d(Cos(a),-end,Sin(end?a:-a));
       }
       glEnd();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
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
    sphere(0,2.6,0,0.3,0,SKIN);
 
    glColor3f(0.85,0.62,0.43);
    cylinder(0,2.2,0.48, 0.12,0.8,-leftLeg,SKIN);
    cylinder(0,2.2,-0.48, 0.12,0.8,leftLeg,SKIN);
    glColor3f(0.20,0.25,0.34);
    cylinder(0,1.5,0.22, 0.15,1.5,leftLeg,GRAIN);
    cylinder(0,1.5,-0.22, 0.15,1.5,rightLeg,GRAIN);
    shoe(0.22,leftLeg);
    shoe(-0.22,rightLeg);
    glPopMatrix();
 }
 
 static void pitch(void)
 {
    //  ex10: offset the ground to avoid coplanar object-base artifacts.
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1,1);
    glNormal3f(0,1,0);
    if (textured)
    {
       //  ex13/ex15 material lighting, multiplied by the grass via GL_MODULATE.
       //  White ambient/diffuse plus both lights saturates before texturing.
       //  Use a neutral, matte material so the moving light remains visible.
       const float grassAmbient[]={0.20,0.20,0.20,1};
       const float grassDiffuse[]={0.75,0.75,0.75,1};
       const float grassSpecular[]={0,0,0,1};
       glPushAttrib(GL_LIGHTING_BIT);
       glDisable(GL_COLOR_MATERIAL);
       glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,grassAmbient);
       glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,grassDiffuse);
       glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,grassSpecular);
       glColor3f(1,1,1);  // Original image colors when lighting is switched off.
    }
    else
       glColor3f(0.12,0.36,0.20);
    Texture(GRASS);
    //  Repeat ex14's textured square at one-world-unit scale on the same plane.
    glBegin(GL_QUADS);
    if (textured)
       for (int x=-7;x<7;x++)
          for (int z=-6;z<6;z++)
          {
             MaterialCoord(GRASS,0,0); glVertex3d(x,0,z);
             MaterialCoord(GRASS,0,1); glVertex3d(x,0,z+1);
             MaterialCoord(GRASS,1,1); glVertex3d(x+1,0,z+1);
             MaterialCoord(GRASS,1,0); glVertex3d(x+1,0,z);
          }
    else
    {
       //  Retain HW5's original ground rendering for the texture-off comparison.
       glVertex3d(-7,0,-6);
       glVertex3d(-7,0,6);
       glVertex3d(7,0,6);
       glVertex3d(7,0,-6);
    }
    glEnd();
    if (textured)
       glPopAttrib();  // Restore HW5's material settings for the other objects.
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_POLYGON_OFFSET_FILL);
 
 }
 
 
 static void cone(double x,double z)
 {
    Texture(CHECKER);
    //  ex14 repetition: two texture repeats across the cone diameter.
    glPushMatrix();
    glTranslated(x,0,z);
    glColor3f(1,0.4,0);
    glBegin(GL_TRIANGLES);
    for (int a=0;a<360;a+=30)
    {
       glNormal3d(0.45*Cos(a+15),0.2,-0.45*Sin(a+15));
       glTexCoord2d(0.5,0.5);
       glVertex3d(0,0.45,0);
       glNormal3d(0.45*Cos(a),0.2,-0.45*Sin(a));
       glTexCoord2d(0.5+Cos(a),0.5-Sin(a));
       glVertex3d(0.2*Cos(a),0,-0.2*Sin(a));
       glNormal3d(0.45*Cos(a+30),0.2,-0.45*Sin(a+30));
       glTexCoord2d(0.5+Cos(a+30),0.5-Sin(a+30));
       glVertex3d(0.2*Cos(a+30),0,-0.2*Sin(a+30));
    }
    glEnd();
 
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,-1,0);
    glTexCoord2d(0.5,0.5);
    glVertex3d(0,0,0);
    for (int a=0;a<=360;a+=30)
    {
       glTexCoord2d(0.5+Cos(a),0.5+Sin(a));
       glVertex3d(0.2*Cos(a),0,0.2*Sin(a));
    }
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
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
 
    Texture(GRAIN);
    glColor3f(0.8,0.65,0.35);
    glBegin(GL_QUADS);
    for (int a=0;a<12;a++)
       for (int j=0;j<4;j++)
       {
          int k=(j+1)%4;
          Normal(handle[a][j],handle[a+1][k],handle[a+1][j]);
          MaterialCoord(GRAIN,j/4.0,a/12.0);
          glVertex3dv(handle[a][j]);
          MaterialCoord(GRAIN,j/4.0,(a+1)/12.0);
          glVertex3dv(handle[a+1][j]);
          MaterialCoord(GRAIN,(j+1)/4.0,(a+1)/12.0);
          glVertex3dv(handle[a+1][k]);
          MaterialCoord(GRAIN,(j+1)/4.0,a/12.0);
          glVertex3dv(handle[a][k]);
       }
    glNormal3f(0,-1,0);
    const double uv[4][2]={{0,0},{1,0},{1,1},{0,1}};
    for (int j=3;j>=0;j--)
    {
       MaterialCoord(GRAIN,uv[j][0],uv[j][1]);
       glVertex3dv(handle[0][j]);
    }
    for (int j=0;j<4;j++)
    {
       MaterialCoord(GRAIN,uv[j][0],uv[j][1]);
       glVertex3dv(handle[12][j]);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
 }
 
 
 static void duffel(double x,double z,double size,double angle)
 {
    //  Keep HW5's control profile and taper, rounding their corners three times.
    enum { NP=72, NR=56 };
    double profile[NP][2]={
       {-0.6,0},{0.6,0},{0.8,0.2},{0.8,0.6},{0.5,0.9},
       {0,1},{-0.5,0.9},{-0.8,0.6},{-0.8,0.2}
    };
    const double along[5]={-0.75,-0.55,0,0.55,0.75};
    const double width[5]={0.65,1,1.05,1,0.65};
    const double height[5]={0.65,0.9,1,0.9,0.65};
    //  Sections contain x, base height, width scale, and height scale.
    //  The two collapsed end sections close the bag with rounded rims.
    double section[NR][4]={{-0.75,0.2,0,0}};
    int np=9,nr=7;
    for (int i=0;i<5;i++)
    {
       section[i+1][0]=along[i];
       section[i+1][2]=width[i];
       section[i+1][3]=height[i];
    }
    section[6][0]=0.75;
    section[6][1]=0.2;
    for (int pass=0;pass<3;pass++)
    {
       double roundedProfile[NP][2];
       double roundedSection[NR][4];
       for (int j=0;j<np;j++)
          for (int c=0;c<2;c++)
          {
             int k=(j+1)%np;
             roundedProfile[2*j][c]=0.75*profile[j][c]+0.25*profile[k][c];
             roundedProfile[2*j+1][c]=0.25*profile[j][c]+0.75*profile[k][c];
          }
       for (int c=0;c<4;c++)
       {
          roundedSection[0][c]=section[0][c];
          roundedSection[2*nr-1][c]=section[nr-1][c];
          for (int i=0;i<nr-1;i++)
          {
             roundedSection[2*i+1][c]=0.75*section[i][c]+0.25*section[i+1][c];
             roundedSection[2*i+2][c]=0.25*section[i][c]+0.75*section[i+1][c];
          }
       }
       np*=2;
       nr*=2;
       for (int j=0;j<np;j++)
          for (int c=0;c<2;c++) profile[j][c]=roundedProfile[j][c];
       for (int i=0;i<nr;i++)
          for (int c=0;c<4;c++) section[i][c]=roundedSection[i][c];
    }

    double v[NR][NP][3],normal[NR][NP][3];
    double perimeter[NR][NP+1]={{0}},uv[NR][NP+1][2];
    for (int i=0;i<nr;i++)
       for (int j=0;j<np;j++)
       {
          v[i][j][0]=section[i][0];
          v[i][j][1]=section[i][1]+0.65*section[i][3]*profile[j][1];
          v[i][j][2]=0.42*section[i][2]*profile[j][0];
       }
    //  Cross the two surface tangents to get a shared outward vertex normal.
    //  GL_NORMALIZE handles normalization after the bag's scale transform.
    for (int i=1;i<nr-1;i++)
       for (int j=0;j<np;j++)
       {
          int prev=(j+np-1)%np,next=(j+1)%np;
          double a[3],b[3];
          for (int c=0;c<3;c++)
          {
             a[c]=v[i+1][j][c]-v[i-1][j][c];
             b[c]=v[i][next][c]-v[i][prev][c];
          }
          normal[i][j][0]=a[1]*b[2]-a[2]*b[1];
          normal[i][j][1]=a[2]*b[0]-a[0]*b[2];
          normal[i][j][2]=a[0]*b[1]-a[1]*b[0];
       }
    //  Preserve distance-based leather mapping and close two mirrored repeats.
    for (int i=1;i<nr-1;i++)
       for (int j=0;j<np;j++)
       {
          int k=(j+1)%np;
          double dy=v[i][k][1]-v[i][j][1];
          double dz=v[i][k][2]-v[i][j][2];
          perimeter[i][j+1]=perimeter[i][j]+sqrt(dy*dy+dz*dz);
       }
    double density=2/perimeter[nr/2][np];
    for (int i=1;i<nr-1;i++)
       for (int j=0;j<=np;j++)
       {
          int k=j%np;
          double dx=v[i][k][0]-v[i-1][k][0];
          double dy=v[i][k][1]-v[i-1][k][1];
          double dz=v[i][k][2]-v[i-1][k][2];
          uv[i][j][0]=(i==1 ? 0 : uv[i-1][j][0])+density*sqrt(dx*dx+dy*dy+dz*dz);
          uv[i][j][1]=2*perimeter[i][j]/perimeter[i][np];
       }

    if (textured)
    {
       //  ex15 material setup: let the leather image supply its own brown color.
       const float leatherAmbient[]={0.20,0.20,0.20,1};
       const float leatherDiffuse[]={0.75,0.75,0.75,1};
       const float leatherSpecular[]={0.20,0.20,0.20,1};
       glPushAttrib(GL_LIGHTING_BIT);
       glDisable(GL_COLOR_MATERIAL);
       glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,leatherAmbient);
       glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,leatherDiffuse);
       glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,leatherSpecular);
       glColor3f(1,1,1);
    }
    Texture(LEATHER);
    glPushMatrix();
    glTranslated(x,0,z);
    glRotated(angle,0,1,0);
    glScaled(size,size,size);
    for (int i=1;i<nr-2;i++)
    {
       //  Planar end-panel mapping avoids pinching the leather at each pole.
       int endPanel=(section[i+1][0]<=-0.60 || section[i][0]>=0.60);
       if (!textured)
          glColor3f(endPanel ? 0.42 : 0.55,endPanel ? 0.16 : 0.22,endPanel ? 0.08 : 0.12);
       glBegin(GL_QUAD_STRIP);
       for (int j=0;j<=np;j++)
          for (int row=i;row<=i+1;row++)
          {
             int k=j%np;
             glNormal3dv(normal[row][k]);
             if (endPanel)
                MaterialCoord(LEATHER,0.5+density*v[row][k][2],density*v[row][k][1]);
             else
                MaterialCoord(LEATHER,uv[row][j][0],uv[row][j][1]);
             glVertex3dv(v[row][k]);
          }
       glEnd();
    }
    if (!textured) glColor3f(0.42,0.16,0.08);
    for (int end=0;end<2;end++)
    {
       int pole=end ? nr-1 : 0;
       int ring=end ? nr-2 : 1;
       glBegin(GL_TRIANGLE_FAN);
       glNormal3f(end ? 1 : -1,0,0);
       MaterialCoord(LEATHER,0.5,density*v[pole][0][1]);
       glVertex3dv(v[pole][0]);
       for (int j=0;j<=np;j++)
       {
          int k=(end ? np-j : j)%np;
          glNormal3dv(normal[ring][k]);
          MaterialCoord(LEATHER,0.5+density*v[ring][k][2],density*v[ring][k][1]);
          glVertex3dv(v[ring][k]);
       }
       glEnd();
    }
    //  Restore the original material before drawing the tan, grain-textured straps.
    if (textured)
       glPopAttrib();
    straps(-0.20);
    straps(0.20);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
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
 
    glDisable(GL_TEXTURE_2D);
    glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT1);
    if (sunlight && obj==0)
    {
       //  Draw the sun unlit so its texture stays bright like a light source.
       if (textured)
          glColor3f(1,1,1);
       else
          glColor3f(1.0,0.85,0.2);
       sphere(sunPosition[0],sunPosition[1],sunPosition[2],0.7,0,SUN);
    }
    if (light)
    {
       float Ambient[]  = {0.01*ambient,0.01*ambient,0.01*ambient,1};
       float Diffuse[]  = {0.01*diffuse,0.01*diffuse,0.01*diffuse,1};
       float Specular[] = {0.01*specular,0.01*specular,0.01*specular,1};
       double radius=obj ? 0.5*distance : distance;
       float Position[] = {radius*Cos(zh),ylight,radius*Sin(zh),1};
       //  Draw the marker without lighting, using the existing handmade sphere.
       glColor3f(1,1,1);
       sphere(Position[0],Position[1],Position[2],0.12,0,-1);
       glEnable(GL_NORMALIZE);
       glEnable(GL_LIGHTING);
       glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
       glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
       glEnable(GL_COLOR_MATERIAL);
       glEnable(GL_LIGHT0);
       glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);
       glLightfv(GL_LIGHT0,GL_DIFFUSE,Diffuse);
       glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
       glLightfv(GL_LIGHT0,GL_POSITION,Position);
       if (sunlight)
       {
          float SunAmbient[] ={0.08,0.07,0.05,1};
          float SunDiffuse[] ={0.45,0.40,0.30,1};
          float SunSpecular[]={0.20,0.18,0.12,1};
          glEnable(GL_LIGHT1);
          glLightfv(GL_LIGHT1,GL_AMBIENT,SunAmbient);
          glLightfv(GL_LIGHT1,GL_DIFFUSE,SunDiffuse);
          glLightfv(GL_LIGHT1,GL_SPECULAR,SunSpecular);
          glLightfv(GL_LIGHT1,GL_POSITION,sunPosition);
       }
       float white[]={1,1,1,1};
       float black[]={0,0,0,1};
       glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
       glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
       glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
    }
 
    if (obj==0)
    {
       pitch();
       cone(-3.5,-1.4);
       cone(0.6,-1.6);
       cone(-0.8,1.3);
       cone(3.4,1.5);
 
       human(-2.7,0,1,0,0,right,0);
       human(2.7,-0.45,1.05,180,0,otherRight,1);
       human(-2,-5,1,-90,0,0,2);
       duffel(-0.7,-2.7,0.9,20);
       sphere(ballX,0.3,-0.22,0.3,0,BALL);
 
    }
    else if (obj==1)
       human(0,0,1,0,0,right,0);
    else if (obj==2)
       duffel(0,0,3,0);
    else if (obj==3)
    {
       glPushMatrix();
       glScaled(4,4,4);
       cone(0,0);
       glPopMatrix();
    }
    else if (obj==4)
       sphere(0,1.3,0,1,0,BALL);
 
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    glColor3f(1,1,1);
    glWindowPos2i(10,150);
    Print("t: textures %s (modulated with lighting)",textured ? "On" : "Off");
    glWindowPos2i(10,130);
    Print("u: sun/sunlight %s",sunlight ? (light ? "On" : "Visible (lighting off)") : "Off");
    glWindowPos2i(10,110);
    Print("Light=%s  Orbit=%s  Azimuth=%.0f Height=%.1f  Object=%s",
          light ? "On" : "Off",move ? "Moving" : "Stopped",zh,ylight,
          obj==0 ? "Scene" : obj==1 ? "Player" : obj==2 ? "Bag" : obj==3 ? "Cone" : "Ball");
    glWindowPos2i(10,90);
    Print("l: lighting  p: orbit  </>: move light  []: height  o/O: inspect objects  F1/F2/F3: light options");
    glWindowPos2i(10,70);
    Print("Ambient b/B=%d  Diffuse v/V=%d  Specular c/C=%d  Shininess n/N=%.0f  %s",
          ambient,diffuse,specular,shiny,smooth ? "Smooth" : "Flat");
    glWindowPos2i(10,50);
    Print("Projection=%s",
          mode==0 ? "Orthogonal" : mode==1 ? "Perspective" : "First person",
          mode==2 ? yaw : th,mode==2 ? look : ph,dim,fov,
          paused ? "Paused" : "Playing");
    glWindowPos2i(10,30);
    if (mode==2)
       Print("WASD: walk, Arrows: look around");   
    else
       Print("Arrows: change view");
    if (paused)
       Print(", Paused! press Space again to continue");
    else
       Print(", Playing! press Space again to pause");
    glWindowPos2i(10,10);
    Print("m: mode  Space: pause/play  +/- : zoom  m: switch mode  r: restart  0: reset cameras  Esc: quit");
 
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
    if (key==GLUT_KEY_F1) smooth=1-smooth;
    else if (key==GLUT_KEY_F2) local=1-local;
    else if (key==GLUT_KEY_F3) distance=(distance==6) ? 3 : 6;
    else if (mode==2)
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
    //  Zoom in: shrink the orthogonal view or narrow the perspective FOV.
    else if (ch=='+')
    {
       if (mode==0)
       {
          if (dim>2) dim-=0.5;
       }
       else if (fov>15)
          fov--;
    }
    //  Zoom out: enlarge the orthogonal view or widen the perspective FOV.
    else if (ch=='-')
    {
       if (mode==0)
       {
          if (dim<40) dim+=0.5;
       }
       else if (fov<100)
          fov++;
    }
    else if (ch=='t' || ch=='T') textured=1-textured;
    else if (ch=='l' || ch=='L') light=1-light;
    else if (ch=='u' || ch=='U') sunlight=1-sunlight;
    else if (ch=='p' || ch=='P') move=1-move;
    else if (ch=='<' || ch=='>')
    {
       move=0;
       zh=fmod(zh+(ch=='<' ? 5 : -5)+360,360);
    }
    else if (ch=='[' && ylight>0.5) ylight-=0.25;
    else if (ch==']' && ylight<10) ylight+=0.25;
    else if (ch=='b' && ambient>0) ambient-=5;
    else if (ch=='B' && ambient<100) ambient+=5;
    else if (ch=='v' && diffuse>0) diffuse-=5;
    else if (ch=='V' && diffuse<100) diffuse+=5;
    else if (ch=='c' && specular>0) specular-=5;
    else if (ch=='C' && specular<100) specular+=5;
    else if (ch=='n' && shininess> -1) shininess--;
    else if (ch=='N' && shininess<7) shininess++;
    else if (ch=='o' || ch=='O')
    {
       obj=(obj+(ch=='o' ? 1 : 4))%5;
       mode=0;
       dim=obj ? 3 : 11;
    }
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
    shiny=shininess<0 ? 0 : pow(2.0,shininess);
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
    if (move) zh=fmod(zh+45*dt,360);
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
    glutCreateWindow("Jay Vakil - HW6 Lighting and Textures");
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
    //  ex14/ex15: load BMPs once after creating the OpenGL context.
    LoadTextures();
    ErrCheck("textures");
    lastTime=glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();
    return 0;
 }
