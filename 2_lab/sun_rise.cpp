#include <GL/glut.h>
#include <cmath>

void changeSize(int w, int h);

void ground();

void draw_sun(bool= false);

void draw_house();

void sky();

void timer(int);

void render_scene();

void init() {
    glClearColor(0, 0, (float) 110 / 255, 0);
}


int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Sunrise");

    glutDisplayFunc(render_scene); // redraw
    glutReshapeFunc(changeSize); // resize

    glutTimerFunc(50, timer, 0);
    init();
    glutMainLoop();

    return 0;
}

bool night = false;
GLfloat x_sun_pos = -1.0f;

void render_scene() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (night) {
        // drawing a Crescent
        glColor3f(1, 1, 1);
        draw_sun();

        glColor3f(0, 0, 0);
        draw_sun(true);
    } else {
        // day
        if (x_sun_pos < 0.7)
            // drawing a sun
            glColor3f(255.0f, 242.0f, 0.0f);
        else
            // drawing a moon
            glColor3f(1, 1, 1);

        draw_sun();
        sky();
    }

    ground();
    glutSwapBuffers();
}

void draw_house() {
    // house roof
    glColor3ub(145, 30, 66);

    GLfloat vertices[] = {
            -0.45f, 0.15f,
            -0.2f, -0.2f,
            -0.7f, -0.2f
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_VERTEX_ARRAY);

    // house foundation
    glColor3ub(255, 0, 14);
    glRectf(-0.7f, -0.7f, -0.2f, -0.2f);

    // window
    if (!night)
        glColor3f(1, 1, 1);
    else
        glColor3f(255.0f, 242.0f, 0.0f);

    glRectf(-0.6f, -0.4f, -0.3f, -0.3f);
}

void sky() {
    if (!night) {
        if (x_sun_pos < -0.8 || x_sun_pos >= 0.7) {
            // sunrise or sunset
            glClearColor(0, 0, (float) 110 / 255, 0);
        } else
            // day
            glClearColor(0, 0, 215, 0);
    }

    // black sky will be drawn in a timer when the night will start
}

void draw_sun(bool crescent) {
    glBegin(GL_TRIANGLE_FAN);
    GLfloat rad = 0.1f;
    GLfloat cy = 0.85f;
    glVertex2f(x_sun_pos, cy);

    for (unsigned int i = 0; i <= 360; i++) {
        GLfloat x = x_sun_pos + rad * cos(i * 2.0f * M_PI / 360);
        if (crescent)
            x += 0.05;

        GLfloat y = cy + rad * sin(i * 2.0f * M_PI / 360);

        glVertex2f(x, y);
    }

    glEnd();
}

void ground() {
    if (x_sun_pos >= 0.7 || night || x_sun_pos < -0.8) {
        glColor3f(0, (float) 110 / 255, 0);
    } else {
        glColor3f(0, 154, 0);
    }

    GLfloat verts[] = {
            -1.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, -1.0f,
            -1.0f, -1.0f
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    draw_house();
}

void timer(int= 0) {
    glutPostRedisplay();
    glutTimerFunc(50, timer, 0);

    if (x_sun_pos >= 1.1) {
        x_sun_pos = -1.0;
        if (night)
            night = false;
        else {
            night = true;
            glClearColor(0, 0, 0, 0);
        }
    } else {
        x_sun_pos += 0.03;
    }
}

void changeSize(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    glMatrixMode(GL_PROJECTION);
    // matrix nullification
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    // return to the matrix projection
    glMatrixMode(GL_MODELVIEW);
}
