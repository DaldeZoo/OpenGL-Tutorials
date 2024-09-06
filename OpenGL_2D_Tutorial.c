#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h> // Including OpenGL GLUT library

/*
- ***** A Note on Transformations *****
  - Recall from linear algebra the concept of matrices and linear transformations.
  in OpenGL shapes or primitives are represented with vertices or points.
  These collection of points are what make the shapes. The lines that make up
  the shapes can be thought of a vectors. So when we draw a shape or apply a transformation,
  that is all done with matrices and matrix multiplication (to the ModelView matrix by default).
  - Note: The ModelView matrix is used to transform vertices into shapes and render them.
        Based on its setup (which can be altered with transformations like translation and rotation)
        we display things on the screen. Ie if we use glTranslate() for example we only change the 
        setup of the ModelView matrix and not the shapes or coordinate system.

- ***** A Deeper Dive into the ModelView Matrix *****
  The ModelView matrix is a 4x4 matrix with entries that represent various transformations 'n stuff.
  When we alter "the setup" of the ModelView matrix, as mentioned above, we literally are just
  changing the number values of the vectors of the ModelView matrix.
  - This is how it looks like:
    | m11  m12  m13  m14 |
    | m21  m22  m23  m24 |
    | m31  m32  m33  m34 |
    | m41  m42  m43  m44 |

  Each element (m11, m12, ..., m44) corresponds to a transformation component:
  The first three columns (m11, m21, m31) represent the transformation for the x, y, and z axes, respectively.
  The last column (m41, m42, m43) usually represents translation.
  The last row (m14, m24, m34) usually contains perspective information.
  The element m44 is typically set to 1, but it can be used for other purposes like projective transformations.

  And for rotations in the ModelView matrix, elements m11, m12, m13, m21, m22, and m23 are typically affected.
  OpenGL provides functions like glRotatef() or glRotate() to apply rotations, and these functions internally
  update the appropriate elements of the ModelView matrix to achieve the desired rotation.
  Here's how it works:
  1. Determine the Rotation Axis: You specify the axis of rotation (x, y, or z) and the angle of rotation
  (in degrees) as parameters to the rotation function.
  2. Compute the Rotation Matrix: Internally, OpenGL calculates the rotation matrix based on the specified
  angle and axis of rotation. This matrix is a 3x3 matrix representing the rotation transformation.
  3. Apply the Rotation to the ModelView Matrix: OpenGL then updates the ModelView matrix by multiplying it
  with the rotation matrix. Since the ModelView matrix is 4x4, the rotation matrix is embedded within it
  as a 3x3 submatrix.
  4. Combined Transformation: When you render objects, OpenGL uses the updated ModelView matrix to transform
  the vertices of the objects. This transformation incorporates all previous transformations, including rotations,
  translations, and scalings.
*/

float x_position = -10.0;
int state = 1; // When state is 1 we move right and if state is -1 we move left.
float r = 0;
float g = 0;
float b = 0;
int color_counter = 0;

