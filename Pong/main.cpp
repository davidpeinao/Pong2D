#include <string>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <sstream>
#include <math.h>
#include <gl\gl.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <gl\glu.h>
#include "GL/freeglut.h"
 using namespace std;
// keycodes for W and S defined or it makes an error
#define VK_W 0x57
#define VK_S 0x53
// window size variables, with 60 fps
int width = 1000;
int height = 500;
int interval = 1000 / 60;
// score variables
int score_left = 0;
int score_right = 0;
// rackets in general variables
int racket_width = 10;
int racket_height = 80;
int racket_speed = 3;
// left racket position variables
float racket_left_x = 10.0f;
float racket_left_y = 50.0f;
// right racket position variables
float racket_right_x = width - racket_width - 10;
float racket_right_y = 50;
// ball variables
float ball_pos_x = width / 2;
float ball_pos_y = height / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
int ball_size = 15;
int ball_speed = 5;


void drawText( float x, float y, const char *string ) {
	int j = strlen( string );
	glColor3f( 1.0, 1.0, 1.0 );
	glRasterPos2f( x, y );
	for( int i = 0; i < j; i++ ) {
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, string[i] );
	}
}

void drawRect(float x, float y, float width, float height) {
    /* like this
        4   3
        1   2
    */
    glBegin(GL_QUADS);
        glVertex2f(x, y); // 1
        glVertex2f(x + width, y); // 2
        glVertex2f(x + width, y + height); // 3
        glVertex2f(x, y + height); // 4
    glEnd();
}

string int2str(int x) {
    // converts int to string
    stringstream ss;
    ss << x;
    return ss.str( );
}

void draw() {
    // clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    // draw result
    string result = int2str(score_left) + " - " + int2str(score_right);
    const char * s = result.c_str();
    drawText(width / 2 - 10, height - 20, s);
    // draw red racket
    glColor3f(1.0f, 0.0f, 0.0f);
    drawRect(racket_left_x, racket_left_y, racket_width, racket_height);
    // draw blue racket
    glColor3f(0.0f, 0.0f, 1.0f);
    drawRect(racket_right_x, racket_right_y, racket_width, racket_height);
    // draw ball in white
    glColor3f(1.0f, 1.0f, 1.0f);
    drawRect(ball_pos_x, ball_pos_y, ball_size, ball_size);
    // swap buffers (has to be done at the end)
    glutSwapBuffers();
}

void keyboard() {
    // left racket
    if (GetAsyncKeyState(VK_W) && racket_left_y < height-75) racket_left_y += racket_speed;
    if (GetAsyncKeyState(VK_S) && racket_left_y > 0) racket_left_y -= racket_speed;
    // right racket
    if (GetAsyncKeyState(VK_UP) && racket_right_y < height-75) racket_right_y += racket_speed;
    if (GetAsyncKeyState(VK_DOWN) && racket_right_y > 0) racket_right_y -= racket_speed;
}

void vec2_norm(float& x, float &y) {
        // sets a vectors length to 1 (which means that x + y == 1)
        float length = sqrt((x * x) + (y * y));
        if (length != 0.0f) {
            length = 1.0f / length;
            x *= length;
            y *= length;
        }
}

void updateBall() {
    // makes the ball move in its direction
    ball_pos_x += ball_dir_x * ball_speed;
    ball_pos_y += ball_dir_y * ball_speed;
    // hit by left racket = let it fly to the right and set the y direction depending on where it hit the racket
    // so it also flies up and down and not just to the left and to the right
    if (ball_pos_x < racket_left_x + racket_width &&
        ball_pos_x > racket_left_x &&
        ball_pos_y < racket_left_y + racket_height &&
        ball_pos_y > racket_left_y) {
        // set fly direction depending on where it hit the racket
        // t is 0.5 if hit at top, 0 at center and -0.5 at bottom
        float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = t;
    }
    // hit by right racket =  same as left racket, just to the opposite direction
    if (ball_pos_x > racket_right_x &&
        ball_pos_x < racket_right_x + racket_width &&
        ball_pos_y < racket_right_y + racket_height &&
        ball_pos_y > racket_right_y) {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
        float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = t;
    }
    // hit left wall =  increase right player's score and reset ball to the center
    if (ball_pos_x < 0) {
        ++score_right;
        ball_pos_x = width / 2;
        ball_pos_y = height / 2;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = 0;
    }
    // hit right wall = increase left player's score and reset ball to the center
    if (ball_pos_x > width) {
        ++score_left;
        ball_pos_x = width / 2;
        ball_pos_y = height / 2;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = 0;
    }
    // hit top wall = invert the ball's y fly direction
    if (ball_pos_y > height) {
        ball_dir_y = -fabs(ball_dir_y); // force it to be negative
    }
    // hit bottom wall = invert the ball's fly direction
    if (ball_pos_y < 0) {
        ball_dir_y = fabs(ball_dir_y); // force it to be positive
    }
    // make sure that the length of direction stays at 1
    vec2_norm(ball_dir_x, ball_dir_y);
}

void update(int value) {
   // input handling
   keyboard();
   // update ball
   updateBall();
   // call update() again in 'interval' milliseconds
   glutTimerFunc(interval, update, 0);
   // redisplay frame
   glutPostRedisplay();
}

void enable2D(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    // initialize opengl (via glut)
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Pong");
    glutDisplayFunc(draw);
    glutTimerFunc(interval, update, 0);
    // setup scene to 2d mode and set draw color to white by default, I can change it above
    enable2D(width, height);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutMainLoop();
    return 0;
}
