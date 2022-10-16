#include "VRT.h"
#include "3rd/glew-2.1.0/GL/glew.h"
#include <iostream>
#define INTENSITY 0.05

const float vertices[] = {
        -1.0,-1.0,0.0,
        1.0,-1.0,0.0,
        1.0,1.0,0.0,
        -1.0,1.0,0.0,
};

const UInt indices[] = {
    0,1,2,
    0,2,3
};

UInt imageSize[] = { 3, 3, 3 };

float imageData[3][3][3] = {
    {{INTENSITY, 0.0, INTENSITY}, {INTENSITY, 0.0, INTENSITY}, {INTENSITY, 0.0, INTENSITY}},
    {{0.0, 0.0, 0.0}, {0.0, INTENSITY, 0.0}, {0.0, 0.0, 0.0}},
    {{INTENSITY, 0.0, INTENSITY}, {0.0, 0.0, 0.0}, {INTENSITY, 0.0, INTENSITY}}
};
VRT::~VRT()
{
    Finish();
}

void VRT::Init()
{
	InitShape();
    InitTexture();
    InitShader();
    InitRenderTarget();
    glViewport(0, 0, m_iTargetSize[0], m_iTargetSize[1]);
}
void VRT::Render()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(m_iShader);
    glBindTextureUnit(0, m_iVolume);
	glBindVertexArray(m_iVAO);
	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}
void VRT::RenderToTarget(float* _pData, UInt _x, UInt _y)
{
    if (_x > m_iTargetSize[0] && _y > m_iTargetSize[1])
    {
        _pData = nullptr;
        return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_iFBO);
    Render();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_iFBO);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, m_iTargetSize[0], m_iTargetSize[1], GL_RED, GL_FLOAT, _pData);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
void VRT::Finish()
{
    glDeleteBuffers(1, &m_iVBO);
    glDeleteBuffers(1, &m_iEBO);
    glDeleteVertexArrays(1, &m_iVAO);

    glDeleteProgram(m_iShader);

    glDeleteFramebuffers(1, &m_iFBO);
    glDeleteRenderbuffers(2, m_iRBOs);
}
void VRT::GetTargetSize(UInt& x, UInt& y)
{
    x = m_iTargetSize[0];
    y = m_iTargetSize[1];
}
void VRT::InitShape()
{
    //OpenGL 4.5 and above
	glCreateBuffers(1, &m_iVBO);
	glNamedBufferData(m_iVBO, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glCreateBuffers(1, &m_iEBO);
	glNamedBufferData(m_iEBO, sizeof(indices), indices, GL_STATIC_DRAW);

	glCreateVertexArrays(1, &m_iVAO);
	
	glVertexArrayElementBuffer(m_iVAO, m_iEBO);

    UInt vaoBindingIndex = 0;
    glVertexArrayVertexBuffer(m_iVAO, vaoBindingIndex, m_iVBO, 0, 3 * sizeof(float));
    UInt vaoAttributeSlot = 0;
    glVertexArrayAttribBinding(m_iVAO, vaoAttributeSlot, vaoBindingIndex);
    glVertexArrayAttribFormat(m_iVAO, vaoAttributeSlot, 3, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(m_iVAO, vaoAttributeSlot);

    //OpenGL 4.4 and below
    /*glGenVertexArrays(1, &m_iVAO);
    glGenBuffers(1, &m_iVBO);
    glGenBuffers(1, &m_iEBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(m_iVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);*/
}
void VRT::InitShader()
{
	const char* vertexShaderSource = 
        "#version 460 core                                         \n"
        "layout(location = 0) in vec3 aPos;                        \n"
        "out vec2 texCoord;                                        \n"
        "void main()                                               \n"
        "{                                                         \n"
        "    texCoord = vec2(aPos.x + 1.0, aPos.y + 1.0) * 0.5;    \n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);      \n"
        "}\n\0";
	const char* fragmentShaderSource = 
        "#version 460 core                                    \n"
        "#define RAY_STEP 10                                  \n"
        "uniform sampler3D volume;                            \n"
        "in vec2 texCoord;                                    \n"
        "out vec4 FragColor;                                  \n"
                                                              
        "void main()                                          \n"
        "{                                                    \n"
        "    vec3 StartPos = vec3(texCoord.xy, 0.0);          \n"
        "    vec3 Step = vec3(0.0, 0.0, 0.1);                 \n"
        "    vec3 CurrentPos = StartPos;                      \n"
        "    float res = 0.0;                                 \n"
        "    for (int i = 0; i < 10; i++)                     \n"
        "    {                                                \n"
        "        float value = texture(volume, CurrentPos).r; \n"
        "        res += value;                                \n"
        "        CurrentPos += Step;                          \n"
        "    }                                                \n"
        "    FragColor = vec4(res);                           \n"
        "}\n\0";
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
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
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
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    m_iShader = glCreateProgram();
    glAttachShader(m_iShader, vertexShader);
    glAttachShader(m_iShader, fragmentShader);
    glLinkProgram(m_iShader);
    // check for linking errors
    glGetProgramiv(m_iShader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_iShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
void VRT::InitTexture()
{
    glCreateTextures(GL_TEXTURE_3D, 1, &m_iVolume);
    glTextureStorage3D(m_iVolume, 1, GL_R32F, imageSize[0], imageSize[1], imageSize[2]);
    glTextureSubImage3D(m_iVolume,
        0,
        0, 0, 0,
        imageSize[0], imageSize[1], imageSize[2],
        GL_RED, GL_FLOAT,
        imageData);
}
void VRT::InitRenderTarget()
{
    m_iTargetSize[0] = 512;
    m_iTargetSize[1] = 512;

    //render to texture
    //glCreateTextures(GL_TEXTURE_2D, 1, &m_iRenderTarget);
    //glTextureStorage2D(m_iRenderTarget, 1, GL_R32F, m_iTargetSize[0], m_iTargetSize[1]);
    
    //render to buffer
    glCreateRenderbuffers(2, m_iRBOs);
    glNamedRenderbufferStorage(m_iRBOs[0], GL_R32F, m_iTargetSize[0], m_iTargetSize[1]);
    glNamedRenderbufferStorage(m_iRBOs[1], GL_DEPTH_COMPONENT, m_iTargetSize[0], m_iTargetSize[1]);
    glCreateFramebuffers(1, &m_iFBO);
    glNamedFramebufferRenderbuffer(m_iFBO, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_iRBOs[0]);
    glNamedFramebufferRenderbuffer(m_iFBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iRBOs[1]);  
}