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
#include <FreeImage.h>
#include <Shader.h>

// include gloost::Mesh wich is a geometry container
#include <Mesh.h>
gloost::Mesh* mesh = 0;

// loader for the wavefront *.obj file format
#include <ObjLoader.h>

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
unsigned SamplerUniformLocation          = 0;

unsigned BufferIds[6]  = { 0u };
unsigned ShaderIds[3]  = { 0u };
unsigned TextureId     = 0;

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

    // save the current transformation onto the MatrixStack (sun)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.translate(.1, -.2, 0);
        ModelViewMatrixStack.scale(2.0f);

        // transfer ModelViewMatrix for Geometry 1 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // bind Texture and transfer to shader
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, TextureId);
        glUniform1i(SamplerUniformLocation, GL_TEXTURE0 + 0);

        // transfer object color vector for Geometry 1 to Shaders
        // glUniform4f(ObjectColorUniformLocation, 1.0f, 1.0f, 0.0f, 1.0f); // yellow

        // set the NormalMatrix for Geometry 1
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix for Geometry 1 to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);
        // draw Geometry 1
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size()*3, GL_UNSIGNED_INT, 0);

    }
    ModelViewMatrixStack.pop();

    // save the current transformation onto the MatrixStack (first planet)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.scale(0.25f);
        ModelViewMatrixStack.rotate(0.0f, rotation * 1.5, 0.0f);
        ModelViewMatrixStack.translate(0.0f, 0.0f, 7.0f);


        // transfer ModelViewMatrix for Geometry 2 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // transfer object color vector for Geometry 2 to Shaders
        glUniform4f(ObjectColorUniformLocation, 0.0f, 0.18f, 0.25f, 1.0f); // light blue

        // set the NormalMatrix for Geometry 2
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix for Geometry 2 to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);
        // draw Geometry 2
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size()*3, GL_UNSIGNED_INT, 0);

    }
    ModelViewMatrixStack.pop(); // first planet

    // save the current transformation onto the MatrixStack (second planet)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.scale(0.4f);
        ModelViewMatrixStack.rotate(0.0f, rotation * 0.25f, 0.0f);
        ModelViewMatrixStack.translate(0.0f, 0.0f, 6.0f);

        // transfer ModelViewMatrix for Geometry 3 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // transfer object color vector for Geometry 3 to Shaders
        glUniform4f(ObjectColorUniformLocation, 0.4f, 0.2f, 0.0f, 1.0f); // orange

        // set the NormalMatrix for Geometry 3
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix for Geometry 3 to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);
        // draw Geometry 3
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size()*3, GL_UNSIGNED_INT, 0);

    }
    // don't pop it, since we want it to have a moon

    // save the current transformation onto the MatrixStack (second planet's moon)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.scale(0.25f);
        ModelViewMatrixStack.rotate(rotation * 1.2f, 0.0f, 0.0f);
        ModelViewMatrixStack.translate(0.0f, 0.0f, 3.0f);

        // transfer ModelViewMatrix for Geometry 4 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // transfer object color vector for Geometry 4 to Shaders
        glUniform4f(ObjectColorUniformLocation, 0.1f, 0.1f, 0.1f, 1.0f); // white

        // set the NormalMatrix for Geometry 4
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix for Geometry 4 to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);
        // draw Geometry 4
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size()*3, GL_UNSIGNED_INT, 0);

    }
    ModelViewMatrixStack.pop(); // the moon
    ModelViewMatrixStack.pop(); // and the second planet

    // save the current transformation onto the MatrixStack (third planet)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.scale(.65f);
        ModelViewMatrixStack.rotate(0.0f, rotation, 0.0f);
        ModelViewMatrixStack.translate(0.0, 0.0, 5.0f);

        // transfer ModelViewMatrix for Geometry 5 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // transfer object color vector for Geometry 5 to Shaders
        glUniform4f(ObjectColorUniformLocation, 0.5f, 0.3f, 0.0f, 1.0f); // brownish

        // set the NormalMatrix for Geometry 5
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix for Geometry 5 to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);
        // draw Geometry 5
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size()*3, GL_UNSIGNED_INT, 0);

    }
    ModelViewMatrixStack.pop(); // third planet

    // save the current transformation onto the MatrixStack (fourth planet)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.scale(.75f);
        ModelViewMatrixStack.rotate(0.0f, rotation * 0.8f + 1, 0.0f);
        ModelViewMatrixStack.translate(0.0, 0.0, 6.0f);

        // transfer ModelViewMatrix for Geometry 6 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // transfer object color vector for Geometry 6 to Shaders
        glUniform4f(ObjectColorUniformLocation, 0.13f, 0.6f, 0.36f, 1.0f); // turquoise

        // set the NormalMatrix for Geometry 6
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix for Geometry 6 to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);
        // draw Geometry 6
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size()*3, GL_UNSIGNED_INT, 0);

    }
    ModelViewMatrixStack.pop(); // fourth planet

    // save the current transformation onto the MatrixStack (fifth planet)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.scale(.25f);
        ModelViewMatrixStack.rotate(0.0f, rotation * 2, 0.0f);
        ModelViewMatrixStack.translate(0.0, 0.0, 22.0f);

        // transfer ModelViewMatrix for Geometry 7 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // transfer object color vector for Geometry 7 to Shaders
        glUniform4f(ObjectColorUniformLocation, 0.0f, 0.0f, 0.5f, 1.0f); // glarish blue
        // set the NormalMatrix for Geometry 7
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix for Geometry 7 to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);
        // draw Geometry 7
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size()*3, GL_UNSIGNED_INT, 0);

    }
    ModelViewMatrixStack.pop(); // fifth planet

    // save the current transformation onto the MatrixStack (sixths and last planet)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.scale(.45f);
        ModelViewMatrixStack.rotate(0.0f, rotation * 0.33 - 4, 0.0f);
        ModelViewMatrixStack.translate(0.0, 0.0, 15.0f);

        // transfer ModelViewMatrix for Geometry 8 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // transfer object color vector for Geometry 7 to Shaders
        glUniform4f(ObjectColorUniformLocation, 0.5f, 0.33f, 0.5f, 1.0f); // bubble-gum pink

        // set the NormalMatrix for Geometry 8
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix for Geometry 8 to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);
        // draw Geometry 7
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size()*3, GL_UNSIGNED_INT, 0);

    }

    // two moons

    // save the current transformation onto the MatrixStack (last planet's first moon)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.scale(.25f);
        ModelViewMatrixStack.rotate(0.0f, rotation * 1.75, 0.0f);
        ModelViewMatrixStack.translate(0.0, 0.0, 2.55f);

        // transfer ModelViewMatrix for Geometry 8 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // transfer object color vector for Geometry 8 to Shaders
        glUniform4f(ObjectColorUniformLocation, 0.1f, 0.1f, 0.1f, 1.0f); // white

        // set the NormalMatrix for Geometry 8
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix for Geometry 8 to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);
        // draw Geometry 7
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size()*3, GL_UNSIGNED_INT, 0);

    }
    ModelViewMatrixStack.pop(); // last planet's first moon

    // save the current transformation onto the MatrixStack (last planet's second moon)
    ModelViewMatrixStack.push();
    {
        ModelViewMatrixStack.scale(.25f);
        ModelViewMatrixStack.rotate(rotation * 1.75, 0.0f, 0.0f);
        ModelViewMatrixStack.translate(0.0, 0.0, 3.5f);

        // transfer ModelViewMatrix for Geometry 9 to Shaders
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, GL_FALSE, ModelViewMatrixStack.top().data());

        // transfer object color vector for Geometry 9 to Shaders
        glUniform4f(ObjectColorUniformLocation, 0.1f, 0.1f, 0.1f, 1.0f); // white

        // set the NormalMatrix for Geometry 9
        normalMatrix = ModelViewMatrixStack.top();
        normalMatrix.invert();
        normalMatrix.transpose();

        // transfer NormalMatrix for Geometry 9 to Shaders
        glUniformMatrix4fv(NormalMatrixUniformLocation, 1, GL_FALSE, normalMatrix.data());

        // bind the Geometry
        glBindVertexArray(BufferIds[0]);
        // draw Geometry 7
        glDrawElements(GL_TRIANGLES, mesh->getTriangles().size()*3, GL_UNSIGNED_INT, 0);

    }
    ModelViewMatrixStack.pop(); // last planet's second moon

    ModelViewMatrixStack.pop(); // last planet
    ModelViewMatrixStack.pop(); // sun

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
        ShaderIds[1] = Shader::loadShader("../data/shaders/simpleVertexShader.vs", GL_VERTEX_SHADER);
        ShaderIds[2] = Shader::loadShader("../data/shaders/simpleFragmentShader.fs", GL_FRAGMENT_SHADER);

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
    SamplerUniformLocation          = glGetUniformLocation(ShaderIds[0], "ObjectSampler");
}


