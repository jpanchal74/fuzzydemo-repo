//
//  main.cpp
//  FuzzyLogicDemo
//
//  Created by Jignesh Panchal on 11/2/22.
//

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string.h>

#ifdef __APPLE__
// Defined before OpenGL and GLUT includes to avoid deprecation messages
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE
#define GLEW_STATIC

//#include <glad/glad.h>
//GLEW
#include <GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>
//GLUT
#include <GL/glut.h>
#include <GL/glx.h>
#endif

typedef int BOOL;
#define TRUE 1
#define FALSE 0

enum {
    MENU_RECTANGLE = 1, // 1
    MENU_BARTLETT,      // 2
    MENU_HANNING,       // 3
    MENU_HAMMING,       // 4
    MENU_BLACKMAN,      // 5
    MENU_FILTER,        // f
    MENU_LOWPASS,       // l
    MENU_HIGHPASS,      // h
    MENU_BANDPASS,      // b
    MENU_BANDSTOP,      // p
    MENU_INPUT,         // i
    MENU_SINE,          // s
    MENU_SQUARE,        // q
    MENU_NOISE,         // n
    MENU_NOISYSINE,     // ns
    MENU_NOISYSQUARE,   // nq
    MENU_OUTPUT,        // o
    MENU_EXIT,          // Esc
    MENU_EMPTY
};

static BOOL g_bButtonLEFTDown = FALSE;
static BOOL g_bButtonRIGHTDown = FALSE;
static BOOL g_bPause = TRUE;

static int g_yClick = 0;

static int g_Width = 680;                          // Initial window width
static int g_Height = 480;

#define L 30.0
#define W 15.0

struct var_type{
    char vr[5];
    char vname[5];
    int rn;
    int rm;
    int rp;
}var[20];

struct set_type{
    char vin1[5];
    char vin2[5];
    char vout[5];
}set[40];

// TRUCK/Van Location coordinates
int xm_glut = g_Width/8;
int ym_glut = g_Height/8;
int xm, ym;
int phi = 10;

int dx,dy,phi90,count_var,count_set;
float thi;

static BOOL docked = FALSE;


int XcordTransformBGIToGLUT(int x, int xmax_glut)
{
    return (x - (xmax_glut/2));
}

int YcordTransformBGIToGLUT(int y, int ymax_glut)
{
    return (-y + (ymax_glut/2));
}

int XcordTransformGLUTToBGI(int x_glut, int xmax_glut)
{
    return (x_glut + (xmax_glut/2));
}

int YcordTransformGLUTToBGI(int y_glut, int ymax_glut)
{
    return (-y_glut + (ymax_glut/2));
}

/*
 * Function that handles the drawing of a circle using the triangle fan
 * method. This will create a filled circle.
 *
 * Params:
 *    x (GLFloat) - the x position of the center point of the circle
 *    y (GLFloat) - the y position of the center point of the circle
 *    radius (GLFloat) - the radius that the painted circle will have
 */
void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius){
    int i;
    int triangleAmount = 20; //# of triangles used to draw circle
    
    //GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * M_PI;
    
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // center of circle
        for(i = 0; i <= triangleAmount;i++) {
            glVertex2f(
                    x + (radius * cos(i *  twicePi / triangleAmount)),
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();
}

/*
 * Function that handles the drawing of a circle using the line loop
 * method. This will create a hollow circle.
 *
 * Params:
 *    x (GLFloat) - the x position of the center point of the circle
 *    y (GLFloat) - the y position of the center point of the circle
 *    radius (GLFloat) - the radius that the painted circle will have
 */
void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius){
    int i;
    int lineAmount = 100; //# of triangles used to draw circle
    
    //GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * M_PI;
    
    glBegin(GL_LINE_LOOP);
        for(i = 0; i <= lineAmount;i++) {
            glVertex2f(
                x + (radius * cos(i *  twicePi / lineAmount)),
                y + (radius* sin(i * twicePi / lineAmount))
            );
        }
    glEnd();
}

/**
* Draw a character string.
*
* @param x        The x position
* @param y        The y position
* @param string   The character string
*/
void drawString(int x, int y, char *string)
{
    glRasterPos2f(x, y);

    for (char* c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);  // Updates the position
    }
}

