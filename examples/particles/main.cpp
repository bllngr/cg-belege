//////////////////////////////////////////////////////////////////////////
// Sources: http://openglbook.com/
//////////  http://www.dhpoware.com/demos/glObjViewer.html
//////////  http://www.arcsynthesis.org/gltut/
/////////////////////////////////////////////////////////////////////////

/*
 * Keyboard interaction:
 * w, a, s, d  : translate camera
 * SPACE       : pause
 * r           : reset camera
 * ESC, q      : quit
 *
 */

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <gloostMath.h>
#include <gloostGlUtil.h>
#include <Shader.h>
#include <Particles.h>

// include gloost::Mesh which is a geometry container
#include <Mesh.h>
gloost::Mesh* mesh = 0;

Particles particles(1000);

// loader for the wavefront *.obj file format
#include <ObjLoader.h>

#include <Particles.h>


int CurrentWidth  = 800;
int CurrentHeight = 600;
int WindowHandle  = 0;
int timeBefore    = 0;

int MovementOriginX = -1;
int MovementOriginY = -1;

gloost::Vector3 cameraTranslation(0.0f, 0.0f, 8.0f);
gloost::Vector3 cameraRotation(0.0f, 0.0f, 0.0f);

unsigned FrameCount = 0;

unsigned ProjectionMatrixUniformLocation = 0;
unsigned ModelViewMatrixUniformLocation  = 0;
unsigned NormalMatrixUniformLocation     = 0;
unsigned ObjectColorUniformLocation      = 0;

unsigned BufferIds[6] = { 0u };
unsigned ShaderIds[3] = { 0u };

float rotationOffset = 0.0f;

bool paused = false;

enum CameraManipulation {CAM_TRANSLATE, CAM_ROTATE};

// the three different matrices for projection, viewing and model transforming
#include <Matrix.h>
gloost::Matrix ProjectionMatrix;

#include <MatrixStack.h>
gloost::MatrixStack ModelViewMatrixStack;

// Function callbacks
void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void IdleFunction(void);
void TimerFunction(int);
void KeyboardFunction(unsigned char, int, int);
void MouseFunction(int, int, int, int);
void MotionFunction(int, int);
void MoveCamera(CameraManipulation, float, float, float);
void Cleanup(void);
void LoadModel(void);
void SetupShader();
void Draw(void);
void RenderFunction(void);


/////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{

    Initialize(argc, argv);

    glutMainLoop();

    exit(EXIT_SUCCESS);
}

    gloost::Matrix cameraTransform;

/////////////////////////////////////////////////////////////////////////////////////////

// called every frame this functions draw
void Draw(void)
{
    int timeNow = glutGet(GLUT_ELAPSED_TIME);

    // Calculate the rotation, based on number of ms since the programm started
    float rotation = paused ? rotationOffset : ((timeNow - timeBefore) * 0.001) + rotationOffset; // one turn per 10 seconds ^= 36 deg/sec
    rotationOffset = rotation;

    timeBefore = timeNow;


    //////////////////////////////////////////////////////////////////////////

    glUseProgram(ShaderIds[0]);

    // reset the modelmatrix
    ModelViewMatrixStack.clear();
    ModelViewMatrixStack.loadMatrix(cameraTransform);

    ModelViewMatrixStack.translate(cameraTranslation);
    ModelViewMatrixStack.rotate(cameraRotation);
    ModelViewMatrixStack.top().invert();

    gloost::Matrix normalMatrix;

/*
    // save the current transformation onto the MatrixStack (sun)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.translate(0, -3.0f, 0);

        // transfer ModelViewMatrix for Geometry 1 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // set the NormalMatrix
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);

        // draw Geometry
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size() * 3, GL_UNSIGNED_INT, 0);

    }
    ModelViewMatrixStack.pop();
*/
    particles.update();
    particles.draw();

    glUseProgram(0);
}

/////////////////////////////////////////////////////////////////////////////////////////
void TimerFunction(int value)
{
    if(0 != value) {
        int fps = FrameCount * 4;
        glutSetWindowTitle( (gloost::toString(fps) + " fps").c_str());
    }
    FrameCount = 0;
    glutTimerFunc(250, TimerFunction, 1);
}


