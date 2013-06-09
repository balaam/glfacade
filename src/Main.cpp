//g++ -Wall main.cpp../lib/lua/src/liblua.a -I../lib/lua/src -Dmain=SDL_main -L/usr/local/lib -o DancingSquid -lmingw32 -lSDLmain -lSDL -lopengl32 -static-libgcc -static-libstdc++ -lws2_32 -lwinmm

#ifndef _WIN32
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif /* GL_GLEXT_PROTOTYPES */
#endif /* _WIN32 */

#include "Main.h"
#include <assert.h>
#include <stdio.h>
#include "SDL/SDL.h"
#include <gl/gl.h>
#include <gl/glext.h>
#include <vector>
#include <stack>
#include <math.h>


#include "VitaMath.h"
#include "VitaGLFacade.h"

#include <Cg/Cg.h>
#include <Cg/cgGL.h>

#include "soil.h"


// GG object
CGcontext cgContext;
CGprofile cgVertexProfile;
CGprofile cgFragmentProfile;
CGprogram cgVertexShader;
CGprogram cgFragmentShader;

// Stuff passed into the shader
CGparameter cgModelViewProj;
CGparameter cgPosition;
CGparameter cgColor;
CGparameter cgTexCoord;
CGparameter cgTexture;

int testTexture;

VitaGLFacade gVitaFacade = VitaGLFacade();

using namespace std;

int LoadTexture(const char *filename)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* image = SOIL_load_image
    (
        filename,
        &width, &height, &channels,
        SOIL_LOAD_AUTO
    );

    if(image == NULL)
    {
        return -1;
    }

    GLuint tex_2d = SOIL_create_OGL_texture
    (
        image,
        width, height, channels,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );

    SOIL_free_image_data
    (
        image
    );

    return tex_2d;
}

void Main::OnEvent(SDL_Event* event)
{
    switch(event->type)
    {
        case SDL_QUIT:
        {
            mRunning = false;
        } break;
        case SDL_KEYDOWN:
        {
            if(event->key.keysym.sym == SDLK_ESCAPE)
            {
                printf("Stopped looping because escape pressed.\n");
                mRunning = false;
            }
        } break;
    }
}

void printCGError(CGcontext cgContext)
{
    CGerror err = cgGetError();
    printf("Error:%s\n", cgGetErrorString(err));
    printf("\n%s", cgGetLastListing(cgContext));
}

bool initCG()
{
    cgContext = cgCreateContext();

    if(!cgContext)
    {
        printf("\nFailed to create CG context\n");
        return false;
    }

    // Don't have as Vita is a fixed target ?
    cgFragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);

    // Vita programs need to be precompliled - you can't load them in like this.
    cgFragmentShader = cgCreateProgramFromFile(cgContext, CG_SOURCE, "facade_fragment.cg", cgFragmentProfile, NULL, NULL);
    cgVertexShader = cgCreateProgramFromFile(cgContext, CG_SOURCE, "facade_vertex.cg", cgVertexProfile, NULL, NULL);

    if(!cgFragmentShader)
    {
        printCGError(cgContext);
        printf("\nFragment shader failed to load.\n");
        return false;
    }

    if(!cgVertexShader)
    {
        printCGError(cgContext);
        printf("\nFragment shader failed to load.\n");
        return false;
    }

    printf("Vertex and Fragment shader loaded.\n");


    cgGLLoadProgram(cgVertexShader);
    cgModelViewProj = cgGetNamedParameter(cgVertexShader, "modelViewProj");

    cgPosition = cgGetNamedParameter(cgVertexShader, "vIn.position");
    if(NULL == cgPosition)
    {
        printf("Error getting named parameter color.\n");
    }

    cgColor = cgGetNamedParameter(cgVertexShader, "vIn.color");
    if(NULL == cgColor)
    {
        printf("Error getting named parameter color.\n");
    }
    cgTexCoord = cgGetNamedParameter(cgVertexShader, "vIn.texCoord");
    if(NULL == cgColor)
    {
        printf("Error getting named parameter texCoord.\n");
    }

    cgGLLoadProgram(cgFragmentShader);
    cgTexture = cgGetNamedParameter( cgFragmentShader, "tex");
    if(NULL == cgColor)
    {
        printf("Error getting named parameter tex.\n");
    }
    cgGLSetTextureParameter( cgTexture, testTexture );

    return true;
}