int fuzzy_triangle(int rn,int rp,int rm,int in)
{
    int out;

    if((in<rn)||(in>rp))out=0;if(in==rm)out=100;
    if((in>rn)&&(in<rm))out=100*(in-rn)/(rm-rn);
    else out=100*(rp-in)/(float)(rp-rm);
    return out;
}

void fuzzy_rev_triangle(int rn,int rp,int rm,int out,float *in1,float *in2)
{
    *in1 = (float)rn + (float)(rm-rn)*out/100.0;
    *in2 = (float)rm + (float)(rp-rm)*out/100.0;
}

void var_set_load(void)
{
    FILE *fp;
    int dmy,i;
    char str[5];

    //---------------------------------------
    if((fp=fopen("range_x.txt","r"))==NULL)
    {
        printf("Error: Can't open range_x.txt");
        glutDestroyWindow(glutGetWindow());
        exit(0);
    }
    fscanf(fp,"%s",str);
    fscanf(fp,"%s",str);
    fscanf(fp,"%d",&count_var);
    fscanf(fp,"%d",&dmy);
    fscanf(fp,"%d",&dmy);
    for(i=0;i<count_var;i++)
    {
        fscanf(fp,"%s",var[i].vr);
        fscanf(fp,"%s",var[i].vname);
        fscanf(fp,"%d",&var[i].rn);
        fscanf(fp,"%d",&var[i].rm);
        fscanf(fp,"%d",&var[i].rp);
    }
    fclose(fp);
    //---------------------------------------

    //---------------------------------------
    if((fp=fopen("set.txt","r"))==NULL)
    {
        printf("Error: Can't open set.txt");
        glutDestroyWindow(glutGetWindow());
        exit(0);
    }
    fscanf(fp,"%s",str);
    fscanf(fp,"%d",&count_set);
    fscanf(fp,"%d",&dmy);
    for(i=0;i<count_set;i++){
        fscanf(fp,"%s",set[i].vin1);
        fscanf(fp,"%s",set[i].vin2);
        fscanf(fp,"%s",set[i].vout);
    }
    fclose(fp);
    //---------------------------------------
}


int cir(int rl,int rk,int xl,int yl,int xk,int yk,int *xa,int *ya,int *xb,int *yb)
{
    float d,de,su,r,di,sc,x1,y1,x2,y2,x,y;
    int s=1,s1=2;
    
    d = pow(xl-xk,2)+pow(yl-yk,2);

    if(d==0)
    {
        return s1;
        //How this return used?
    }
    /*printf("two circle have the same centre");*/
    else
    {
        de = pow(rl,2)-pow(rk,2);
        su = pow(rk,2)+pow(rl,2);
        
        r = 2.0*su*d - d*d - de*de;
        
        if(r<0)
        {
            return s;
            //How this return used?
        }
        else
        {
            di = 0.5/d;
            sc = 0.5 - de*di;
            x = (xl-xk)*sc + xk;
            y = (yl-yk)*sc + yk;
            if(r==0)
            {
                *xa=ceil(x);*ya=ceil(y);*xb=ceil(x);*yb=ceil(y);
            }
            else
            {
                r=di*pow(r,0.5);
                x1 = x - (yl-yk)*r;
                y1 = y + (xl-xk)*r;
                x2 = x + (yl-yk)*r;
                y2 = y - (xl-xk)*r;
                *xa=ceil(x1);*ya=ceil(y1);
                *xb=ceil(x2);*yb=ceil(y2);
                /*printf("circle intersect at two points at (%d,%d) & (%d,%d)",(int)x1,(int)y1,(int)x2,(int)y2);*/
            }
            
            return 0;
            //How this return used?
        }
    }
}

