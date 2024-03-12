

#ifndef SHADERSOURCE_H
#define SHADERSOURCE_H

/******************************************************************************/
/******************************   Default Shader ******************************/
/******************************************************************************/


const char* vertexShaderSource = 
        "#version 330 core\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "uniform vec3 meshColor;\n"
        "layout (location = 0) in vec3 aPos;\n"
        "out vec3 oColor;\n"
        "void main()\n"
        "{\n"
        "gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "oColor = meshColor;\n"
        "}\n";



const char *fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 oColor;\n"
        "void main()\n"
        "{\n"
        "FragColor = vec4(oColor, 1.0f);\n"
        "}\n\0";



#endif //SHADERSOURCE_H
