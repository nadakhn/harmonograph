#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

#define NUMBER_OF_VERTICES 1000 // Define the number of vertices

int main(void)
{
    GLFWwindow *window;

    // Initialize the library
    if (!glfwInit())
    {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "Harmonograph", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    float animationTime = 0.0f; // Initialize animation time

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Render OpenGL here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the viewing transformation
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

        // Draw the harmonograph
        glBegin(GL_LINE_STRIP);
        glColor3f(1.0f, 1.0f, 1.0f); // Set line color to white

        std::vector<float> vertices;
        float x, y, z, time;
        const float amplitude = 0.5f;
        const float damping = 0.02f;
        const float freq1 = 3.001f,
                    freq2 = 2.0f,
                    freq3 = 3.0f,
                    freq4 = 2.0f,
                    freq5 = 3.0f,
                    freq6 = 2.0f,
                    damping1 = 0.004f,
                    damping2 = 0.0065f,
                    damping3 = 0.008f,
                    damping4 = 0.019f,
                    damping5 = 0.012f,
                    damping6 = 0.005f,
                    phase1 = 0,
                    phase2 = 0,
                    phase3 = M_PI / 2,
                    phase4 = 3 * M_PI / 2,
                    phase5 = M_PI / 4,
                    phase6 = 2 * M_PI;

        for (time = 0; time < animationTime; time += 0.01)
        {
            x = amplitude * sin(time * freq1 + phase1) * exp(-damping1 * time) + amplitude * sin(time * freq2 + phase2) * exp(-damping2 * time);
            y = amplitude * sin(time * freq3 + phase3) * exp(-damping3 * time) + amplitude * sin(time * freq4 + phase4) * exp(-damping4 * time);
            z = amplitude * sin(time * freq5 + phase5) * exp(-damping5 * time) + amplitude * sin(time * freq6 + phase6) * exp(-damping6 * time);

            glVertex3f(x, y, z);
        }

        glEnd();

        // Increment animation time
        animationTime += 0.01f;

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