void recordNewSystemState(void)
{
    if(phi==0){phi90=0;dx=xm+(int)L;dy=ym;}
    if((phi>0)&&(phi<90)){
        phi90=phi;
        dy=ym-(int)(L*sin(22.0*phi90/(180.0*7.0)));
        dx=xm+(int)(L*cos(22.0*phi90/(180.0*7.0)));
    }
    if(phi==90){phi90=90;dx=xm;dy=ym-(int)L;}
    if((phi>90)&&(phi<180)){
        phi90=phi-90;
        dy=ym-(int)(L*cos(22.0*phi90/(180.0*7.0)));
        dx=xm-(int)(L*sin(22.0*phi90/(180.0*7.0)));
    }
    if(phi==180){phi90=0;dx=xm-(int)L;dy=ym;}
    if((phi>180)&&(phi<270)){
        phi90=phi-180;
        dy=ym+(int)(L*sin(22.0*phi90/(180.0*7.0)));
        dx=xm-(int)(L*cos(22.0*phi90/(180.0*7.0)));
    }
    if(phi==270){phi90=90;dx=xm;dy=ym+(int)L;}
    if((phi>-90)&&(phi<0)){
        phi90=-phi;
        dy=ym+(int)(L*sin(22.0*phi90/(180.0*7.0)));
        dx=xm+(int)(L*cos(22.0*phi90/(180.0*7.0)));
    }
    
    
    //printf("updateSystemState():(xm=%d, ym=%d):(Phi=%d Deg, Thi=%3.2f Deg)\n", XcordTransformBGIToGLUT(xm,g_Width), YcordTransformBGIToGLUT(ym,g_Height), phi, thi);
    
    //----------------------
    // Draw TRUCK
    //----------------------
    glColor3f(1.0,0.0,0.0);
    glLineWidth(2);
    
    glBegin(GL_LINE_STRIP);
        glVertex2d(XcordTransformBGIToGLUT(xm,g_Width),YcordTransformBGIToGLUT(ym,g_Height));
        glVertex2d(XcordTransformBGIToGLUT(dx,g_Width),YcordTransformBGIToGLUT(dy,g_Height));
    glEnd();
    
    drawHollowCircle(XcordTransformBGIToGLUT(xm,g_Width),YcordTransformBGIToGLUT(ym,g_Height),20);
    //----------------------
    
    //----------------------
    // Draw the Dock
    //----------------------
    glColor3f (1.0, 1.0, 1.0);
    //rectangle(getmaxx()/2,0,getmaxx()/2+40,50);
    //glRecti( XcordTransformBGIToGLUT((680/2),g_Width),YcordTransformBGIToGLUT(0,g_Height), XcordTransformBGIToGLUT((680/2)+40,g_Width), YcordTransformBGIToGLUT(50,g_Height));
    glBegin (GL_LINE_LOOP);
        glVertex2d( XcordTransformBGIToGLUT((680/2)-40,g_Width), YcordTransformBGIToGLUT(0,g_Height) );
        glVertex2d( XcordTransformBGIToGLUT((680/2)+40,g_Width), YcordTransformBGIToGLUT(0,g_Height) );
        glVertex2d( XcordTransformBGIToGLUT((680/2)+40,g_Width), YcordTransformBGIToGLUT(60,g_Height) );
        glVertex2d( XcordTransformBGIToGLUT((680/2)-40,g_Width), YcordTransformBGIToGLUT(60,g_Height) );
    glEnd ();
    char filter_string[100];
    snprintf(filter_string, 100, "DOCK\n");
    drawString(XcordTransformBGIToGLUT((680/2)-40,g_Width)+25,YcordTransformBGIToGLUT(0,g_Height)-10,filter_string);
    //----------------------
    
    //----------------------
    // X,Y and Phi
    //----------------------
    glColor3f(1.0,1.0,0.0);
    //char filter_string[100];
    snprintf(filter_string, 100, "x = %d, y = %d, Phi = %d Deg\n", XcordTransformBGIToGLUT(xm,g_Width), YcordTransformBGIToGLUT(ym,g_Height), phi);
    drawString((-g_Width/2)+10,(g_Height/2)-10,filter_string);
    //----------------------
}