void RenderFunction(void)
{
    ++FrameCount;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Draw();

    glutSwapBuffers();
    glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////////////////////


void SetupShader()
{
    // LOAD AND LINK SHADER
    ShaderIds[0] = glCreateProgram();
    {
        // takes a (shader) filename and a shader-type and returns and id of the compiled shader
        ShaderIds[1] = Shader::loadShader("../data/shaders/simpleParticleShader.vert", GL_VERTEX_SHADER);
        ShaderIds[2] = Shader::loadShader("../data/shaders/simpleParticleShader.frag", GL_FRAGMENT_SHADER);

        // attaches a shader to a program
        glAttachShader(ShaderIds[0], ShaderIds[1]);
        glAttachShader(ShaderIds[0], ShaderIds[2]);
    }
    glLinkProgram(ShaderIds[0]);

    // describes how the uniforms in the shaders are named and to which shader they belong
    ModelViewMatrixUniformLocation  = glGetUniformLocation(ShaderIds[0], "ModelViewMatrix");
    ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");
    NormalMatrixUniformLocation     = glGetUniformLocation(ShaderIds[0], "NormalMatrix");
    ObjectColorUniformLocation      = glGetUniformLocation(ShaderIds[0], "ObjectColor");
}


/////////////////////////////////////////////////////////////////////////////////////////


void LoadModel(void)
{

    // load a wavefront *.obj file
    gloost::ObjLoader loader("../data/objects/sphere.obj");

    mesh = loader.getMesh();

    // IMPORTANT: use this to increase the reference counter
    // gloost::meshes have a garbage collector which throws
    // the mesh away otherwise
    mesh->takeReference();

    mesh->generateNormals();

    // normalizes the mesh
    mesh->scaleToSize(1.0);

    // puts the meshdata in one array
    mesh->interleave();

    // mesh->printMeshInfo();

    // create VAO which holds the state of our Vertex Attributes and VertexBufferObjects - a control structure
    // note: for different objects more of these are needed
    glGenVertexArrays(1, &BufferIds[0]);

    // bind Vertex Array - Scope begins
    glBindVertexArray(BufferIds[0]);

    // Create two VertexBufferObject and bind the first one to set its data
    glGenBuffers(2, &BufferIds[1]);
    glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);

    // set the vertex data for the actual buffer; the second parameter is the size in bytes of all Vertices together
    // the third parameter is a pointer to the vertexdata
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * mesh->getInterleavedAttributes().size(),
                 &mesh->getInterleavedAttributes().front(),
                 GL_STATIC_DRAW);

    // enables a VertexAttributeArray
    glEnableVertexAttribArray(0);

    // specifies where in the GL_ARRAY_BUFFER our data (the vertex position) is exactly
    glVertexAttribPointer(0,
                          GLOOST_MESH_NUM_COMPONENTS_VERTEX,
                          GL_FLOAT, GL_FALSE,
                          mesh->getInterleavedInfo().interleavedPackageStride,// mesh->getInterleavedInfo().interleavedVertexStride,
                          (GLvoid*)(mesh->getInterleavedInfo().interleavedVertexStride));

    // enables a VertexAttributeArray
    glEnableVertexAttribArray(1);

    // specifies where in the GL_ARRAY_BUFFER our data (the vertex position) is exactly
    glVertexAttribPointer(1,
                          GLOOST_MESH_NUM_COMPONENTS_NORMAL,
                          GL_FLOAT, GL_FALSE,
                          mesh->getInterleavedInfo().interleavedPackageStride,
                          (GLvoid*)(mesh->getInterleavedInfo().interleavedNormalStride));

    // the second VertexBufferObject ist bound
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);

    // its data are the indices of the vertices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(gloost::TriangleFace) * mesh->getTriangles().size(),
                 &mesh->getTriangles().front(),
                 GL_STATIC_DRAW);

    // unbind the VertexArray - Scope ends
    glBindVertexArray(0);
}


/////////////////////////////////////////////////////////////////////////////////////////


void Cleanup(void)
{
    glDetachShader(ShaderIds[0], ShaderIds[1]);
    glDetachShader(ShaderIds[0], ShaderIds[2]);
    glDeleteShader(ShaderIds[1]);
    glDeleteShader(ShaderIds[2]);
    glDeleteProgram(ShaderIds[0]);

    glDeleteBuffers(2, &BufferIds[1]);
    glDeleteVertexArrays(1, &BufferIds[0]);
}


/////////////////////////////////////////////////////////////////////////////////////////


void IdleFunction(void)
{
    glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////////////////////


void ResizeFunction(int Width, int Height)
{
    CurrentWidth = Width;
    CurrentHeight = Height;
    glViewport(0, 0, CurrentWidth, CurrentHeight);

    gloost::gloostPerspective(ProjectionMatrix,
                              60.0f,
                              (float)CurrentWidth / CurrentHeight,
                              1.0f,
                              100.0f
                             );

    glUseProgram(ShaderIds[0]);
    glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, ProjectionMatrix.data());
    glUseProgram(0);
}