int Main::Execute()
{
	int viewWidth = 640;
    int viewHeight = 360;
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return false;
    }

    // Allow the game pads to be polled.
    SDL_JoystickEventState(SDL_IGNORE);
    SDL_EnableUNICODE(1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,        8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,      8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,       8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,      8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,      16);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,     32);

    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,  8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);

    // This should use the data from the object def.
    SDL_WM_SetCaption("OpenGL Fake", "OpenGL Fake");

    if((mSurface = SDL_SetVideoMode(viewWidth, viewHeight, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)) == NULL)
    {
        return false;
    }

    SDL_WarpMouse(viewWidth/2, viewHeight/2);

    // Setups an orthographic view, should be handled by renderer.
    glClearColor(0, 0, 0, 0);
    glViewport(0, 0, viewWidth, viewHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, viewWidth, viewHeight, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);
    glLoadIdentity();
    glClearColor(0.0, 0.0, 0.0, 1.0f);
    glPointSize(5);


    testTexture = LoadTexture("texture.png");
    if(-1 == testTexture)
    {
        printf("FAILED TO LOAD TEST TEXTURE.\n");
    }
    if(false == initCG())
    {
        printf("Failed to init CG\n");
    }
    DoOpenGLTests();


    unsigned int thisTime = 0;
    unsigned int lastTime = 0;

    SDL_Event event;
    while(mRunning)
    {
        // Calculate delta time
        thisTime = SDL_GetTicks(); // returns in milliseconds
        mDeltaTime = (float)(thisTime - lastTime) / 1000; // convert to seconds
        lastTime = thisTime;

        while(SDL_PollEvent(&event))
        {
            OnEvent(&event);
        }

    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        OnUpdate();
    	glLoadIdentity();
    	SDL_GL_SwapBuffers();
        SDL_Delay(0);
    }
	return 0;
}

struct OpenGLMatrix
{
    float values[16];
};

void PrintMatrixStack(GLenum whichStack, GLenum whichStackDepth)
{
    vector<OpenGLMatrix> oglMatrixStack;
    int depth = -1;
    glGetIntegerv(whichStackDepth,  &depth);
    printf("\nStack Depth: %d\n", depth);

    if(-1 == depth)
    {
        printf("Failed to get a valid stack depth.\n");
    }

    for(int i = 0; i < depth; i++)
    {
        if(i == depth -1)
        {
            printf("Matrix at position: %d [Bottom]\n", (depth - 1) - i);
        }
        else
        {
            printf("Matrix at position: %d\n", (depth - 1) - i);
        }

        OpenGLMatrix matrix;
        glGetFloatv (whichStack, (float*)matrix.values);
        for(int i = 0; i < 4; i++)
        {
            printf("%.5f\t%.5f\t%.5f\t%.5f\n",
                matrix.values[i],
                matrix.values[i + 4],
                matrix.values[i + 8],
                matrix.values[i + 12]);
        }
        oglMatrixStack.push_back(matrix);
        glPopMatrix();
    }

    // We've popped all the matrices off the stack
    // Let's put them back
    for(int i = 0; i < depth; i++)
    {
        if(i > 0) // There's always a 0 matrix whatever you do.
        {
            glPushMatrix();
        }
        glLoadMatrixf(oglMatrixStack.back().values);
        oglMatrixStack.pop_back();
    }
    assert(oglMatrixStack.empty());
}

void PrintModelViewMatrix()
{
    PrintMatrixStack(GL_MODELVIEW_MATRIX, GL_MODELVIEW_STACK_DEPTH);
}

void PrintProjMatrix()
{
    PrintMatrixStack(GL_PROJECTION_MATRIX, GL_PROJECTION_STACK_DEPTH);
}



void Main::DoOpenGLTests()
{

    if(true)
    {
        return;
    }

    gVitaFacade.glMatrixMode(GL_PROJECTION);
    gVitaFacade.glLoadIdentity();
    gVitaFacade.glOrtho(0, 640, 0, 360, 1, -1);
    gVitaFacade.PrintMatrix(gVitaFacade.CalcModelViewProjMatrix());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640, 0, 360, 1, -1);
    PrintProjMatrix();

    if(true)
    {
        return;
    }