void updateSystemState(void)
{
    //int ex, ey, sx, sy, phi0;
    int xmm=0, ymm=0;
    int rx=0,ry=0;
    int mul=0, r=0;
    int dmy=0, dmx=0;
    float deg=-4.0,rad=0.0;

    if(thi==0){
        if(xm==dx)
        {
            if(ym>dy)
            {
                ym-=2;dy-=2;phi=90;
            }
            if(ym<dy)
            {
                ym+=2;dy+=2;phi=270;
            }
        }
        if(ym==dy)
        {
            if(xm>dx)
            {
                xm+=2;dy+=2;phi=0;
            }
            if(xm<dx)
            {
                xm-=2;dx-=2;phi=180;
            }
        }
        goto END;
    }
        
    xmm = (xm+dx)/2;
    ymm = (ym+dy)/2;
    r = abs((int)(30.0*40.0/thi));

    if(thi>=0)
    {
        mul=1;
        
    }
    else
    {
        mul=-1;
    }
    
    deg = deg*mul;
    
    if(phi==0)
    {
        ry = ymm - mul*r;
        rx = xmm;
    }
    
    if( (phi>0) && (phi<90) )
    {
        rad = atan2((float)r,L/2)+atan2(L/2,(float)r)-((float)phi*22.0/(7.0*180.0));
        ry = ymm - mul*r*sin(rad);
        rx = xmm - mul*r*cos(rad);
    }
    
    if(phi==90)
    {
        rx = xmm - mul*r;
        ry = ymm;
    }
    
    if( (phi>90) && (phi<180) )
    {
        rad = atan2((float)r,L/2)+atan2(L/2,(float)r)-((float)(180-phi)*22.0/(7.0*180.0));
        ry = ymm + mul*r*sin(rad);
        rx = xmm - mul*r*cos(rad);
    }
    
    if(phi==180)
    {
        ry = ymm + mul*r;
        rx = xmm;
    }
    
    if( (phi>180) && (phi<270) )
    {
        rad = atan2((float)r,L/2)+atan2(L/2,(float)r)-((float)(phi-180)*22.0/(7.0*180.0));
        ry = ymm + mul*r*sin(rad);
        rx = xmm + mul*r*cos(rad);
    }
    
    if(phi==270)
    {
        rx = xmm + mul*r;
        ry = ymm;
    }
    
    if( (phi>-90) && (phi<0) )
    {
        rad = atan2((float)r,L/2)+atan2(L/2,(float)r)-((float)(-phi)*22.0/(7.0*180.0));
        ry = ymm - mul*r*sin(rad);
        rx = xmm + mul*r*cos(rad);
    }
    
    dmx = rx + (float)(xmm-rx)*cos(22.0*deg/(180.0*7.0))-(float)(ymm-ry)*sin(22.0*deg/(180.0*7.0));
    dmy = ry + (float)(ymm-ry)*cos(22.0*deg/(180.0*7.0))+(float)(xmm-rx)*sin(22.0*deg/(180.0*7.0));

    if(thi>0)
    {
        cir(r,(int)(L/2),rx,ry,dmx,dmy,&dx,&dy,&xm,&ym);
    }
    else
    {
        cir(r,(int)(L/2),rx,ry,dmx,dmy,&xm,&ym,&dx,&dy);
    }
    
    if(xm!=dx)
    {
        phi=(int)(180.0*7*atan2((float)abs(ym-dy),(float)abs(xm-dx))/22.0);
    }
    
    if(xm>dx)
    {
        if(ym>dy)
        {
            phi=180-phi;
        }
        if(ym<dy)
        {
            phi=180+phi;
        }
        if(ym==dy)
        {
            phi=180;
        }
    }
    
    if(xm<dx)
    {
        if(ym>dy)
        {
            phi=phi;
        }
        if(ym<dy)
        {
            phi=-phi;
        }
        if(ym==dy)
        {
            phi=0;
        }
    }
    
    if(xm==dx)
    {
        if(ym>dy)
        {
            phi=90;
        }
        if(ym<dy)
        {
            phi=270;
        }
    }
    
END:
    
    recordNewSystemState();
}

