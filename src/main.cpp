#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <list>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// #include "shader.h" #TODO: dian to move this back to a seperate file
#include "shaderSource.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

using namespace std;

#define _ROTATE_FACTOR 0.005f
#define _SCALE_FACTOR 0.005f
#define _TRANS_FACTOR 0.003f
#define _Z_NEAR 0.001f
#define _Z_FAR 100.0f

#define NUMBER_OF_VERTICES 10000 // Define the number of vertices

/***********************************************************************/
/**************************   global variables   ***********************/
/***********************************************************************/

// declaration
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos);

// Window size
unsigned int winWidth = 1200;
unsigned int winHeight = 1200;

// Camera
glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 2.5f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
float camera_fovy = 45.0f;
glm::mat4 projection;

// Mouse interaction
bool leftMouseButtonHold = false;
bool isFirstMouse = true;
float prevMouseX;
float prevMouseY;
glm::mat4 modelMatrix = glm::mat4(1.0f);

// Colour
glm::vec3 meshColor;

// Animation Control
bool isAnimating = true;
float animationTime = 0.0f;

// Parameters
float amplitude = 0.5f;
float damping = 0.02f;
float animationIncrement = 0.01f;
float *freqPtr1 = new float[3];
float *freqPtr2 = new float[3];
float *dampPtr1 = new float[3];
float *dampPtr2 = new float[3];
float *phasePtr1 = new float[3];
float *phasePtr2 = new float[3];

///=========================================================================================///
///                             Functions for Rendering 3D Model
///=========================================================================================///

// Mesh color table
glm::vec3 colorTable[4] =
    {
        glm::vec3(0.6, 1.0, 0.6),
        glm::vec3(1.0, 0.6, 0.6),
        glm::vec3(0.6, 0.6, 1.0),
        glm::vec3(1.0, 1.0, 0.6)};

///=========================================================================================///
///                            Functions for Manipulating 3D Model
///=========================================================================================///

void RotateModel(float angle, glm::vec3 axis)
{
    glm::vec3 rotateCenter = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

    glm::mat4 rotateMatrix = glm::mat4(1.0f);
    rotateMatrix = glm::translate(rotateMatrix, rotateCenter);
    rotateMatrix = glm::rotate(rotateMatrix, angle, axis);
    rotateMatrix = glm::translate(rotateMatrix, -rotateCenter);

    modelMatrix = rotateMatrix * modelMatrix;
}

void TranslateModel(glm::vec3 transVec)
{
    glm::mat4 translateMatrix = glm::mat4(1.0f);
    translateMatrix = glm::translate(translateMatrix, transVec);

    modelMatrix = translateMatrix * modelMatrix;
}

void ScaleModel(float scale)
{
    glm::vec3 scaleCenter = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

    glm::mat4 scaleMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::translate(scaleMatrix, scaleCenter);
    scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scale, scale, scale));
    scaleMatrix = glm::translate(scaleMatrix, -scaleCenter);

    modelMatrix = scaleMatrix * modelMatrix;
}

///=========================================================================================///
///                                    Callback Functions
///=========================================================================================///

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.

    glViewport(0, 0, width, height);

    winWidth = width;
    winHeight = height;
}
// glfw: whenever the mouse button is clicked, this callback is called
// ---------------------------------------------------------
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    auto &io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard)
    {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        leftMouseButtonHold = true;
    }
    else
    {
        leftMouseButtonHold = false;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yOffset)
{
    float scale = 1.0f + _SCALE_FACTOR * yOffset;

    ScaleModel(scale);
}

// glfw: whenever the cursor moves, this callback is called
// ---------------------------------------------------------
void cursor_pos_callback(GLFWwindow *window, double mouseX, double mouseY)
{
    float dx, dy;
    float nx, ny, scale, angle;

    if (leftMouseButtonHold)
    {
        if (isFirstMouse)
        {
            prevMouseX = mouseX;
            prevMouseY = mouseY;
            isFirstMouse = false;
        }

        else
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                float dx = _TRANS_FACTOR * (mouseX - prevMouseX);
                float dy = -1.0f * _TRANS_FACTOR * (mouseY - prevMouseY); // reversed since y-coordinates go from bottom to top

                prevMouseX = mouseX;
                prevMouseY = mouseY;

                TranslateModel(glm::vec3(dx, dy, 0));
            }

            else
            {
                float dx = mouseX - prevMouseX;
                float dy = -(mouseY - prevMouseY); // reversed since y-coordinates go from bottom to top

                prevMouseX = mouseX;
                prevMouseY = mouseY;

                // Rotation
                nx = -dy;
                ny = dx;
                scale = sqrt(nx * nx + ny * ny);

                // We use "ArcBall Rotation" to compute the rotation axis and angle based on the mouse motion
                nx = nx / scale;
                ny = ny / scale;
                angle = scale * _ROTATE_FACTOR;

                RotateModel(angle, glm::vec3(nx, ny, 0.0f));
            }
        }
    }

    else
    {
        isFirstMouse = true;
    }
}

