#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_CHILD 5
#define MAX_NAME 64

typedef struct tree_element {
    char name[MAX_NAME];
    int ftype;             // 1 = directory, 2 = file
    int nc;                // number of children
    struct tree_element *link[MAX_CHILD];

    // Visual / interactive properties
    float x, y;            // current position (for animation)
    float tx, ty;          // target position
    bool visible;          // whether drawn (in hierarchy)
    bool expanded;         // directory expanded?
} node;

node *root = NULL;

/* Camera */
float camX = 0.0f, camY = 0.0f;
float zoom = 1.0f;
int lastMouseX = 0, lastMouseY = 0;
bool panning = false;

/* Interaction */
node *hoverNode = NULL;

/* Animation */
const float ANIM_SPEED = 8.0f; // larger = snappier

/* Window */
int WIN_W = 1000, WIN_H = 700;

/* ----- Utilities ----- */

void hide_subtree(node *r)
{
    if (!r) return;
    r->visible = false;
    for (int j = 0; j < r->nc; j++)
        hide_subtree(r->link[j]);
}

void interp(node *n, float factor)
{
    if (!n) return;
    n->x += (n->tx - n->x) * factor;
    n->y += (n->ty - n->y) * factor;

    for (int i = 0; i < n->nc; i++)
        interp(n->link[i], factor);
}

float clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }

void screenToWorld(int sx, int sy, float *wx, float *wy) {
    // Convert screen coords (sx,sy with origin top-left) to world coords
    float nx = (float)sx;
    float ny = (float)(WIN_H - sy); // invert y to match gl coords
    *wx = (nx - WIN_W / 2.0f) / zoom - camX;
    *wy = (ny - WIN_H / 2.0f) / zoom - camY;
}

/* ----- Draw helpers ----- */

void drawText(float x, float y, const char *text) {
    glRasterPos2f(x, y);
    for (const char *p = text; *p; ++p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
}

void drawCircle(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 48; i++) {
        float a = (float)i / 48.0f * 2.0f * M_PI;
        glVertex2f(cx + cosf(a) * r, cy + sinf(a) * r);
    }
    glEnd();
}

void drawRoundedRect(float x1, float y1, float x2, float y2, float r) {
    // Simple rounded rect using many vertices around corners
    glBegin(GL_POLYGON);
    const int STEPS = 18;
    // top-right corner
    for (int i = 0; i <= STEPS; ++i) {
        float a = (float)i / STEPS * M_PI_2;
        glVertex2f(x2 - r + cosf(a) * r, y2 - r + sinf(a) * r);
    }
    // top-left
    for (int i = 0; i <= STEPS; ++i) {
        float a = M_PI_2 + (float)i / STEPS * M_PI_2;
        glVertex2f(x1 + r + cosf(a) * r, y2 - r + sinf(a) * r);
    }
    // bottom-left
    for (int i = 0; i <= STEPS; ++i) {
        float a = M_PI + (float)i / STEPS * M_PI_2;
        glVertex2f(x1 + r + cosf(a) * r, y1 + r + sinf(a) * r);
    }
    // bottom-right
    for (int i = 0; i <= STEPS; ++i) {
        float a = -M_PI_2 + (float)i / STEPS * M_PI_2;
        glVertex2f(x2 - r + cosf(a) * r, y1 + r + sinf(a) * r);
    }
    glEnd();
}

/* ----- Tree creation (console input) ----- */