// These tests need to be done after the context is intialised.
// if (GLEE_ARB_multitexture)    //is multitexture support available?
// {
//     printf("Multitext support\n");
// }
// else
// {
//     printf("No multitext supprt\n");
// }

// if(!GLEE_EXT_texture3D)
// {
//    printf("no 3d texture support found");
// }
    //   What opengl operations does Quell use?
    //      [*] glPushMatrix()
    //      [*] glPopMatrix()
    //      [*] glTranslatef()
    //      [*] glRotatef() *Doesn't normalize non-normalized x,y,z vectors. Which  opengl does.
    //      [*] glScalef()
    //      [ ] glOrthof()

    // There are also these that I don't quite know how they'll come into it
    //glClientActiveTexture(GL_TEXTURE0+e);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    //glTexCoordPointer(ctx->texParams, rlTexCoordTypeGL, ctx->vertexStride, (rlTexCoordType*)ctx->texData[e]);
    //glActiveTexture(GL_TEXTURE0+e);

    float testMat[16];
    for(int i = 0; i < 16; i++)
    {
        testMat[i] = i;
    }

    //read the 4x4 matrix and store in x
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    {
       // glOrtho(0, 640, 360, 0, 1, -1);
       // glLoadMatrixf(testMat);
       glTranslatef(50, 0, 0);
       //  glTranslatef(0, 50, 0);
       // // PrintModelViewMatrix();
       //  glRotatef(90, 1, 0, 0);
       //  glScalef(2, 4, 8);
        PrintModelViewMatrix();
        // printf("-----------\n");
        // PrintModelViewMatrix();
        // printf("-----------\n");
        // PrintModelViewMatrix();
    }
    glPopMatrix();

    // Question 1 : When I do a translate the push a matrix and do a translate
    // are those matrices mutiplied or just the top one used?
    // I'm thinking they're multplied.

    // Answer: They're multiplied.

    Matrix4 vitaMat = Matrix4();
    vitaMat = vitaMat.identity();

    printf("--------VITA-----------\n");
   // printf("------------identity-----------\n");
    //vitaMat.print();
    printf("------------translate 50-----------\n");
    // vitaMat = vitaMat.translation(Vector4(50, 0, 0, 0));
    // vitaMat.print();

   // gVitaFacade.glLoadMatrixf(testMat);

    //gVitaFacade.glTranslatef(0, 50, 0);
   // gVitaFacade.PrintMatrix(gVitaFacade.Top());
   // gVitaFacade.glRotatef(90, 1, 0, 0);
    //gVitaFacade.glScalef(2, 4, 8);
   // gVitaFacade.glOrtho(0, 640, 360, 0, 1, -1);

    gVitaFacade.glMatrixMode(GL_MODELVIEW);
    gVitaFacade.glPushMatrix();
    {
        gVitaFacade.glTranslatef(50, 0, 0);
        gVitaFacade.PrintMatrix(gVitaFacade.Top());
    }
    gVitaFacade.glPopMatrix();
    printf("after pop\n");
    //
    // gVitaFacade.PrintMatrix(gVitaFacade.Top());

    // glTranslate produces a translation by (x, y, z) . The current matrix (see glMatrixMode) is multiplied by this translation matrix, with the product replacing the current matrix, as if glMultMatrix were called with the following matrix for its argument:

    // (
    //     1   0   0   x
    //     0   1   0   y
    //     0   0   1   z
    //     0   0   0   1
    // )
}

void DrawVertex(Vector4& v)
{
    glVertex3d(v.getX(), v.getY(), v.getZ());
}

void DrawHVertex(Vector4& v)
{
    glVertex3d(v.getX() / v.getW(), v.getY() / v.getW(), v.getZ() / v.getW());
}

