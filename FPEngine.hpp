#ifndef A4_A4_ENGINE_HPP
#define A4_A4_ENGINE_HPP

#include <CSCI441/ArcballCam.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>
#include <CSCI441/ModelLoader.hpp>
#include <CSCI441/FreeCam.hpp>
#include <vector>

class FPEngine : public CSCI441::OpenGLEngine {
public:
    //***************************************************************************
    // Engine Interface

    FPEngine();

    void run() final;

    //***************************************************************************
    // Event Handlers

    /// \desc handle any key events inside the engine
    /// \param key key as represented by GLFW_KEY_ macros
    /// \param action key event action as represented by GLFW_ macros
    void handleKeyEvent(GLint key, GLint action);

    /// \desc handle any mouse button events inside the engine
    /// \param button mouse button as represented by GLFW_MOUSE_BUTTON_ macros
    /// \param action mouse event as represented by GLFW_ macros
    void handleMouseButtonEvent(GLint button, GLint action);

    /// \desc handle any cursor movement events inside the engine
    /// \param currMousePosition the current cursor position
    void handleCursorPositionEvent(glm::vec2 currMousePosition);

    /// \desc handle any scroll events inside the engine
    /// \param offset the current scroll offset
    void handleScrollEvent(glm::vec2 offset);

    /// \desc value off-screen to represent mouse has not begun interacting with window yet
    static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;
    glm::vec3 position;

private:
    //***************************************************************************
    // Engine Setup

    void _setupGLFW() final;
    void _setupOpenGL() final;
    void _setupShaders() final;
    void _setupTextureShaders() ;
    void _setupBuffers() final;
    void _setupTextures() final;
    void _setupScene() final;

    //***************************************************************************
    // Engine Cleanup

    void _cleanupScene() final;
    void _cleanupTextures() final;
    void _cleanupBuffers() final;
    void _cleanupShaders() final;

    //***************************************************************************
    // Engine Rendering & Updating

