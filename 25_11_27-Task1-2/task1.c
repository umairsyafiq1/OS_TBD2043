#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>

int countFiles;
char fileNames[10][20];

void drawText(float x, float y, const char *text)
{
    glRasterPos2f(x, y);
    while (*text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
}

// Draw a rounded rectangle -----------------------------
void drawRoundedRect(float x1, float y1, float x2, float y2, float r)
{
    float angle;
    glBegin(GL_POLYGON);
    for (angle = 0; angle <= 360; angle += 5)
    {
        float rad = angle * 3.1416 / 180.0;
        float cx = (angle < 90) ? x2 - r : (angle < 180) ? x1 + r : (angle < 270) ? x1 + r : x2 - r;
        float cy = (angle < 90) ? y2 - r : (angle < 180) ? y2 - r : (angle < 270) ? y1 + r : y1 + r;

        glVertex2f(cx + r * cos(rad), cy + r * sin(rad));
    }
    glEnd();
}

// Background gradient ----------------------------------
void drawGradientBackground()
{
    glBegin(GL_QUADS);
        glColor3f(0.85, 0.95, 1.0);   // top light blue
        glVertex2f(0, 600);
        glVertex2f(800, 600);

        glColor3f(0.70, 0.85, 0.90);   // bottom darker blue
        glVertex2f(800, 0);
        glVertex2f(0, 0);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawGradientBackground();

    // Shadow behind root box
    glColor4f(0, 0, 0, 0.25);
    drawRoundedRect(360, 470, 570, 550, 15);

    // Root directory box with rounded edges
    glColor3f(0.95, 0.75, 0.95);  // soft magenta
    drawRoundedRect(350, 480, 560, 540, 15);

    glColor3f(0.2, 0.1, 0.3);
    drawText(360, 505, "ROOT DIRECTORY");

    // Modern file nodes -------------------------
    float spacing = 800.0f / (countFiles + 1);
    float x = spacing;
    float y = 280;

    for (int i = 0; i < countFiles; i++)
    {
        // Node shadow
        glColor4f(0, 0, 0, 0.25);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x + 4, y - 4);
            for (int k = 0; k <= 50; k++)
            {
                float angle = k * 2.0f * 3.14159f / 50;
                glVertex2f(x + 4 + cosf(angle) * 42, y - 4 + sinf(angle) * 42);
            }
        glEnd();

        // Connector line
        glColor3f(0.1, 0.1, 0.2);
        glLineWidth(2);
        glBegin(GL_LINES);
            glVertex2f(395, 480);
            glVertex2f(x, y + 42);
        glEnd();

        // Node circle
        glColor3f(0.45, 0.65, 1.0); // soft blue
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y);
            for (int k = 0; k <= 50; k++)
            {
                float angle = k * 2.0f * 3.14159f / 50;
                glVertex2f(x + cosf(angle) * 40, y + sinf(angle) * 40);
            }
        glEnd();

        // Node outline
        glColor3f(0.1, 0.1, 0.2);
        glBegin(GL_LINE_LOOP);
            for (int k = 0; k <= 50; k++)
            {
                float angle = k * 2.0f * 3.14159f / 50;
                glVertex2f(x + cosf(angle) * 40, y + sinf(angle) * 40);
            }
        glEnd();

        glColor3f(0.05, 0.05, 0.1);
        drawText(x - 25, y - 5, fileNames[i]);

        x += spacing;
    }

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    printf("Enter number of files: ");
    scanf("%d", &countFiles);

    for (int i = 0; i < countFiles; i++)
    {
        printf("Enter file %d name: ", i + 1);
        scanf("%s", fileNames[i]);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Modern Directory Visualization (C)");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);

    glClearColor(1, 1, 1, 1);

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