void DrawImmediate()
{
    float width = 64;
    float height = 64;
    float cx = 640/2;
    float cy = 360/2;

    Vector4 pointTL;
    pointTL.setX(cx - width / 2 );
    pointTL.setY(cy + height / 2);
    pointTL.setZ(0);
    pointTL.setW(1);

    Vector4 pointTR;
    pointTR.setX(cx + width / 2);
    pointTR.setY(cy + height / 2);
    pointTR.setZ(0);
    pointTR.setW(1);

    Vector4 pointBR;
    pointBR.setX(cx + width / 2);
    pointBR.setY(cy - height / 2);
    pointBR.setZ(0);
    pointBR.setW(1);

    Vector4 pointBL;
    pointBL.setX(cx - width / 2);
    pointBL.setY(cy - height / 2);
    pointBL.setZ(0);
    pointBL.setW(1);

    float tlColor[] = {1.0, 1.0, 0.0, 1.0};
    float trColor[] = {1.0, 1.0, 0.0, 1.0};
    float brColor[] = {1.0, 0.0, 1.0, 1.0};
    float blColor[] = {1.0, 0.0, 1.0, 1.0};

    glBegin(GL_TRIANGLES);

    // Top tri
    cgGLSetParameter4fv(cgColor, tlColor);
    cgGLSetParameter2f(cgTexCoord, 0, 0);
    DrawVertex(pointTL);
    cgGLSetParameter2f(cgTexCoord, 1, 0);
    DrawVertex(pointTR);

    cgGLSetParameter4fv(cgColor, blColor);
    cgGLSetParameter2f(cgTexCoord, 0, 1);
    DrawVertex(pointBL);

    // Bottom tri
    cgGLSetParameter4fv(cgColor, trColor);
    cgGLSetParameter2f(cgTexCoord, 1, 0);
    DrawVertex(pointTR);
    cgGLSetParameter4fv(cgColor, brColor);
    cgGLSetParameter2f(cgTexCoord, 1, 1);
    DrawVertex(pointBR);
    cgGLSetParameter4fv(cgColor, blColor);
    cgGLSetParameter2f(cgTexCoord, 0, 1);
    DrawVertex(pointBL);

    glEnd();
}