///=========================================================================================///
///                                  Vertex Normals + Surfaces
///=========================================================================================///

// Function to calculate normals for each vertex
std::vector<glm::vec3> calculateNormals(const std::vector<float> &vertices)
{
    std::vector<glm::vec3> normals;
    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        // Calculate the direction from the current point to the next point
        glm::vec3 currentPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 nextPoint;
        if (i + 3 < vertices.size())
        {
            nextPoint = glm::vec3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
        }
        else
        {
            // If it's the last point, use the previous point as the next point
            nextPoint = glm::vec3(vertices[i - 3], vertices[i - 2], vertices[i - 1]);
        }
        glm::vec3 direction = nextPoint - currentPoint;
        // Normalize the direction to get the normal
        glm::vec3 normal = glm::normalize(direction);
        normals.push_back(normal);
    }
    return normals;
}

void generateSurface(const std::vector<glm::vec3> &lineSegments, std::vector<glm::vec3> &surfaceVertices, std::vector<glm::vec3> &surfaceNormals)
{
    // Assuming lineSegments has at least two line segments
    for (size_t i = 0; i < lineSegments.size() - 1; i += 2)
    {
        // add vertices for rectangle formed by sequential pair of line segments
        surfaceVertices.push_back(lineSegments[i]);
        surfaceVertices.push_back(lineSegments[i + 1]);
        
        // calculate the normal for the line segments
        glm::vec3 edge = lineSegments[i + 1] - lineSegments[i];
        glm::vec3 normal(-edge.y, edge.x, 0.0f);
        normal = glm::normalize(normal);
        
        // push the same normal for both vertices
        surfaceNormals.push_back(normal);
        surfaceNormals.push_back(normal);
    }
}


void extrudeSurface(const std::vector<glm::vec3> &surfaceVertices, const std::vector<glm::vec3> &surfaceNormals, float extrusionDistance, std::vector<glm::vec3> &extrudedVertices, std::vector<unsigned int> &topSurfaceIndices, std::vector<unsigned int> &bottomSurfaceIndices, std::vector<unsigned int> &frontSurfaceIndices, std::vector<unsigned int> &endSurfaceIndices,std::vector<unsigned int> &sideSurface1Indices, std::vector<unsigned int> &sideSurface2Indices )
{
    size_t numVertices = surfaceVertices.size();
    size_t numExtrudedVertices = 2 * numVertices;

    extrudedVertices.reserve(numExtrudedVertices);

    // Create vertices for the top surface of the extrusion
    for (size_t i = 0; i < numVertices; ++i)
    {
        // Extrude each vertex along its normal direction
        glm::vec3 extrudedVertex = surfaceVertices[i] + extrusionDistance * surfaceNormals[i];
        extrudedVertices.push_back(extrudedVertex);
    }

    // Create vertices for the bottom surface of the extrusion
    for (size_t i = 0; i < numVertices; ++i)
    {
        // Bottom surface vertices are the same as the original surface vertices but at a lower position
        glm::vec3 bottomVertex = surfaceVertices[i];
        extrudedVertices.push_back(bottomVertex);
    }

    // Create indices for the top + bottom surface, we get rid of last 4 indices due to normal abnormality
    for (size_t i = 0; i < numVertices-4; ++i)
    {
        topSurfaceIndices.push_back(i);
    }

    for (size_t i = 0; i < numVertices-4; ++i)
    {
        bottomSurfaceIndices.push_back(numVertices + i);
    }
    
    //create indices for front + end surface
    frontSurfaceIndices.push_back(0);
    frontSurfaceIndices.push_back(numVertices);
    frontSurfaceIndices.push_back(1);
    frontSurfaceIndices.push_back(numVertices+1);

    endSurfaceIndices.push_back(numVertices-4-1);
    endSurfaceIndices.push_back(2*(numVertices)-1-4);
    endSurfaceIndices.push_back(numVertices-4-2);
    endSurfaceIndices.push_back(2*(numVertices)-2-4);

    //create indices for side faces
    for (size_t i=0; i<numVertices-4; i += 2){
        sideSurface1Indices.push_back(i);
        sideSurface1Indices.push_back(i+numVertices);
    }
    for (size_t i=1; i<numVertices-4; i += 2){
        sideSurface2Indices.push_back(i);
        sideSurface2Indices.push_back(i+numVertices);
    }
    
}