    /// \desc draws everything to the scene from a particular point of view
    /// \param viewMtx the current view matrix for our camera
    /// \param projMtx the current projection matrix for our camera
    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx);
    /// \desc handles moving our FreeCam as determined by keyboard input
    void _updateScene();

    //***************************************************************************
    // Input Tracking (Keyboard & Mouse)

    /// \desc tracks the number of different keys that can be present as determined by GLFW
    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
    /// \desc boolean array tracking each key state.  if true, then the key is in a pressed or held
    /// down state.  if false, then the key is in a released state and not being interacted with
    GLboolean _keys[NUM_KEYS];

    /// \desc last location of the mouse in window coordinates
    glm::vec2 _mousePosition;
    /// \desc current state of the left mouse button
    GLint _leftMouseButtonState;

    //***************************************************************************
    // Camera Information

    /// \desc the arcball camera in our world
    CSCI441::ArcballCam* _arcballCam;
    CSCI441::FreeCam* _fpCam;
    CSCI441::FreeCam* _bezCam;


    //***************************************************************************
    // Illumination Information

    /// \desc type of the light
    /// \desc 0 - point
    /// \desc 1 - directional
    /// \desc 2 - spot
    GLuint _lightType;
    /// \desc position of the light for point or spotlight
    glm::vec3 _lightPos;
    /// \desc direction of the light for directional or spotlight
    glm::vec3 _lightDir;
    /// \desc angle of our spotlight
    GLfloat _lightAngle;

    //***************************************************************************
    // VAO & Object Information

    /// \desc total number of VAOs in our scene
    static constexpr GLuint NUM_VAOS = 4;
    /// \desc used to index through our VAO/VBO/IBO array to give named access
    enum VAO_ID {
        /// \desc the platform that represents our ground for everything to appear on
        PLATFORM = 0,
        SKYBOX = 3
    };
    /// \desc VAO for our objects
    GLuint _vaos[NUM_VAOS];
    /// \desc VBO for our objects
    GLuint _vbos[NUM_VAOS];
    /// \desc IBO for our objects
    GLuint _ibos[NUM_VAOS];
    /// \desc the number of points that make up our VAO
    GLsizei _numVAOPoints[NUM_VAOS];

    //***************************************************************************
    // Texture Information

    /// \desc total number of textures in our scene
    static constexpr GLuint NUM_TEXTURES = 6;
    /// \desc used to index through our texture array to give named access
    enum TEXTURE_ID {
        TOP = 0,
        RIGHT = 1,
        LEFT = 2,
        FRONT= 3,
        BACK = 4,
        BOTTOM = 5
    };
    /// \desc texture handles for our textures
    GLuint _texHandles[NUM_TEXTURES];

    /// \desc shader program that performs lighting
    CSCI441::ShaderProgram* _textureShaderProgram = nullptr;   // the wrapper for our shader program
    /// \desc stores the locations of all of our shader uniforms
    struct TextureShaderUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;
        /// \desc map of texture
        GLint textureMap;
        GLint sunDir;
        GLint sunColor;
        GLint cameraPos;
        GLint inverseVPMatrix;
        GLint doShading;
        GLint laserColor[16];
        GLint laserPos[16];


    } _textureShaderUniformLocations;
    /// \desc stores the locations of all of our shader attributes
    struct TextureShaderAttributeLocations {
        /// \desc vertex position location
        GLint vPos;
        /// \desc vertex normal location
        GLint vNormal;
        GLint vTexCoord;
        GLint texCoord;
    } _textureShaderAttributeLocations;

    CSCI441::ShaderProgram* _glitchShaderProgram = nullptr;   // the wrapper for our shader program
    /// \desc stores the locations of all of our shader uniforms
    struct GlitchShaderUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;

    } _glitchShaderUniformLocations;
    /// \desc stores the locations of all of our shader attributes
    struct GlitchShaderAttributeLocations {
        /// \desc vertex position location
        GLint vPos;
    } _glitchShaderAttributeLocations;
    //***************************************************************************
    // Private Helper Functions

    /// \desc stores the attributes for a single texture
    struct VertexTextured {
        GLfloat x, y, z;
        GLfloat s, t;
    };

    GLuint _maxLasers = 16;
    GLuint _nextLaser;
    GLfloat _laserSpeed;
        struct Lasers {
            glm::vec4 laserColor;
            glm::vec3 laserPos;
            glm::vec3 laserDir;
        } _lasers[16];

        struct Enemies {
            glm::vec3 position;
            glm::vec3 direction;
        };
        std::vector<Enemies> _enemies;

    /// \desc creates the platform object
    /// \param [in] vao VAO descriptor to bind
    /// \param [in] vbo VBO descriptor to bind
    /// \param [in] ibo IBO descriptor to bind
    /// \param [out] numVAOPoints sets the number of vertices that make up the IBO array
    void _createPlatform(GLuint vao, GLuint vbo, GLuint ibo, GLsizei &numVAOPoints) const;

    /// \desc creates the textured quad object
    /// \param [in] vao VAO descriptor to bind
    /// \param [in] vbo VBO descriptor to bind
    /// \param [in] ibo IBO descriptor to bind
    /// \param [out] numVAOPoints sets the number of vertices that make up the IBO array
    void _createQuad(GLuint vao, GLuint vbo, GLuint ibo, GLsizei &numVAOPoints) const;

    //***************************************************************************
    // Shader Program Information

    /// \desc shader program that performs Gouraud Shading with the Phong Illumination Model
    CSCI441::ShaderProgram* _gouraudShaderProgram = nullptr;
    /// \desc stores the locations of all of our shader uniforms
    struct GouraudShaderProgramUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;
        /// \desc model matrix location
        GLint modelMatrix;
        /// \desc normal matrix location
        GLint normalMatrix;
        /// \desc camera position location
        GLint eyePos;
        /// \desc light position location - used for point/spot
        GLint lightPos;
        /// \desc light direction location - used for directional/spot
        GLint lightDir;
        /// \desc light cone angle location - used for spot
        GLint lightCutoff;
        /// \desc color of the light location
        GLint lightColor;
        /// \desc type of the light location - 0 point 1 directional 2 spot
        GLint lightType;
        /// \desc material diffuse color location
        GLint materialDiffColor;
        /// \desc material specular color location
        GLint materialSpecColor;
        /// \desc material shininess factor location
        GLint materialShininess;
        /// \desc material ambient color location
        GLint materialAmbColor;
    } _gouraudShaderProgramUniformLocations;
    /// \desc stores the locations of all of our shader attributes
    struct GouraudShaderProgramAttributeLocations {
        /// \desc vertex position location
        GLint vPos;
        /// \desc vertex normal location
        GLint vNormal;
    } _gouraudShaderProgramAttributeLocations;

    /// \desc shader program that performs flat shading of a single color
    CSCI441::ShaderProgram* _flatShaderProgram = nullptr;
    struct FlatShaderProgramUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;
        /// \desc the color to apply location
        GLint color;
    } _flatShaderProgramUniformLocations;

    struct FlatShaderProgramAttributeLocations {
        /// \desc precomputed MVP matrix location
        GLint vPos;
        /// \desc the color to apply location
    } _flatShaderProgramAttributeLocations;

    std::vector<CSCI441::ModelLoader*> _hero;

    /// \desc pair of values to store the speed the camera can move/rotate.
    /// \brief x = forward/backward delta, y = rotational delta
    glm::vec2 _cameraSpeed = glm::vec2(0.10f, 0.06f);

    bool _pause = false;
    GLfloat _angleX = 0;
    GLfloat _angleY = 0;
    GLfloat _angleZ = 0;
    int frame = 0;
    int iterator = -100;
    bool isBoosting = false;
    bool isLeft = false;

    void _renderSkybox(glm::mat4 viewMtx, glm::mat4 projMtx);
};

void A4_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods );
void A4_cursor_callback(GLFWwindow *window, double x, double y );
void A4_mouse_button_callback(GLFWwindow *window, int button, int action, int mods );
void A4_scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

#endif // A4_A4_ENGINE_HPP