//
// Uses OpenGL immediate mode and push and pop matrices to draw a single point
// near the middle of the screen
//
void DrawImmediateFixedOpenGL()
{
    glDisable(GL_TEXTURE_2D);
    glPointSize(5);

    float cx = 640/2;
    float cy = 360/2;

    glColor3f(1, 1, 1);
    Vector4 pointPos;
    pointPos.setX(cx);
    pointPos.setY(cy);
    pointPos.setZ(0);
    pointPos.setW(1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640, 0, 360, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    {
        glTranslatef(50, 0, 0);
        glBegin(GL_POINTS);
        DrawVertex(pointPos);
        DrawVertex(pointPos);
        glEnd();
    }
    glPopMatrix();
}

void DrawShaderImmediateMode()
{
        // Shader Manual Transform [FINE]
    glEnable(GL_TEXTURE_2D);

    //Shader GPU Transform
    cgGLEnableProfile(cgFragmentProfile);
    cgGLBindProgram(cgFragmentShader);

    cgGLEnableProfile(cgVertexProfile);
    cgGLBindProgram(cgVertexShader);
   // cgGLSetStateMatrixParameter(cgModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    gVitaFacade.glMatrixMode(GL_PROJECTION);
    gVitaFacade.glLoadIdentity();
    gVitaFacade.glOrtho(0, 640, 0, 360, 1, -1);
    gVitaFacade.glMatrixMode(GL_MODELVIEW);
    gVitaFacade.glLoadIdentity();
    gVitaFacade.glPushMatrix();
    {
        cgGLEnableTextureParameter( cgTexture );
        gVitaFacade.glTranslatef(50, 0, 0);
        float* mOpenGLMat = gVitaFacade.CalcModelViewProjMatrix().GetOpenGLMat();
        cgSetMatrixParameterfc(cgModelViewProj, mOpenGLMat);


        DrawImmediate();
    }
    gVitaFacade.glPopMatrix();
    cgGLDisableProfile(cgVertexProfile);
    cgGLDisableProfile(cgFragmentProfile);
}

void DrawManualTransformFixedOpenGL()
{
    glDisable(GL_TEXTURE_2D);
    glPointSize(5);

    float cx = 640/2;
    float cy = 360/2;

    glColor3f(1, 1, 1);
    Vector4 pointPos;
    pointPos.setX(cx);
    pointPos.setY(cy);
    pointPos.setZ(0);
    pointPos.setW(1);

    //Reset OpenGL's mode so it does't interfer.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gVitaFacade.glMatrixMode(GL_MODELVIEW);
    gVitaFacade.glLoadIdentity();
    gVitaFacade.glPushMatrix();
    {
        gVitaFacade.glTranslatef(50, 0, 0);

        Matrix4 modelViewMat = gVitaFacade.CalcModelViewMatrix();
        Vector4 newPoint = modelViewMat * pointPos;

        glBegin(GL_POINTS);
        DrawVertex(newPoint);
        glEnd();
    }
    gVitaFacade.glPopMatrix();
}

void DrawArraysFixed()
{
    float width = 64;
    float height = 64;
    float cx = 640/2;
    float cy = 360/2;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640, 0, 360, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    int coordsPerVert = 4;
    int vertexStride = 0;
    float vertArray[] =
    {
        cx - width / 2, cy + height / 2, 0, 1, // TL XYZW
        cx + width / 2, cy + height / 2, 0, 1, // TR XYZW
        cx - width / 2, cy - height / 2, 0, 1, // BL XYZW
    };

    float colorArray[] =
    {
        1, 0, 1, 1, // TL RGBA
        1, 0, 1, 1, // TR RGBA
        1, 0, 1, 1, // BL RGBA
    };
    int channelsPerVert = 4;
    float texCoordArray[] =
    {
        0, 0,
        1, 0,
        0, 1,
    };
    int texCoordsPerVert = 2;

    glTexCoordPointer(texCoordsPerVert, GL_FLOAT, vertexStride, texCoordArray);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glColorPointer(channelsPerVert, GL_FLOAT, vertexStride, colorArray);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(coordsPerVert, GL_FLOAT, vertexStride, vertArray);
    glEnableClientState(GL_VERTEX_ARRAY);
    int numVerts = 3;
    glDrawArrays(GL_TRIANGLES, 0, numVerts);

}

void DrawArraysShader()
{
    float width = 64;
    float height = 64;
    float cx = 640/2;
    float cy = 360/2;

    // Stop any OpenGL inteference
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    cgGLEnableProfile(cgFragmentProfile);
    cgGLBindProgram(cgFragmentShader);

    cgGLEnableProfile(cgVertexProfile);
    cgGLBindProgram(cgVertexShader);
    cgGLSetStateMatrixParameter(cgModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    gVitaFacade.glMatrixMode(GL_PROJECTION);
    gVitaFacade.glLoadIdentity();
    gVitaFacade.glOrtho(0, 640, 0, 360, 1, -1);
    gVitaFacade.glMatrixMode(GL_MODELVIEW);
    gVitaFacade.glLoadIdentity();

    int coordsPerVert = 4;
    int vertexStride = 0;
    float vertArray[] =
    {
        cx - width / 2, cy + height / 2, 0, 1, // TL XYZW
        cx + width / 2, cy + height / 2, 0, 1, // TR XYZW
        cx - width / 2, cy - height / 2, 0, 1, // BL XYZW
    };

    float colorArray[] =
    {
        1, 0, 1, 1, // TL RGBA
        1, 0, 1, 1, // TR RGBA
        1, 0, 1, 1, // BL RGBA
    };
    int channelsPerVert = 4;
    float texCoordArray[] =
    {
        0, 0,
        1, 0,
        0, 1,
    };
    int texCoordsPerVert = 2;

    gVitaFacade.glPushMatrix();
    {
        cgGLEnableTextureParameter( cgTexture );
        gVitaFacade.glTranslatef(100, -50, 0);
        float* mOpenGLMat = gVitaFacade.CalcModelViewProjMatrix().GetOpenGLMat();
        cgSetMatrixParameterfc(cgModelViewProj, mOpenGLMat);

        // glTexCoordPointer(texCoordsPerVert, GL_FLOAT, vertexStride, texCoordArray);
        // glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        cgGLEnableClientState(cgTexCoord);
        cgGLSetParameterPointer(cgTexCoord, texCoordsPerVert, GL_FLOAT, vertexStride, texCoordArray);

        // glColorPointer(channelsPerVert, GL_FLOAT, vertexStride, colorArray);
        // glEnableClientState(GL_COLOR_ARRAY);
        cgGLEnableClientState(cgColor);
        cgGLSetParameterPointer(cgColor, channelsPerVert, GL_FLOAT, vertexStride, colorArray);

        // glVertexPointer(coordsPerVert, GL_FLOAT, vertexStride, vertArray);
        // glEnableClientState(GL_VERTEX_ARRAY);
        cgGLEnableClientState(cgPosition);
        cgGLSetParameterPointer(cgPosition, coordsPerVert, GL_FLOAT, vertexStride, vertArray);


        int numVerts = 3;
        glDrawArrays(GL_TRIANGLES, 0, numVerts);
    }
    gVitaFacade.glPopMatrix();
    cgGLDisableProfile(cgVertexProfile);
    cgGLDisableProfile(cgFragmentProfile);
}

struct VertexDef
{
    Vector4 position;
    Vector4 color;
    float u;
    float v;

    public: VertexDef(
        float x, float y, float z, float w,
        float r, float g, float b, float a,
        float u, float v);
};

VertexDef::VertexDef(float x, float y, float z, float w,
        float r, float g, float b, float a,
        float u, float v)
{
    position.setX(x);
    position.setY(y);
    position.setZ(z);
    position.setW(w);
    color.setX(r);
    color.setY(g);
    color.setZ(b);
    color.setW(a);
    this->u = u;
    this->v = v;
}

void DrawStructShader()
{
    float width = 64;
    float height = 64;
    float cx = 640/2;
    float cy = 360/2;

    VertexDef verts[] =
    {
        VertexDef(cx - width / 2, cy + height / 2, 0, 1, 1, 0, 1, 1, 0, 0),
        VertexDef(cx + width / 2, cy + height / 2, 0, 1, 1, 0, 1, 1, 1, 0),
        VertexDef(cx - width / 2, cy - height / 2, 0, 1, 1, 0, 1, 1, 0, 1),
    };


    // Stop any OpenGL inteference
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    cgGLEnableProfile(cgFragmentProfile);
    cgGLBindProgram(cgFragmentShader);

    cgGLEnableProfile(cgVertexProfile);
    cgGLBindProgram(cgVertexShader);
    cgGLSetStateMatrixParameter(cgModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    gVitaFacade.glMatrixMode(GL_PROJECTION);
    gVitaFacade.glLoadIdentity();
    gVitaFacade.glOrtho(0, 640, 0, 360, 1, -1);
    gVitaFacade.glMatrixMode(GL_MODELVIEW);
    gVitaFacade.glLoadIdentity();

    gVitaFacade.glPushMatrix();
    {
        cgGLEnableTextureParameter( cgTexture );
        gVitaFacade.glTranslatef(100, -50, 0);
        float* mOpenGLMat = gVitaFacade.CalcModelViewProjMatrix().GetOpenGLMat();
        cgSetMatrixParameterfc(cgModelViewProj, mOpenGLMat);

        // glTexCoordPointer(texCoordsPerVert, GL_FLOAT, vertexStride, texCoordArray);
        // glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        cgGLEnableClientState(cgTexCoord);
        cgGLSetParameterPointer(cgTexCoord, 2, GL_FLOAT, sizeof(VertexDef), &verts[0].u);

        // glColorPointer(channelsPerVert, GL_FLOAT, vertexStride, colorArray);
        // glEnableClientState(GL_COLOR_ARRAY);
        cgGLEnableClientState(cgColor);
        cgGLSetParameterPointer(cgColor, 4, GL_FLOAT, sizeof(VertexDef), &verts[0].color);

        // glVertexPointer(coordsPerVert, GL_FLOAT, vertexStride, vertArray);
        // glEnableClientState(GL_VERTEX_ARRAY);
        cgGLEnableClientState(cgPosition);
        cgGLSetParameterPointer(cgPosition, 4, GL_FLOAT, sizeof(VertexDef), verts);


        int numVerts = 3;
        glDrawArrays(GL_TRIANGLES, 0, numVerts);
    }
    gVitaFacade.glPopMatrix();
    cgGLDisableProfile(cgVertexProfile);
    cgGLDisableProfile(cgFragmentProfile);
}

void Main::OnUpdate()
{


    // OpenGL Standard [FINE]
    //DrawImmediateFixedOpenGL();

    // OpenGL Manual [FINE]
    //DrawManualTransformFixedOpenGL();

    // Shader Immediate Mode
    //DrawShaderImmediateMode();

    // Fixed Arrays mode
    //DrawArraysFixed();

    // Shader Arrays mode
   // DrawArraysShader();

    DrawStructShader();

}



int main(int argc, char *argv[])
{

	Main main;
 	return main.Execute();
}