/////////////////////////////////////////////////////////////////////////////////////////


void LoadModel(void)
{
    // load a wavefront *.obj file
    gloost::ObjLoader loader("../data/objects/sphere_new.obj");

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

    mesh->printMeshInfo();

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

    // specifies where in the GL_ARRAY_BUFFER our data(the vertex position) is exactly
    glVertexAttribPointer(0,
                          GLOOST_MESH_NUM_COMPONENTS_VERTEX,
                          GL_FLOAT,
                          GL_FALSE,
                          mesh->getInterleavedInfo().interleavedPackageStride,// mesh->getInterleavedInfo().interleavedVertexStride,
                          (GLvoid*)(mesh->getInterleavedInfo().interleavedVertexStride));

    // enables a VertexAttributeArray
    glEnableVertexAttribArray(1);

    // specifies where in the GL_ARRAY_BUFFER our data(the normal vectors) is exactly
    glVertexAttribPointer(1,
                          GLOOST_MESH_NUM_COMPONENTS_NORMAL,
                          GL_FLOAT,
                          GL_FALSE,
                          mesh->getInterleavedInfo().interleavedPackageStride,
                          (GLvoid*)(mesh->getInterleavedInfo().interleavedNormalStride));

    // glBindBuffer(GL_TEXTURE_COORD_ARRAY, BufferIds[1]); // TODO

    // enables a VertexAttributeArray
    glEnableVertexAttribArray(2);

    // specifies where in the GL_TEXTURE_COORD_ARRAY our data(the texture coordinates) is exactly
    glVertexAttribPointer(2, // TODO
                          GLOOST_MESH_NUM_COMPONENTS_TEXCOORD,
                          GL_FLOAT,
                          GL_FALSE,
                          mesh->getInterleavedInfo().interleavedPackageStride,
                          (GLvoid*)(mesh->getInterleavedInfo().interleavedTexcoordStride));

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

bool CreateTexture(std::string const& filename)
{
    FIBITMAP *bitmap    = FreeImage_Load(FreeImage_GetFileType(filename.c_str()), filename.c_str());
    unsigned char *data = FreeImage_GetBits(bitmap);

    // generate texture id
    glGenTextures(1, &TextureId);

    if (0 == TextureId) {
        // OpenGL was not able to generate additional texture
        return false;
    }

    // ??
    glEnable(GL_TEXTURE_2D);

    // bind texture object
    glBindTexture(GL_TEXTURE_2D, TextureId);

    // load image data
    glTexImage2D(
        GL_TEXTURE_2D, // target
        0,      // level of detail
        GL_RGB, // internal format
        FreeImage_GetWidth(bitmap),
        FreeImage_GetHeight(bitmap),
        0,      // border width
        GL_BGR, // format of the pixel data, FreeImage returns BGR values
        GL_UNSIGNED_BYTE, // data type of the pixel data; unsigned char == unsigned byte
        data    // FreeImage_GetBits();
    );

    // setting Texture Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    return true;
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

    FreeImage_DeInitialise(); // not needed if linked dynamically
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

    FreeImage_Initialise();

    LoadModel();
    // CreateTexture("../data/textures/Planet_Texture_02_by_Qzma.jpg");
    CreateTexture("../data/textures/gradient.png");
}