node* create_node_interactive(int level) {
    node *n = (node*) malloc(sizeof(node));
    if (!n) { fprintf(stderr,"malloc failed\n"); exit(1); }
    memset(n,0,sizeof(node));
    printf("\nEnter name for level %d: ", level);
    if (scanf("%63s", n->name) != 1) strcpy(n->name, "unnamed");
    printf("Is \"%s\" a directory (1) or file (2)? ", n->name);
    int t = 1;
    if (scanf("%d", &t) != 1) t = 1;
    n->ftype = (t == 2) ? 2 : 1;
    if (n->ftype == 1) {
        printf("How many children for %s (0-%d)? ", n->name, MAX_CHILD);
        if (scanf("%d", &n->nc) != 1) n->nc = 0;
        if (n->nc < 0) n->nc = 0;
        if (n->nc > MAX_CHILD) n->nc = MAX_CHILD;
    } else {
        n->nc = 0;
    }
    for (int i=0;i<MAX_CHILD;i++) n->link[i] = NULL;
    n->visible = true;
    n->expanded = false;
    for (int i=0;i<n->nc;i++) {
        n->link[i] = create_node_interactive(level+1);
        // child initial positions will be set later by layout
    }
    return n;
}

/* ----- Layout ----- */

void layout_node(node *n, float x, float y) {
    if (!n) return;
    // set target position for this node
    n->tx = x; n->ty = y;
    // For directories that are expanded, layout children horizontally beneath
    if (n->ftype == 1 && n->nc > 0 && n->expanded) {
        float totalWidth = (n->nc) * 220.0f; // spacing
        float startX = x - totalWidth/2.0f + 110.0f;
        float childY = y - 140.0f;
        for (int i=0;i<n->nc;i++) {
            node *c = n->link[i];
            if (c) {
                c->visible = true;
                layout_node(c, startX + i * 220.0f, childY);
            }
        }
    } else {
        // if collapsed, hide descendants
        for (int i=0;i<n->nc;i++) {
            node *c = n->link[i];
            if (c) {
                // hide subtree
                c->visible = false;
                // recursively hide deeper nodes
                // use simple stack recursion
                // define helper:
                hide_subtree(c);
            }
        }
    }
}

/* recompute layout from root (centered) */
void recompute_layout() {
    if (!root) return;
    layout_node(root, 0.0f, 260.0f); // root in world coords (0,260)
}

/* ----- Picking ----- */

bool point_in_circle(float px, float py, float cx, float cy, float r) {
    float dx = px - cx, dy = py - cy;
    return (dx*dx + dy*dy) <= r*r;
}
bool point_in_rect(float px, float py, float x1, float y1, float x2, float y2) {
    return (px >= x1 && px <= x2 && py >= y1 && py <= y2);
}

node* pick_node_recursive(node *n, float wx, float wy) {
    if (!n || !n->visible) return NULL;
    // check children first (so top-most are picked)
    for (int i=0;i<n->nc;i++) {
        node *res = pick_node_recursive(n->link[i], wx, wy);
        if (res) return res;
    }
    if (n->ftype == 1) {
        // rounded rect approx => use rectangle bounds
        float w = 120.0f, h = 40.0f;
        if (point_in_rect(wx, wy, n->x - w, n->y - h, n->x + w, n->y + h)) return n;
    } else {
        if (point_in_circle(wx, wy, n->x, n->y, 38.0f)) return n;
    }
    return NULL;
}

/* ----- Draw tree ----- */

void drawBackground() {
    glBegin(GL_QUADS);
      glColor3f(0.95f, 0.98f, 1.0f); glVertex2f(-WIN_W/2.0f, WIN_H/2.0f);
      glColor3f(0.78f, 0.90f, 0.96f); glVertex2f(WIN_W/2.0f, WIN_H/2.0f);
      glColor3f(0.70f, 0.86f, 0.94f); glVertex2f(WIN_W/2.0f, -WIN_H/2.0f);
      glColor3f(0.88f, 0.96f, 1.0f); glVertex2f(-WIN_W/2.0f, -WIN_H/2.0f);
    glEnd();
}