/////////////////////////////////////////////////////////////////////////////////////////

void KeyboardFunction(unsigned char Key, int X, int Y)
{
    switch (Key) {
        case ' ': {
            paused = !paused;
            break;
        }

        case 'W':
        case 'w': {
            MoveCamera(CAM_TRANSLATE, 0, 0, 0.1f);
            break;
            }

        case 'A':
        case 'a': {
            MoveCamera(CAM_TRANSLATE, 0.1f, 0, 0);
            break;
            }

        case 'S':
        case 's': {
            MoveCamera(CAM_TRANSLATE, 0, 0, -0.1f);
            break;
            }

        case 'D':
        case 'd': {
            MoveCamera(CAM_TRANSLATE, -0.1f, 0, 0);
            break;
            }

        case 'R':
        case 'r': {
            // reset the camera to the initial view
            cameraRotation    = gloost::Vector3(0.0f, 0.0f, 0.0f);
            cameraTranslation = gloost::Vector3(0.0f, 0.0f, 8.0f);
            break;
            }

        case  27: // ESC
        case 'q':
        case 'Q': {
            Cleanup();
            exit(0);
        }

        default:
            break;
    }
}

void MouseFunction(int Button, int State, int X, int Y)
{
switch (Button) {

        case (0): {
            if (GLUT_DOWN == State) {
                MovementOriginX = X;
                MovementOriginY = Y;

            }
            break;
        }

        // Wheel reports as button 3(scroll up) and button 4(scroll down)
        case 3:
        case 4: { // It's a wheel event
            // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
            if (State == GLUT_UP) return; // Disregard redundant GLUT_UP events

            if (4 == Button) MoveCamera(CAM_TRANSLATE, 0, 0, -0.2f);
            else /* 5 */     MoveCamera(CAM_TRANSLATE, 0, 0, 0.2f);

            break;
        }

        default: {
            printf("Button %s At %d %d\n", (State == GLUT_DOWN) ? "Down" : "Up", X, Y);
            break;
        }
    }
}

void MotionFunction(int X, int Y)
{
    MoveCamera(CAM_ROTATE, X, Y, 0.0f);
}

void MoveCamera(CameraManipulation Mode, float X, float Y, float Z)
{
    switch (Mode) {
        case CAM_TRANSLATE: {
            cameraTranslation[0] -= X;
            cameraTranslation[1] -= Y;
            cameraTranslation[2] -= Z;
            break;
        }

        case CAM_ROTATE:{
            cameraRotation[0] = (Y - MovementOriginY)/600.0f; // TODO: change to
            cameraRotation[1] = (X - MovementOriginX)/600.0f; // not suck so much
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////


void InitWindow(int argc, char* argv[])
{
    glutInit(&argc, argv);

    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

    glutInitWindowSize(CurrentWidth, CurrentHeight);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    WindowHandle = glutCreateWindow("");

    if(WindowHandle < 1) {
        fprintf(
            stderr,
            "ERROR: Could not create a new rendering window.\n"
        );
        glutExit();
    }

    // Glut function callbacks
    glutTimerFunc(0, TimerFunction, 0);
    glutReshapeFunc(ResizeFunction);
    glutDisplayFunc(RenderFunction);
    glutIdleFunc(IdleFunction);
    glutKeyboardFunc(KeyboardFunction);
    glutMouseFunc(MouseFunction);
    glutMotionFunc(MotionFunction);
    glutCloseFunc(Cleanup);

}

/////////////////////////////////////////////////////////////////////////////////////////

void Initialize(int argc, char* argv[])
{
    GLenum GlewInitResult;

    InitWindow(argc, argv);

    glewExperimental = GL_TRUE;
    GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult) {
        fprintf(
            stderr,
            "ERROR: %s\n",
            glewGetErrorString(GlewInitResult)
        );
        glutExit();
    }

    fprintf(
        stdout,
        "INFO: OpenGL Version: %s\n",
        glGetString(GL_VERSION)
    );

    glGetError();
    glClearColor(0.20f, 0.2f, 0.2f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    cameraTransform.setIdentity();
    ModelViewMatrixStack.loadIdentity();
    ProjectionMatrix.setIdentity();

    SetupShader();
    LoadModel();
    particles.prepare();
}