void display()
{
    // This function is the display callback, called whenever the window needs to be redrawn.
    // It clears the color buffer, resets the coordinate system, and draws the scene.

    // Buffers are basically ovens. They act as temporary storage for data to be processed
    // and rendered before being diplayed on screen.
    // They improve performance and reduce flickering.

    // Clearing them before drawing again is crucial; otherwise, remnants of the previous frame linger.

    glClear(GL_COLOR_BUFFER_BIT); // Clears the color buffer
    glLoadIdentity(); // Resets the coordinate system to its default (GL_MODELVIEW matrix is affected)
    // This also resets the coordinates for the shape being drawn, so each time it's called,
    // the shape is positioned back to its default. For example, if glTranslate changes the position,
    // calling glLoadIdentity() resets it.

    /*
    // In SINGLE BUFFERING MODE (1):
    // Draw your scene here...
    glBegin(GL_TRIANGLES); // Begins drawing triangles
        // Specify vertices for the first triangle
        glVertex2f(0.0, 3.0); // Vertex 1
        glVertex2f(9.0, 6.0); // Vertex 2
        glVertex2f(5.0, 5.0); // Vertex 3

        // Specify vertices for the second triangle
        glVertex2f(5.5, 6.9); // Vertex 1
        glVertex2f(1.0, 3.787); // Vertex 2
        glVertex2f(0.0, 10.0); // Vertex 3
    glEnd(); // Ends drawing triangles

    glBegin(GL_POLYGON); // Begins drawing a polygon
        // Specify vertices for a hexagon
        glVertex2f(0.0, 0.0); // Vertex 1
        glVertex2f(1.0, 0.0); // Vertex 2
        glVertex2f(0.5, 0.866); // Vertex 3
        glVertex2f(-0.5, 0.866); // Vertex 4
        glVertex2f(-1.0, 0.0); // Vertex 5
        glVertex2f(-0.5, -0.866); // Vertex 6
        glVertex2f(0.5, -0.866); // Vertex 7
    glEnd(); // Ends drawing the polygon

    glBegin(GL_POLYGON);
        glVertex2f(x_position, 8.0);
        glVertex2f(x_position+1, 0);
        glVertex2f(x_position+5, -6.0);
        glVertex2f(x_position+10, 1.0);
    glEnd();
    glFlush();

    glFlush(); // Displays the contents of the color buffer
    // needed when using single buffering mode (the defualt)
    */

    // In DOUBLE BUFFERING MODE:
    // Double buffering is crucial for smooth animation. When rendering complex geometry,
    // OpenGL may not finish rendering the entire scene within one refresh cycle.
    // Without double buffering, incomplete frames may be displayed, resulting in 
    // flickering or incomplete visuals as the display advances to the next frame.

    // Double buffering works by having two frame buffers working in tandum, a front and a back buffer.
    // The front buffer displays the rendered frame, all while the back buffer renders the next frame,
    // then we switch the front and back buffers, front is displayed, and back is drawn on and so on...
    // Basically, Display while Redrawing then Swap, Display while Redrawing then Swap... DRS I guess.

    // The only issue (which isnt all too bad tbh) is that if the back buffer frame doesnt render
    // fully like ever, then the frame will basically freeze on the previous frame because
    // double buffering only displays the most recently FULLY completed frame.
    
    // ***********************************************************************************************

    //glTranslatef(5.0, -7.0, 0); // (can NOT be used inside glbegin())
    // Applies a translation to the current matrix, which is by
    // default the modelview matrix (which is used for drawing shapes, transformations, and more).
    // This means that any subsequent drawing operations will be affected by this translation,
    // Unless the current matrix in use is reset with glLoadIdentity.
    // Internally, glTranslatef() multiplies the current matrix by a translation matrix,
    // combining the translation with any previous transformations applied to the matrix.
    
    glColor3f(r, g, b); // Sets the color for primitives (shapes) to be drawn.
    // (CAN be used inside glBegin).
    // Note: In OpenGL, setting a color with functions like glClearColor or glColor3f affects 
    // subsequent drawing operations unless explicitly changed again. If no new color is set, 
    // shapes will be drawn using the last specified color or the default color.

    glBegin(GL_POLYGON);
        glVertex2f(x_position, 4.0);
        glVertex2f(x_position, 0);
        glVertex2f(x_position+3, 0);
        glVertex2f(x_position+3, 4);
    glEnd();
    // Makes a 3-units wide and 4-units long rectangle.

    glutSwapBuffers(); // Swaps the buffers, and automatically does the buffer flush.

}

void reshape(int width, int height)
{
    // This function is called when the window is resized.
    // It adjusts the viewport and projection matrix to match the new window size.

    glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Sets up the viewport
    // It takes 4 arguments: x and y coordinates (bottom-left corner) and width and height of the viewport.

    // In default, the origin (0,0) is in the bottom left corner of the screen and the top right
    // is the screen width and height (in our code (500, 500)).

    // The code below now changes the coordinate system or orthographic projection of the screen
    // to have -10 be the left most x-axis and bottom most y-axis and
    // 10 be the right most x-axis and top most y-axis
    // all while still preserving the actual screen size of (500, 500) in our code.

    glMatrixMode(GL_PROJECTION); // Sets the current matrix mode to projection
    glLoadIdentity(); // Resets parameters of the projection matrix
    gluOrtho2D(-10, 10, -10, 10); // Sets up an orthographic projection
    // gluOrtho2D(left, right, bottom, top) defines a 2D orthographic projection matrix.
    // It maps a specified rectangle to the clipping volume, preserving the aspect ratio.
    // Note: An orthographic projection is typically what is used for 2D because it doesnt
    // show depth. Ie two objects of the same size in different positions along the z-axis
    // will appear to have the same size. [Check orthographic vs. perspective projection online].

    glMatrixMode(GL_MODELVIEW); // Sets the current matrix mode back to modelview

    // We must change back to the modelview matrix because:
    // The modelview matrix is used to transform objects in your scene 
    // (e.g., translation, rotation, scaling). The projection matrix is used to set up
    // the camera's perspective (e.g., perspective or orthographic projection).
    // By switching back to the modelview matrix mode after setting up the projection matrix,
    // you ensure that subsequent transformations affect the objects in your scene and 
    // not the camera's perspective.
}