void fuzzy_controller(void)
{
    int i,j,out1=0,out2=0,dx1,count;
    float in1,in2,sum=0.0;
    FILE *fp1;
    char str1[5],str2[5];
    
    
    //----------------------------------------------------------------
    if((fp1=fopen("result.txt","w"))==NULL)
    {
        printf("Error");
        glutDestroyWindow(glutGetWindow());
        exit(0);
    }
    
    dx1=(int)(dx*10.0/68.0);
    
    for(i=0;i<count_var;i++)
    {
        if(!strcmp(var[i].vr,"x"))
        {
            if( (var[i].rn<dx1) && (var[i].rp>dx1) )
            {
                out1 = fuzzy_triangle(var[i].rn,var[i].rp,var[i].rm,dx1);
                
                for(j=0;j<count_var;j++)
                {
                    if(!strcmp(var[j].vr,"p"))
                    {
                        if( (var[j].rn<phi) && (var[j].rp>phi) )
                        {
                            out2 = fuzzy_triangle(var[j].rn,var[j].rp,var[j].rm,phi);
                            fprintf(fp1,"%s %s %d %d\n",var[i].vname,var[j].vname,out1,out2);
                        }
                    }
                }
            }
        }
    }
    fclose(fp1);
    //----------------------------------------------------------------
    
    //----------------------------------------------------------------
    if((fp1=fopen("result.txt","r"))==NULL)
    {
        printf("Error");
        glutDestroyWindow(glutGetWindow());
        exit(0);
    }
    count=0;
    while(!feof(fp1)){
        fscanf(fp1,"%s",str1);fscanf(fp1,"%s",str2);
        fscanf(fp1,"%d",&out1);fscanf(fp1,"%d",&out2);
        if(out1>out2)out1=out2;
        for(i=0;i<count_set;i++){
            if((!strcmp(set[i].vin1,str1))&&(!strcmp(set[i].vin2,str2))){
                for(j=0;j<count_var;j++){
                    if((!strcmp(set[i].vout,var[j].vname))&&(!strcmp(var[j].vr,"t"))){
                        fuzzy_rev_triangle(var[j].rn,var[j].rp,var[j].rm,out1,&in1,&in2);
                        sum=sum+in1+in2;goto OUT;}
                }
            }
        }
    
    OUT:;
        count++;
    }
    fclose(fp1);
    //----------------------------------------------------------------
    
    thi=sum/count;
    
    updateSystemState();
    //getch();
}

void displaySystem(void)
{
    //Clear information from last draw
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    
    //Switch to the drawing perspective
    glMatrixMode(GL_MODELVIEW);
    
    //Start with identity matrix
    glLoadIdentity();

    recordNewSystemState();
    
    glFlush();
}

void reshape(GLint width, GLint height)
{
    //Viewport: area within drawing are displayed
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    //Setup viewing projection
    glMatrixMode(GL_PROJECTION);
    
    //Start with identity matrix
    glLoadIdentity();
    
    gluOrtho2D(-g_Width/2,g_Width/2,-g_Height/2,g_Height/2);
    
    //Switch to the drawing perspective
    glMatrixMode(GL_MODELVIEW);
}

void UpdateSystem(void)
{
    if (g_bPause == FALSE)
    {
        if (!docked)
        {
            fuzzy_controller();
            
            if( (phi>=90) && (phi<=94) && (dy<=10) )
                //if( (phi>=90) && (phi<=94) && (dy<=10) && (dx == 0) )
            {
                docked = TRUE;
            }
            glutPostRedisplay();
        }
        else
        {
            glColor3f(0.0,1.0,0.0);
            
            char filter_string[100];
            snprintf(filter_string, 100, "Docking Completed!!!\n");
            drawString((g_Width/4)+10,(g_Height/2)-10,filter_string);
            
            glFlush();
        }
    }
    else
    {
        glutPostRedisplay();
        
        glColor3f(0.0,1.0,1.0);
        
        char filter_string[100];
        snprintf(filter_string, 100, "*** System PAUSED ***\n");
        drawString((g_Width/4)-100,(g_Height/2)-10,filter_string);
        
        snprintf(filter_string, 100, "Press 'p' to toggle Unpuase/Pause\n");
        drawString((g_Width/4)-100,(g_Height/2)-20,filter_string);
        
        snprintf(filter_string, 100, "Use MOUSE to Repostion Truck\n");
        drawString((g_Width/4)-100,(g_Height/2)-30,filter_string);
        
        snprintf(filter_string, 100, "Press RIGHT button & move mouse UP/DOWN to reangle Truck\n");
        drawString((g_Width/4)-100,(g_Height/2)-40,filter_string);
        
        glFlush();
    }
    
}

