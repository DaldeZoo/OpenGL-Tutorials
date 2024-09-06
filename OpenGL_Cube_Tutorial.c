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

- ***** 3D Perspective Projection *****
  So basically in OpenGL (and almost every other game engine), 3D perspective is achieved with
  something called a View Frustum. This view frustum is region of space that the viewer/player/camera
  can see in our 3D environment (ie R^3 -> we have (x,y,z)), its where objects are rendered and can 
  be visible on screen (Any object not in the view frustum is not rendered to improve efficieny -
  they'll be either unseen or clipped if part of 'em is on screen).
  Note: Basically when a player moves his camera in a 3D environment he's basically moving the view frustum.

  The view frustum works similar to the actual human eye or camera:
    It is defined by six planes: left, right, bottom, top, near, and far.
    These planes create a truncated pyramid shape, with the apex at the camera position
    and the base extending outward into the scene.
    - The near plane is the closest plane to the camera and represents the minimum distance
      at which objects are rendered.
    - The far plane is the farthest plane from the camera and represents the maximum distance
      at which objects are rendered.
    - The left, right, bottom, and top planes define the sides of the frustum and
      determine its width and height.

  Here is how I'd explain how we SEE things on our screens:
    (There are cool videos on YouTube that explain Perspective projection and how display works)
    So imagine the camera as cyclops from x-men's eye which shoots lazers or rays of light
    through the neaar plane and onto the far plane, all the region between the near and far planes
    are our world. The far plane is like a mirror that reflects the rays into the objects inside
    and onto the near plane, basically making the near plane our actual irl window or screen.
    (It's as the game world actually exists behind the screen lol).

  Note: The camera is by defualt in the origin (0,0,0) and points towards the negative z-axis,
        effectively making the right direction +x, left direction -x, upwards +y, and downwards -y.
        (check 3D graph viewer if need be).
*/

float g_angle = 0;

void display()
{
    // This function is the display callback, called whenever the window needs to be redrawn.
    // It clears the color buffer, resets the coordinate system, and draws the scene.

    // Buffers are basically ovens. They act as temporary storage for data to be processed
    // and rendered before being diplayed on screen.
    // They improve performance and reduce flickering.

    // Clearing them before drawing again is crucial; otherwise, remnants of the previous frame linger.

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the color and depth buffers
    // This is essential because, in each frame, data about colors and depth are stored.
    // Unless we clear them before redrawing in the next frame, previous frame data
    // will persist or mix with the new frame data, leading to undesired results.

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
    
    // glColor3f(r, g, b); // Sets the color for primitives (shapes) to be drawn.
    // (CAN be used inside glBegin).
    // Note: In OpenGL, setting a color with functions like glClearColor or glColor3f affects 
    // subsequent drawing operations unless explicitly changed again. If no new color is set, 
    // shapes will be drawn using the last specified color or the default color.

    glTranslatef(0,0,-9); // Parameters: (x,y,z)

    // - Not visible at z = 0 because thats behind the Near clipping plane as defined in glBegin()
    // - Note: We have set the zNear plane to 2 in the reshape callback function,
    //         ie the square will be closest to the screen at z = -5 or (0,0,-5).
    // - Remember: we move in the NEGATIVE z-axis so we just translate it.

    // The rotation convention in OpenGL is with respect to the "Right Hand Rule".
    // RHR: Point your thumb towards your face, curl your fingers, and boom, thats the positive
    //      angle of rotation. Ie positive angle -> counterclockwise rotation.
    //                            negative angle -> clockwise rotation.

    // Parameters:
    // 1. Angle of Rotation: The rotation convention in OpenGL follows the "Right Hand Rule".
    //    According to this rule, a positive angle of rotation results in counterclockwise rotation,
    //    while a negative angle leads to clockwise rotation.
    // 2,3,4. Axis of Rotation (x,y,z): These parameters specify the axis of rotation. For example:
    //    - glRotatef(45, 1, 0, 0) rotates around the x-axis.
    //    - glRotatef(45, 0, 1, 0) rotates around the y-axis.
    //    - glRotatef(45, 0, 0, 1) rotates around the z-axis.
    //    - glRotatef(45, 1, 1, 0) rotates around both the x-axis and y-axis.
    // Note: The values (x,y,z) only determine the direction of the axis of rotation, not its magnitude.
    // Therefore, glRotatef(45, 1, 0, 0) is equivalent to glRotatef(45, 2, 0, 0)
    // as both represent the x-axis direction.

    glRotatef(g_angle, 0, 1, 1);
    glRotatef(g_angle, 1, 1, 0);
    glRotatef(g_angle, 0, 1, 0);
    glRotatef(g_angle, 1, 0, 1);   
    glRotatef(g_angle, 1, 1, 1);
    glRotatef(g_angle, 1, 0, 0);
    glRotatef(g_angle, 0, 0, 1);


    glBegin(GL_QUADS); // To make a cube use GL_Quads
    // Note: the front, right, and top faces must be defined in a counterclockwise order.
    //       while the back, left, and bottom faces must be defined in a clockwise order.
        // Front
        glColor3f(0.8,0.2,0.6);
        glVertex3f(-2, 2, 0);
        glVertex3f(-2, -2, 0);
        glVertex3f(2, -2, 0);
        glVertex3f(2, 2, 0);

        // Back
        glColor3f(0.8,0.7,0.2);
        glVertex3f(-2, 2, -4);
        glVertex3f(2, 2, -4);
        glVertex3f(2, -2, -4);
        glVertex3f(-2, -2, -4);

        // Top
        glColor3f(0.3,0.8,0.2);
        glVertex3f(-2, 2, -4);
        glVertex3f(-2, 2, 0);
        glVertex3f(2, 2, 0);
        glVertex3f(2, 2, -4);

        // Bottom
        glColor3f(0.4,0.8,0.5);
        glVertex3f(-2, -2, -4);
        glVertex3f(2, -2, -4);
        glVertex3f(2, -2, 0);
        glVertex3f(-2, -2, 0);

        // Right
        glColor3f(0.4,0.5,0.8);
        glVertex3f(2, 2, 0);
        glVertex3f(2, -2, 0);
        glVertex3f(2, -2, -4);
        glVertex3f(2, 2, -4);

        // Left
        glColor3f(0.5,0.3,0.7);
        glVertex3f(-2, 2, 0);
        glVertex3f(-2, 2, -4);
        glVertex3f(-2, -2, -4);
        glVertex3f(-2, -2, 0);
    glEnd();
/*
    // Not my so not very good cube:
    glColor3f(1.0,0.0,0.0);
    glVertex3f(-1.0,1.0,1.0);
    glVertex3f(-1.0,-1.0,1.0);
    glVertex3f(1.0,-1.0,1.0);
    glVertex3f(1.0,1.0,1.0);
    //back
    glColor3f(0.0,1.0,0.0);
    glVertex3f(1.0,1.0,-1.0);
    glVertex3f(1.0,-1.0,-1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(-1.0,1.0,-1.0);
    //right
    glColor3f(0.0,0.0,1.0);
    glVertex3f(1.0,1.0,1.0);
    glVertex3f(1.0,-1.0,1.0);
    glVertex3f(1.0,-1.0,-1.0);
    glVertex3f(1.0,1.0,-1.0);
    //left
    glColor3f(1.0,1.0,0.0);
    glVertex3f(-1.0,1.0,-1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(-1.0,-1.0,1.0);
    glVertex3f(-1.0,1.0,1.0);
    //top
    glColor3f(0.0,1.0,1.0);
    glVertex3f(-1.0,1.0,-1.0);
    glVertex3f(-1.0,1.0,1.0);
    glVertex3f(1.0,1.0,1.0);
    glVertex3f(1.0,1.0,-1.0);
    //bottom
    glColor3f(1.0,0.0,1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(-1.0,-1.0,1.0);
    glVertex3f(1.0,-1.0,1.0);
    glVertex3f(1.0,-1.0,-1.0);

    glEnd();
*/
    // Makes a 3D cube.
    // Note: In OpenGL, without depth testing, a square drawn after another square at a different
    //       depth will simply overwrite the previous one. Therefore, we enable depth testing
    //       mode during initialization, clear the depth buffer to ensure proper rendering order,
    //       and include

    glutSwapBuffers(); // Swaps the buffers, and automatically does the buffer flush.

}

void reshape(int width, int height)
{
    // This function is called when the window is resized.
    // It adjusts the viewport and projection matrix to match the new window size.

    glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Sets up the viewport ie 
    // (where the rendered image is displayed).
    // It takes 4 arguments: x and y coordinates (bottom-left corner) and width and height of the viewport.

    // In default, the origin (0,0) is in the bottom left corner of the screen and the top right
    // is the screen width and height (in our code (500, 500)).

    // The code below now changes the coordinate system or orthographic projection of the screen
    // to have -10 be the left most x-axis and bottom most y-axis and
    // 10 be the right most x-axis and top most y-axis
    // all while still preserving the actual screen size of (500, 500) in our code.

    glMatrixMode(GL_PROJECTION); // Sets the current matrix mode to projection
    glLoadIdentity(); // Resets parameters of the projection matrix

    gluPerspective(60.0, 1.0, 2.0, 50.0); // Sets up the perspective projection matrix which
    // defines how 3D objects are projected onto a 2D screen.
    // Basically make the view frustum (explained in the top section under header files)
    // ie configures projection mode to perspective projection.

    // The paramaters are as follows:
    // 1. FOV angle (in degrees): Determines the FOV (field of view or how much of the screen is visible).
    //    A higher angle means more can be viewed and less means less (basically zoom-in and zoom-out).
    //    Imagine the camera as a vertex with two equally angled lines coming out of it kinda like "<" shape.
    //    Theoretically it can be set to any value, but 30-90 degrees is the usual (test things out).
    //    Note this only controls the vertical fov, what controls the horizontal fov is the aspect ratio.
    // 2. Aspect Ratio: The ratio of the width by the height of the screen ie width/height.
    //    It determines the horizontal stretch or shrink of the view ie the horizontal fov (test things out).
    //    Note that if the aspect ratio doesnt match the aspect ratio of our window, it leads to distortions.
    // 3. zNear or Near Clip Plane: Defines the (positive) distance of the Near plane from the camera.
    //    Objects closer to the camera than this distance will be clipped.
    //    Setting the near clipping plane (zNear) very close to the camera can introduce several issues:
    // ---> Z-fighting: Surfaces close to each other or the near clipping plane may flicker or fight
    //      for visibility due to depth precision errors.
    // ---> Floating-Point Precision: OpenGL's depth buffer uses floating-point numbers, and setting zNear
    //      too close can lead to depth precision loss and rendering artifacts.
    // ---> Visual Artifacts: Objects near the camera may appear distorted or incorrectly rendered due
    //      to limited depth precision and depth buffer resolution.
    // ---> Performance Impact: Rendering objects close to the camera requires more computational resources,
    //      impacting performance, especially in complex scenes or on limited hardware.
    //    Therefore, it's advisable to set zNear to a reasonable distance from the camera to avoid these issues.
    // 4. zFar or Far Clip Plane: Defines the (positive) distance of the Far plane from the camera.
    //    Objects beyond this distance will be clipped.
    //    Avoiding a very large zFar value prevents depth precision issues
    //    and reduces memory and processing overhead:
    // ---> Depth Buffer: Large zFar may lead to depth-fighting artifacts.
    // ---> Efficiency: Reduces memory and processing for better performance.
    // ---> Clipping: Optimizes culling and clipping for faster rendering.
    //    Set zFar close to the farthest visible object for quality and efficiency.

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

    if (g_angle > 360) g_angle = g_angle - 360;
    g_angle += 0.8;


}


void init()
{
    // This function initializes OpenGL state.
    // It sets the clear color (background color) for the color buffer.

    glClearColor(0.3, 0.4, 0.5, 0); // Sets the background color.
    // CHECK display function for details about colors in OpenGL.

    glEnable(GL_DEPTH_TEST); // Enables depth testing.
    // Depth testing is crucial for 3D environments in OpenGL.
    // It determines which objects should be visible based on their distance from the viewer.
    // Note: For efficieny, objects that are behind other objects ie wont be seen or not fully
    //       are either not rendered or clipped.
}

int main(int argc, char** argv)
{

    glutInit(&argc, argv); // Initializes the GLUT library and processes any command line arguments.

    // To execute SINGLE BUFFER version of code (just drawing shapes) remove "GLUT_DOUBLE" below.
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    // Sets the initial display mode to use RGB color model.
    // ALSO to Double Buffering mode (Explained in display function).
    // ALSO to Depth Buffering mode (for 3D shapes, needed for depth testing).

    glutInitWindowPosition(500, 200); // Sets the initial position of the window (in pixels).
    glutInitWindowSize(500, 500); // Sets the initial size of the window (in pixels).
    // Note: this makes the window size to be 500 pixels wide and 500 pixels long, ie it will contain
    // 500*500 = 250,000 pixels in total (ie will cover 250,000 pixels of the screen).
    // If we draw a square that starts at let's say x = -10 to x = 0 and y = 0 to y = 4,
    // it will be 10 pixels wide and 4 pixels long ie will contain 10*4 = 40 pixels in total.

    glutCreateWindow("Daldezo's Cube"); // Creates a window with the given title.

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