void timer(int i)
{
    // This is basically a loop function that is needed for the animation.
    // It continuosly or periodically calls itself every 1/60th of a second
    // ie 60 frames per seconds.

    glutPostRedisplay(); // Signals the window to be redrawn in the next frame. Along with glClear()
    // they work hand in hand to clear the screen and prepare for rendering.

    glutTimerFunc(1000/60, timer, 0); // Registers the timer again to call this function after
    // approximately 1/60th of a second (assuming a 60 frames per second refresh rate).
    // We wont be needing to use the 3rd paramater (explained in main).

    /* Stops at the end as our shape is 3 units wide and max x-coordinate is 10
    if (x_position < 7)
    {
        x_position += 0.15;
    }
    */

    if (color_counter < 10)
    {
        r = 1;
        g = 0;
        b = 0;
    }
    else if (color_counter < 20)
    {
        r = 0;
        g = 1;
        b = 0;
    }
    else
    {
        r = 0;
        g = 0;
        b = 1;
    }
    color_counter++;
    if (color_counter > 30) color_counter = 0;

    switch (state)
    {
        case 1:
        if (x_position < 7) x_position += 0.30;
        else
        {
            state = -1;
        }
        break;

        case -1:
        if (x_position > -10) x_position -= 0.30;
        else
        {
            state = 1;
        }
        break;
    }

    /* With Translation
    glTranslatef(x_position, 0, 0);
    switch (state)
    {
        case 1:
        if (x_position < 7) x_position += 0.30;
        else
        {
            state = -1;
        }
        break;

        case -1:
        if (x_position > -10) x_position -= 0.30;
        else
        {
            state = 1;
        }
        break;
    }
    */
}


void init()
{
    // This function initializes OpenGL state.
    // It sets the clear color (background color) for the color buffer.

    glClearColor(0.3, 0.3, 0.3, 0); // Sets the background color.
    // CHECK display function for details about colors in OpenGL.
}

int main(int argc, char** argv)
{

    glutInit(&argc, argv); // Initializes the GLUT library and processes any command line arguments.

    // To execute SINGLE BUFFER version of code (just drawing shapes) remove "GLUT_DOUBLE" below.
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // Sets the initial display mode to use RGB color model.
    // ALSO to Double Buffering mode (Explained in display function)

    glutInitWindowPosition(0, 0); // Sets the initial position of the window (in pixels).
    glutInitWindowSize(500, 500); // Sets the initial size of the window (in pixels).
    // Note: this makes the window size to be 500 pixels wide and 500 pixels long, ie it will contain
    // 500*500 = 250,000 pixels in total (ie will cover 250,000 pixels of the screen).
    // If we draw a square that starts at let's say x = -10 to x = 0 and y = 0 to y = 4,
    // it will be 10 pixels wide and 4 pixels long ie will contain 10*4 = 40 pixels in total.

    glutCreateWindow("Daldezo's Square 1"); // Creates a window with the given title.

    // Callback functions are functions that are passed as arguments to other functions
    // Note: functions are reduced to pointers after being passed as arguments in some function.

    glutDisplayFunc(display); // Registers the display callback function.
    glutReshapeFunc(reshape); // Registers the reshape callback function.
    glutTimerFunc(0, timer, 0); // Registers the timer callback function.
    // Takes three arguments: 
    // 1. unsigned int millisec (time interval for callback function to be called, in milliseconds),
    // 2. pointer to the callback function,
    // 3. integer value representing user-defined data to be passed to the callback function -
    // (This can be used for example as a straight-up timer on screen - you start at 0 and every
    // second you add 1 and print to the screen).


    init(); // Calls the initialization function to set up OpenGL state.

    glutMainLoop(); // Enters the GLUT event processing loop.
    return 0;
}