void MouseButton(int button, int state, int x, int y)
{
    // Respond to mouse button presses.
    // If button1 pressed, mark this state so we know in motion function.
    if (button == GLUT_LEFT_BUTTON)
    {
        g_bButtonLEFTDown = (state == GLUT_DOWN) ? TRUE : FALSE;
        //xm = XcordTransformGLUTToBGI(x,g_Width);
        //ym = XcordTransformGLUTToBGI(y,g_Height);
        
        xm = x;
        ym = y;
        
        if (xm > g_Width) xm = g_Width;
        if (xm < -g_Width) xm = -g_Width;
        if (ym > g_Height) ym = g_Height;
        if (ym < -g_Height) ym = -g_Height;
        
        docked = FALSE;
        
        displaySystem();
        glutPostRedisplay();
        
        //printf("LEFT BUTTON: %d %d \n",button,state);
    }
    
    if (button == GLUT_RIGHT_BUTTON)
    {
        g_bButtonRIGHTDown = (state == GLUT_DOWN) ? TRUE : FALSE;
        
        g_yClick = y - phi;
        
        docked = FALSE;
        
        displaySystem();
        glutPostRedisplay();
        
        //printf("RIGHT BUTTON: %d %d \n",button,state);
    }

}

void MouseMotion(int x, int y)
{
    // If button1 pressed, zoom in/out if mouse is moved up/down.
    if (g_bButtonLEFTDown)
    {
        //xm = XcordTransformGLUTToBGI(x,g_Width);
        //ym = XcordTransformGLUTToBGI(y,g_Height);
        
        xm = x;
        ym = y;
        
        if (xm > g_Width) xm = g_Width;
        if (xm < -g_Width) xm = -g_Width;
        if (ym > g_Height) ym = g_Height;
        if (ym < -g_Height) ym = -g_Height;
        
        docked = FALSE;
        
        displaySystem();
        glutPostRedisplay();
        
        //printf("LEFT BUTTON MOTION\n");
    }
    
    if (g_bButtonRIGHTDown)
    {
        phi = (y - g_yClick);
        
        if (phi < -90) phi = 270 - (abs(phi)%90);
        if (phi > 360) phi = phi%360;
        
        if ( (phi > 270) & (phi <= 360) )
        {
            phi = phi - 360;
        }
        
        docked = FALSE;
        
        displaySystem();
        glutPostRedisplay();
            
        //printf("RIGHT BUTTON MOTION\n");
    }
}

void Keyboard(unsigned char key, int x, int y)
{
    //printf("%c\n",key);
    
    switch (key)
    {
        case 'p':
            g_bPause = !g_bPause;
            break;
        case 27: // ESCAPE key
            glutDestroyWindow(glutGetWindow());
            exit(0);
            break;
    }
}

int main(int argc, char **argv)
{
    //Create Data
    xm = XcordTransformGLUTToBGI(xm_glut,g_Width);
    ym = XcordTransformGLUTToBGI(ym_glut,g_Height);
    var_set_load();
    
    // GLUT Window Initialization:
    glutInit (&argc, argv);
    
    glutInitDisplayMode ( GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    
    //Position of Window appears on the terminal screen
    glutInitWindowPosition(0,0);
    
    //Size of the Window
    glutInitWindowSize(g_Width,g_Height);
    
    glutCreateWindow ("FuzzyLogic Demo : DOCK THE TRUCK");
    
    glClearColor(0.0, 0.0, 0.0, 1.0);         // BLACK background

    // Register callbacks:
    glutDisplayFunc(displaySystem);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    glutIdleFunc(UpdateSystem);
    
    // Turn the flow of control over to GLUT
    glutMainLoop ();
    
    glutDestroyWindow(glutGetWindow());
    exit(EXIT_SUCCESS);
}