void draw_node(node *n) {
    if (!n || !n->visible) return;
    // connectors to children
    glLineWidth(2.0f);
    glColor3f(0.18f,0.18f,0.22f);
    for (int i=0;i<n->nc;i++) {
        node *c = n->link[i];
        if (c && c->visible) {
            glBegin(GL_LINES);
                glVertex2f(n->x, n->y - 10.0f);
                glVertex2f(c->x, c->y + 40.0f);
            glEnd();
        }
    }
    // shadow
    if (n->ftype == 1) {
        glColor4f(0,0,0,0.12f);
        drawRoundedRect(n->x - 62.0f, n->y - 22.0f - 4.0f, n->x + 62.0f, n->y + 22.0f - 4.0f, 10.0f);
    } else {
        glColor4f(0,0,0,0.12f);
        drawCircle(n->x + 4.0f, n->y - 4.0f, 42.0f);
    }
    // node body
    if (n->ftype == 1) {
        glColor3f(0.96f,0.76f,0.96f); // folder color
        drawRoundedRect(n->x - 60.0f, n->y - 20.0f, n->x + 60.0f, n->y + 20.0f, 10.0f);
    } else {
        glColor3f(0.45f,0.65f,1.0f);
        drawCircle(n->x, n->y, 40.0f);
    }
    // outline
    glColor3f(0.12f,0.10f,0.16f);
    if (n->ftype == 1) {
        // outline rounded rect: reuse polygon approx (draw thin)
        glLineWidth(1.5f);
        // draw same shape as rounded rect but as line loop
        glBegin(GL_LINE_LOOP);
        const int STEPS = 32;
        for (int i = 0; i <= STEPS; ++i) {
            float a = (float)i / STEPS * 2.0f * M_PI;
            float rx = 60.0f * cosf(a);
            float ry = 20.0f * sinf(a);
            // warp rx/ry to approximate rounded rect look:
            // not perfect but acceptable
            glVertex2f(n->x + rx, n->y + ry);
        }
        glEnd();
    } else {
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            for (int i=0;i<=48;i++){
                float a = (float)i/48.0f*2.0f*M_PI;
                glVertex2f(n->x + cosf(a)*40.0f, n->y + sinf(a)*40.0f);
            }
        glEnd();
    }
    // highlight if hover
    if (hoverNode == n) {
        glColor4f(1.0f, 1.0f, 1.0f, 0.16f);
        if (n->ftype == 1) drawRoundedRect(n->x - 60.0f, n->y - 20.0f, n->x + 60.0f, n->y + 20.0f, 10.0f);
        else drawCircle(n->x, n->y, 40.0f);
    }
    // text
    glColor3f(0.08f,0.06f,0.12f);
    drawText(n->x - (float)strlen(n->name)*3.5f, n->y - 4.0f, n->name);
}

void draw_tree_recursive(node *n) {
    if (!n || !n->visible) return;
    // draw children first (so connectors and nodes overlay nicely)
    for (int i=0;i<n->nc;i++) draw_tree_recursive(n->link[i]);
    draw_node(n);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // camera transform
    glPushMatrix();
    glTranslatef(WIN_W/2.0f, WIN_H/2.0f, 0);
    glScalef(zoom, zoom, 1.0f);
    glTranslatef(camX, camY, 0);

    drawBackground();
    if (root) draw_tree_recursive(root);

    glPopMatrix();

    glutSwapBuffers();
}

/* ----- Animation tick ----- */

void animate_step(float dt) {
    if (!root) return;
    // simple recursive interpolation towards target positions  
    float f = clampf(dt * ANIM_SPEED, 0.0f, 1.0f);
    interp(root, f);
}

/* ----- Input callbacks ----- */

int lastTime = 0;
void idle_func() {
    int t = glutGet(GLUT_ELAPSED_TIME);
    int dt_ms = t - lastTime;
    if (lastTime == 0) dt_ms = 16;
    lastTime = t;
    animate_step(dt_ms / 1000.0f);
    glutPostRedisplay();
}