///=========================================================================================///
///                                      Harmonograph Function
///=========================================================================================///

std::vector<float> drawHarmonograph(float animationTime, bool renderSurface)
{
    // Buffers for harmonograph line segments
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Draw the harmonograph
    std::vector<float> vertices;
    float x, y, z, time;

    for (time = 0; time < animationTime; time += 0.01)
    {
        x = amplitude * sin(time * freqPtr1[0] + phasePtr1[0]) * exp(-dampPtr1[0] * time) + amplitude * sin(time * freqPtr1[1] + phasePtr1[1]) * exp(-dampPtr1[1] * time);
        y = amplitude * sin(time * freqPtr1[2] + phasePtr1[2]) * exp(-dampPtr1[2] * time) + amplitude * sin(time * freqPtr2[0] + phasePtr2[0]) * exp(-dampPtr2[0] * time);
        z = amplitude * sin(time * freqPtr2[1] + phasePtr2[1]) * exp(-dampPtr2[1] * time) + amplitude * sin(time * freqPtr2[2] + phasePtr1[2]) * exp(-dampPtr2[2] * time);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
    }

    // Upload vertices data to GPU
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // Set attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Draw line segments
    glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);

    if (renderSurface)
    {
        std::vector<glm::vec3> controlPoints;
        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            controlPoints.push_back(glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]));
        }

        // Calculate + render normals
        std::vector<glm::vec3> normals = calculateNormals(vertices);

        // buffers for normals
        unsigned int normalVBO, normalVAO;
        glGenVertexArrays(1, &normalVAO);
        glGenBuffers(1, &normalVBO);
        glBindVertexArray(normalVAO);
        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);

        // Store normal vectors' data in VBO
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

        // Set vertex attribute pointer for position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glEnableVertexAttribArray(0);

        // Draw the normal vectors
        // Calculate vertices for line segments (control points to normals)
        std::vector<glm::vec3> lineSegments;
        for (size_t i = 0; i < controlPoints.size(); ++i)
        {
            lineSegments.push_back(controlPoints[i]);
            lineSegments.push_back(controlPoints[i] + 0.5f * normals[i]); // nada TODO: change the length of the normal in relation to the input amp
        }

        // Store line segment vertices' data in VBO
        glBufferData(GL_ARRAY_BUFFER, lineSegments.size() * sizeof(glm::vec3), &lineSegments[0], GL_STATIC_DRAW);

        // Set vertex attribute pointer for line segment vertices
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glEnableVertexAttribArray(0);

        // -- uncomment the 3 lines belw if you want to draw the line segments
        // glDrawArrays(GL_LINES, 0, lineSegments.size());
        // glPointSize(5.0f);                              // Set point size for better visibility
        // glDrawArrays(GL_POINTS, 0, normals.size() * 2); // this draws out the normal end points


        // generate surface vertices and normals and extrusions
        std::vector<glm::vec3> surfaceVertices;
        std::vector<glm::vec3> surfaceNormals;
        generateSurface(lineSegments, surfaceVertices, surfaceNormals);

        // Extrude surface
        std::vector<glm::vec3> extrudedVertices;
        float extrusionDistance = 0.1; // adjust extrusion here
        std::vector<unsigned int> bottomSurfaceIndices;
        std::vector<unsigned int> topSurfaceIndices;
        std::vector<unsigned int> frontSurfaceIndices;
        std::vector<unsigned int> endSurfaceIndices;
        std::vector<unsigned int> sideSurface1Indices;
        std::vector<unsigned int> sideSurface2Indices;


        extrudeSurface(surfaceVertices, surfaceNormals, extrusionDistance, extrudedVertices, topSurfaceIndices, bottomSurfaceIndices, frontSurfaceIndices, endSurfaceIndices, sideSurface1Indices, sideSurface2Indices);

        // Create and bind VAO and VBO for extruded surface
        unsigned int extrudedVAO, extrudedVBO;
        glGenVertexArrays(1, &extrudedVAO);
        glGenBuffers(1, &extrudedVBO);
        glBindVertexArray(extrudedVAO);
        glBindBuffer(GL_ARRAY_BUFFER, extrudedVBO);
        glBufferData(GL_ARRAY_BUFFER, extrudedVertices.size() * sizeof(glm::vec3), extrudedVertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glEnableVertexAttribArray(0);

        // Create and bind VBOs for surface indices
        unsigned int topSurfaceIndexVBO, bottomSurfaceIndexVBO, frontSurfaceIndexVBO, endSurfaceIndexVBO, sideSurface1IndexVBO, sideSurface2IndexVBO;
        glGenBuffers(1, &topSurfaceIndexVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topSurfaceIndexVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, topSurfaceIndices.size() * sizeof(unsigned int), topSurfaceIndices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &bottomSurfaceIndexVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomSurfaceIndexVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, bottomSurfaceIndices.size() * sizeof(unsigned int), bottomSurfaceIndices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &frontSurfaceIndexVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frontSurfaceIndexVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, frontSurfaceIndices.size() * sizeof(unsigned int), frontSurfaceIndices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &endSurfaceIndexVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, endSurfaceIndexVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, endSurfaceIndices.size() * sizeof(unsigned int), endSurfaceIndices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &sideSurface1IndexVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideSurface1IndexVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sideSurface1Indices.size() * sizeof(unsigned int), sideSurface1Indices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &sideSurface2IndexVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideSurface2IndexVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sideSurface2Indices.size() * sizeof(unsigned int), sideSurface2Indices.data(), GL_STATIC_DRAW);

        // Draw the surfaces
        glBindVertexArray(extrudedVAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topSurfaceIndexVBO);
        glDrawElements(GL_TRIANGLE_STRIP, topSurfaceIndices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(extrudedVAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomSurfaceIndexVBO);
        glDrawElements(GL_TRIANGLE_STRIP, bottomSurfaceIndices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(extrudedVAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frontSurfaceIndexVBO);
        glDrawElements(GL_TRIANGLE_STRIP, frontSurfaceIndices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(extrudedVAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, endSurfaceIndexVBO);
        glDrawElements(GL_TRIANGLE_STRIP, endSurfaceIndices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(extrudedVAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideSurface1IndexVBO);
        glDrawElements(GL_TRIANGLE_STRIP, sideSurface1Indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(extrudedVAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideSurface2IndexVBO);
        glDrawElements(GL_TRIANGLE_STRIP, sideSurface2Indices.size(), GL_UNSIGNED_INT, 0);

        // Cleanup after rendering the extruded surface
        glDeleteVertexArrays(1, &extrudedVAO);
        glDeleteBuffers(1, &extrudedVBO);
        glDeleteBuffers(1, &topSurfaceIndexVBO);
        glDeleteBuffers(1, &bottomSurfaceIndexVBO);

        
        // // create + bind VAO and VBO for surface
        // unsigned int surfaceVAO, surfaceVBO, surfaceNormalVBO;
        // glGenVertexArrays(1, &surfaceVAO);
        // glGenBuffers(1, &surfaceVBO);
        // glGenBuffers(1, &surfaceNormalVBO);
        // glBindVertexArray(surfaceVAO);
        // glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);
        // glBufferData(GL_ARRAY_BUFFER, surfaceVertices.size() * sizeof(glm::vec3), &surfaceVertices[0], GL_STATIC_DRAW);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        // glEnableVertexAttribArray(0);

    
        // // Bind normal VBO - for surface shading
        // glBindBuffer(GL_ARRAY_BUFFER, surfaceNormalVBO);
        // glBindBuffer(GL_ARRAY_BUFFER, surfaceNormalVBO);
        // glBufferData(GL_ARRAY_BUFFER, surfaceNormals.size() * sizeof(glm::vec3), &surfaceNormals[0], GL_STATIC_DRAW);
        // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        // glEnableVertexAttribArray(1);

    
        // // Draw the surface
        // glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, surfaceVertices.size());


        

        // // Cleanup after rendering the surface
        // glDeleteVertexArrays(1, &surfaceVAO);
        // glDeleteBuffers(1, &surfaceVBO);
        // glDeleteBuffers(1, &surfaceNormalVBO);
    }


    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    return vertices;
    
}

///=========================================================================================///
///                                      Main Function
///=========================================================================================///

int main(void)
{
    GLFWwindow *window;

    // Initialize the library
    if (!glfwInit())
    {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Request OpenGL 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(winWidth, winHeight, "Harmonograph", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    ///// setting up the shaders
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // correct resize
    glfwSetScrollCallback(window, scroll_callback);                    // scale
    glfwSetCursorPosCallback(window, cursor_pos_callback);             // translate OR rotate
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // tell GLFW to capture the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // configure multi-sampling
    glEnable(GL_MULTISAMPLE);

    //// build and compile our shader program
    //// ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return -1;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return -1;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
        return -1;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set uniform locations
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int meshColorLoc = glGetUniformLocation(shaderProgram, "meshColor");
    int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Enable shader
    glUseProgram(shaderProgram);

    // Set attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // shader stuff ends here

    float animationTime = 0.0f; // Initialize animation time
    printf("%s\n", glGetString(GL_VERSION));

    const char *glsl_version = "#version 330";

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Initializer Frequncy
    freqPtr1[0] = 3.001f;
    freqPtr1[1] = 2.0f;
    freqPtr1[2] = 3.0f;

    freqPtr2[0] = 2.0f;
    freqPtr2[1] = 3.0f;
    freqPtr2[2] = 2.0f;

    dampPtr1[0] = 0.004f;
    dampPtr1[1] = 0.0065f;
    dampPtr1[2] = 0.008f;

    dampPtr2[0] = 0.019f;
    dampPtr2[1] = 0.012f;
    dampPtr2[2] = 0.005f;

    phasePtr1[0] = 0;
    phasePtr1[1] = 0;
    phasePtr1[2] = M_PI / 2;

    phasePtr2[0] = 3 * M_PI / 2;
    phasePtr2[1] = M_PI / 4;
    phasePtr2[2] = 2 * M_PI;

    int freeze = 1;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Process inputs
        processInput(window);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        static int clicked = 0;
        ImGui::Begin("Harmonograph Controls");

        ImGui::SliderFloat("Speed", &animationIncrement, 0.01f, 0.5f, "Speed: %.4f");

        ImGui::SliderFloat("Amplitude", &amplitude, 0.01f, 5.0f, "Amplitude: %.4f");

        ImGui::DragFloat3("Frequency Set 1", freqPtr1, 0.01f, 0.0f, 1.0f, "%.4f");
        ImGui::DragFloat3("Frequency Set 2", freqPtr2, 0.01f, 0.0f, 1.0f, "%.4f");

        ImGui::DragFloat3("Damp Set 1", dampPtr1, 0.01f, 0.0f, 1.0f, "%.4f");
        ImGui::DragFloat3("Damp Set 2", dampPtr2, 0.01f, 0.0f, 1.0f, "%.4f");

        ImGui::DragFloat3("Phase Set 1", phasePtr1, 0.01f, 0.0f, 1.0f, "%.4f");
        ImGui::DragFloat3("Phase Set 2", phasePtr2, 0.01f, 0.0f, 1.0f, "%.4f");

        if (ImGui::Button("Freeze"))
        {
            freeze++;
            isAnimating = !isAnimating;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            animationTime = 0;
            freeze = 1;
            isAnimating = true;
        }
        ImGui::SameLine();
        ImGui::Button("Preset 1");
        ImGui::SameLine();
        ImGui::Button("Preset 2");
        ImGui::SameLine();
        ImGui::Button("Preset 3");
        ImGui::End();

        // Render OpenGL here
        glClearColor(0.95f, 0.95f, 0.95f, 1.0f); // change background colour
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the viewing transformation
        projection = glm::perspective(glm::radians(camera_fovy), (float)winWidth / (float)winHeight, _Z_NEAR, _Z_FAR);
        glm::mat4 view = glm::lookAt(camera_position, camera_target, camera_up);

        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniform3fv(meshColorLoc, 1, &colorTable[0][0]);
        glUniform3fv(viewPosLoc, 1, &camera_position[0]);

        std::vector<float> vertices = drawHarmonograph(animationTime, !isAnimating);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (freeze & 1)
        {
            animationTime += animationIncrement;
        }

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Deallocate all resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}