void mouse_wheel(int wheel, int dir, int x, int y) {
    // freeglut provides wheel callback: dir = +/-1
    float oldZoom = zoom;
    if (dir > 0) zoom *= 1.12f;
    else zoom *= 0.88f;
    zoom = clampf(zoom, 0.2f, 3.5f);

    // zoom to cursor position: adjust camX/camY so that world pos under cursor remains under cursor
    float wx_before, wy_before, wx_after, wy_after;
    screenToWorld(x, y, &wx_before, &wy_before);
    // apply new zoom temporarily to compute after:
    float savedZoom = zoom;
    // compute world after with new zoom (we already updated zoom)
    screenToWorld(x, y, &wx_after, &wy_after);
    // adjust camera by delta
    camX += (wx_after - wx_before);
    camY += (wy_after - wy_before);
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            lastMouseX = x; lastMouseY = y;
            panning = true;
        } else {
            // mouse up -> interpret as click (if small drag)
            if (panning) {
                // handle pick if not actually panning (we'll detect small movement)
                int dx = x - lastMouseX;
                int dy = y - lastMouseY;
                float threshold = 4.0f;
                if (abs(dx) < threshold && abs(dy) < threshold) {
                    float wx, wy;
                    screenToWorld(x, y, &wx, &wy);
                    if (root) {
                        node *picked = pick_node_recursive(root, wx, wy);
                        if (picked) {
                            if (picked->ftype == 1) {
                                // toggle expanded
                                picked->expanded = !picked->expanded;
                                recompute_layout();
                            }
                        }
                    }
                }
            }
            panning = false;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        // could be used later
    } else if (button == 3 || button == 4) {
        // some GLUT implementations map wheel to button 3/4 in mouse function
        int dir = (button == 3) ? 1 : -1;
        mouse_wheel(0, dir, x, y);
    }
}

void mouseMotion(int x, int y) {
    if (panning) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;
        // convert screen delta to world delta
        camX += (float)dx / zoom / 1.0f;
        camY -= (float)dy / zoom / 1.0f; // subtract because screen y inverted
        lastMouseX = x; lastMouseY = y;
    }
    // hover detection
    float wx, wy;
    screenToWorld(x, y, &wx, &wy);
    node *newHover = NULL;
    if (root) newHover = pick_node_recursive(root, wx, wy);
    hoverNode = newHover;
}

/* freeglut wheel interface wrapper */
void wheel_wrapper(int wheel, int dir, int x, int y) {
    mouse_wheel(wheel, dir, x, y);
}

/* ----- Initialization ----- */

void init_positions_recursive(node *n) {
    if (!n) return;
    // initialize x,y to target for a snappy start
    n->x = n->tx; n->y = n->ty;
    for (int i=0;i<n->nc;i++) init_positions_recursive(n->link[i]);
}

int main(int argc, char **argv) {
    printf("=== Interactive Classic Tree Creator ===\n");
    printf("We will build the tree from console input.\n");
    // Create root node
    printf("\nRoot (level 0):\n");
    root = create_node_interactive(0);
    // Put root at center
    root->tx = 0.0f; root->ty = 260.0f;
    root->x = root->tx; root->y = root->ty;
    // Initially collapsed children hidden
    recompute_layout();
    init_positions_recursive(root);

    // GLUT init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Classic Interactive Directory Tree");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);

    // Set up callbacks
    glutDisplayFunc(display);
    glutIdleFunc(idle_func);
    glutMouseFunc(mouseButton);
    // freeglut has glutMouseWheelFunc; if not present, wheel may come as mouse buttons 3/4
#ifdef GLUT_MOUSEWHEEL
    glutMouseWheelFunc(wheel_wrapper);
#endif
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mouseMotion);

    // enable blending for shadows/highlight
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    printf("\nControls:\n - Left-click on a directory to toggle expand/collapse\n - Left-drag to pan\n - Mouse wheel to zoom\n - Hover highlights nodes\n\n");

    lastTime = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();
    return 0;
}
