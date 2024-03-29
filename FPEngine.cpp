#include "FPEngine.hpp"

#include <CSCI441/TextureUtils.hpp>
#include <CSCI441/objects.hpp>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/ext/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <cstdlib>

//*************************************************************************************
//
// Helper Functions

#ifndef M_PI
#define M_PI 3.14159265
#endif

//*************************************************************************************
//
// Public Interface

FPEngine::FPEngine()
         : CSCI441::OpenGLEngine(4, 1,
                                 640, 480,
                                 "FP:There and Back Again") {

    for(auto& _key : _keys) _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;

    _nextLaser = 0;
    _laserSpeed = 200.0f/1.0f/60.0f; // 200units/5s  /  60 fps = ? units/frame
}

void FPEngine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if(action == GLFW_PRESS) {
        _keyLog.push_back(key);
        switch( key ) {
            // quit!
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
                break;

            // toggle between light types
            case GLFW_KEY_1:    // point light
            case GLFW_KEY_2:    // directional light
            case GLFW_KEY_3:    // spotlight
                _lightType = key - GLFW_KEY_1; // GLFW_KEY_1 is 49.  they go in sequence from there

                // ensure shader program is not null
                if(_gouraudShaderProgram) {
                    _gouraudShaderProgram->setProgramUniform(_gouraudShaderProgramUniformLocations.lightType, _lightType );
                }
                break;
            case GLFW_KEY_SPACE:
                isBoosting = !isBoosting;
                break;
            case GLFW_KEY_E:
                _modelCam->recomputeOrientation();
                if (!isLeft) {
                    _lasers[_nextLaser].laserPos =
                            glm::vec3(_modelCam->getPosition().x, _modelCam->getPosition().y,
                                      _modelCam->getPosition().z) + glm::vec3(2.0f, 0, -5.0f);
                    isLeft = !isLeft;
                }
                else {
                    _lasers[_nextLaser].laserPos =
                            glm::vec3(_modelCam->getPosition().x, _modelCam->getPosition().y,
                                      _modelCam->getPosition().z) + glm::vec3(-2.0f, 0, -5.0f);
                    isLeft = !isLeft;
                }
                _lasers[_nextLaser].laserDir = glm::normalize(_modelCam->getLookAtPoint() - _modelCam->getPosition());
                _nextLaser++;
                if(_nextLaser == _maxLasers) {
                    _nextLaser = 0;
                }
                break;

            default: break; // suppress CLion warning
        }
    }
    if (action == GLFW_RELEASE){
        switch( key ) {
            case GLFW_KEY_SPACE:
                isBoosting = !isBoosting;
                break;
            default:
                break; // suppress CLion warning
        }
    }
}

void FPEngine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void FPEngine::handleCursorPositionEvent(glm::vec2 currMousePosition) {
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(fabs(_mousePosition.x - MOUSE_UNINITIALIZED) <= 0.000001f) {
        _mousePosition = currMousePosition;
    }

    // active motion - if the left mouse button is being held down while the mouse is moving
    if(_leftMouseButtonState == GLFW_PRESS) {
        // if shift is held down, update our camera radius
        if( _keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT] ) {
            GLfloat totChgSq = (currMousePosition.x - _mousePosition.x) + (currMousePosition.y - _mousePosition.y);
            printf("%f ", currMousePosition.x);
            _arcballCam->moveForward( totChgSq * 0.01f );
        }
        // otherwise, update our camera angles theta & phi
        else {
            // rotate the camera by the distance the mouse moved
            if (_canMoveCam) {
                _arcballCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                                    (_mousePosition.y - currMousePosition.y) * 0.005f);
            }
        }

        // ensure shader program is not null
        if(_gouraudShaderProgram) {
            // set the eye position - needed for specular reflection
            _gouraudShaderProgram->setProgramUniform(_gouraudShaderProgramUniformLocations.eyePos,_arcballCam->getPosition());
        }
    }
    // passive motion
    else {

    }

    // update the mouse position
    _mousePosition = currMousePosition;
}

void FPEngine::handleScrollEvent(glm::vec2 offset) {
    // update the camera radius in/out
    GLfloat totChgSq = offset.y;
    _arcballCam->moveForward( totChgSq * 0.2f );
}

//*************************************************************************************
//
// Engine Setup

void FPEngine::_setupGLFW() {
    CSCI441::OpenGLEngine::_setupGLFW();

    // set our callbacks
    glfwSetKeyCallback(_window, A4_keyboard_callback);
    glfwSetMouseButtonCallback(_window, A4_mouse_button_callback);
    glfwSetCursorPosCallback(_window, A4_cursor_callback);
    glfwSetScrollCallback(_window, A4_scroll_callback);
}

void FPEngine::_setupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                    // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	            // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// clear the frame buffer to black

    glFrontFace(GL_CCW);                                            // the front faces are CCW
    glCullFace(GL_BACK);                                            // cull the back faces
}

void FPEngine::_setupShaders() {
    //***************************************************************************
    // Setup Gouraud Shader Program

    _gouraudShaderProgram = new CSCI441::ShaderProgram( "shaders/gouraudShader.v.glsl", "shaders/gouraudShader.f.glsl" );
    // get uniform locations
    _gouraudShaderProgramUniformLocations.mvpMatrix           = _gouraudShaderProgram->getUniformLocation("mvpMatrix");
    _gouraudShaderProgramUniformLocations.modelMatrix         = _gouraudShaderProgram->getUniformLocation("modelMatrix");
    _gouraudShaderProgramUniformLocations.normalMatrix        = _gouraudShaderProgram->getUniformLocation("normalMtx");
    _gouraudShaderProgramUniformLocations.eyePos              = _gouraudShaderProgram->getUniformLocation("eyePos");
    _gouraudShaderProgramUniformLocations.lightPos            = _gouraudShaderProgram->getUniformLocation("lightPos");
    _gouraudShaderProgramUniformLocations.lightDir            = _gouraudShaderProgram->getUniformLocation("lightDir");
    _gouraudShaderProgramUniformLocations.lightCutoff         = _gouraudShaderProgram->getUniformLocation("lightCutoff");
    _gouraudShaderProgramUniformLocations.lightColor          = _gouraudShaderProgram->getUniformLocation("lightColor");
    _gouraudShaderProgramUniformLocations.lightType           = _gouraudShaderProgram->getUniformLocation("lightType");
    _gouraudShaderProgramUniformLocations.materialDiffColor   = _gouraudShaderProgram->getUniformLocation("materialDiffColor");
    _gouraudShaderProgramUniformLocations.materialSpecColor   = _gouraudShaderProgram->getUniformLocation("materialSpecColor");
    _gouraudShaderProgramUniformLocations.materialShininess   = _gouraudShaderProgram->getUniformLocation("materialShininess");
    _gouraudShaderProgramUniformLocations.materialAmbColor    = _gouraudShaderProgram->getUniformLocation("materialAmbColor");
    // get attribute locations
    _gouraudShaderProgramAttributeLocations.vPos              = _gouraudShaderProgram->getAttributeLocation("vPos");
    _gouraudShaderProgramAttributeLocations.vNormal           = _gouraudShaderProgram->getAttributeLocation("vNormal");

    //***************************************************************************
    // Setup Flat Shader Program

    _flatShaderProgram = new CSCI441::ShaderProgram( "shaders/flatShader.v.glsl", "shaders/flatShader.f.glsl" );
    // get uniform locations
    _flatShaderProgramUniformLocations.mvpMatrix             = _flatShaderProgram->getUniformLocation("mvpMatrix");
    _flatShaderProgramUniformLocations.color                 = _flatShaderProgram->getUniformLocation("color");
    _flatShaderProgramAttributeLocations.vPos          = _flatShaderProgram->getAttributeLocation("vPos");
    // NOTE: we do not query an attribute locations because in our shader we have set the locations to be the same as
    // the Gouraud Shader attribute locations

    // NOTE: cartoon shader is overriding normal texture shader curr
    _textureShaderProgram = new CSCI441::ShaderProgram("shaders/cartoonTextureShader.v.glsl", "shaders/cartoonTextureShader.f.glsl" );
    // _textureShaderProgram = new CSCI441::ShaderProgram("shaders/textureShader.v.glsl", "shaders/textureShader.f.glsl" );

    // query uniform locations
    _textureShaderUniformLocations.mvpMatrix       = _textureShaderProgram->getUniformLocation("mvpMatrix");
    _textureShaderUniformLocations.textureMap      = _textureShaderProgram->getUniformLocation("textureMap");
    _textureShaderUniformLocations.sunDir      = _textureShaderProgram->getUniformLocation("sunDir");
    _textureShaderUniformLocations.sunColor      = _textureShaderProgram->getUniformLocation("sunColor");
    _textureShaderUniformLocations.cameraPos      = _textureShaderProgram->getUniformLocation("cameraPos");
    _textureShaderUniformLocations.inverseVPMatrix      = _textureShaderProgram->getUniformLocation("inverseVPMatrix");
    _textureShaderUniformLocations.doShading = _textureShaderProgram->getUniformLocation("doShading");
    _textureShaderUniformLocations.laserColor[0] = _textureShaderProgram->getUniformLocation("laserOneColor");
    _textureShaderUniformLocations.laserPos[0] = _textureShaderProgram->getUniformLocation("laserOnePos");
    _textureShaderUniformLocations.laserColor[1] = _textureShaderProgram->getUniformLocation("laserTwoColor");
    _textureShaderUniformLocations.laserPos[1] = _textureShaderProgram->getUniformLocation("laserTwoPos");
    _textureShaderUniformLocations.laserColor[2] = _textureShaderProgram->getUniformLocation("laserThreeColor");
    _textureShaderUniformLocations.laserPos[2] = _textureShaderProgram->getUniformLocation("laserThreePos");
    _textureShaderUniformLocations.laserColor[3] = _textureShaderProgram->getUniformLocation("laserFourColor");
    _textureShaderUniformLocations.laserPos[3] = _textureShaderProgram->getUniformLocation("laserFourPos");
    _textureShaderUniformLocations.laserColor[4] = _textureShaderProgram->getUniformLocation("laserFiveColor");
    _textureShaderUniformLocations.laserPos[4] = _textureShaderProgram->getUniformLocation("laserFivePos");
    _textureShaderUniformLocations.laserColor[5] = _textureShaderProgram->getUniformLocation("laserSixColor");
    _textureShaderUniformLocations.laserPos[5] = _textureShaderProgram->getUniformLocation("laserSixPos");
    _textureShaderUniformLocations.laserColor[6] = _textureShaderProgram->getUniformLocation("laserSevenColor");
    _textureShaderUniformLocations.laserPos[6] = _textureShaderProgram->getUniformLocation("laserSevenPos");
    _textureShaderUniformLocations.laserColor[7] = _textureShaderProgram->getUniformLocation("laserEightColor");
    _textureShaderUniformLocations.laserPos[7] = _textureShaderProgram->getUniformLocation("laserEightPos");
    _textureShaderUniformLocations.laserColor[8] = _textureShaderProgram->getUniformLocation("laserNineColor");
    _textureShaderUniformLocations.laserPos[8] = _textureShaderProgram->getUniformLocation("laserNinePos");
    _textureShaderUniformLocations.laserColor[9] = _textureShaderProgram->getUniformLocation("laserTenColor");
    _textureShaderUniformLocations.laserPos[9] = _textureShaderProgram->getUniformLocation("laserTenPos");
    _textureShaderUniformLocations.laserColor[10] = _textureShaderProgram->getUniformLocation("laserElevenColor");
    _textureShaderUniformLocations.laserPos[10] = _textureShaderProgram->getUniformLocation("laserElevenPos");
    _textureShaderUniformLocations.laserColor[11] = _textureShaderProgram->getUniformLocation("laserTwelveColor");
    _textureShaderUniformLocations.laserPos[11] = _textureShaderProgram->getUniformLocation("laserTwelvePos");
    _textureShaderUniformLocations.laserColor[12] = _textureShaderProgram->getUniformLocation("laserThirteenColor");
    _textureShaderUniformLocations.laserPos[12] = _textureShaderProgram->getUniformLocation("laserThirteenPos");
    _textureShaderUniformLocations.laserColor[13] = _textureShaderProgram->getUniformLocation("laserFourteenColor");
    _textureShaderUniformLocations.laserPos[13] = _textureShaderProgram->getUniformLocation("laserFourteenPos");
    _textureShaderUniformLocations.laserColor[14] = _textureShaderProgram->getUniformLocation("laserFifteenColor");
    _textureShaderUniformLocations.laserPos[14] = _textureShaderProgram->getUniformLocation("laserFifteenPos");
    _textureShaderUniformLocations.laserColor[15] = _textureShaderProgram->getUniformLocation("laserSixteenColor");
    _textureShaderUniformLocations.laserPos[15] = _textureShaderProgram->getUniformLocation("laserSixteenPos");

    // query attribute locations
    _textureShaderAttributeLocations.vPos          = _textureShaderProgram->getAttributeLocation("vPos");
    _textureShaderAttributeLocations.vNormal       = _textureShaderProgram->getAttributeLocation("vNormal");
    _textureShaderAttributeLocations.vTexCoord     = _textureShaderProgram->getAttributeLocation("vTexCoord");

    // set static uniforms
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.textureMap,0);


    _glitchShaderProgram = new CSCI441::ShaderProgram("shaders/glitchShader.v.glsl", "shaders/glitchShader.f.glsl" );
    // _textureShaderProgram = new CSCI441::ShaderProgram("shaders/textureShader.v.glsl", "shaders/textureShader.f.glsl" );

    // query uniform locations
    _glitchShaderUniformLocations.mvpMatrix       = _glitchShaderProgram->getUniformLocation("mvpMtx");

    // query attribute locations
    _glitchShaderAttributeLocations.vPos          = _glitchShaderProgram->getAttributeLocation("vPos");
    // hook up the CSCI441 object library to our shader program - MUST be done after the shader is used and before the objects are drawn
    // if we have multiple shaders the flow would be:
    //      1) shader->useProgram()
    //      2) CSCI441::setVertexAttributeLocations()
    //      3) CSCI441::draw*()
    // OR the alternative is to ensure that all of your shader programs use the
    // same attribute locations for the vertex position, normal, and texture coordinate
    // as this lab is doing
    CSCI441::setVertexAttributeLocations(_textureShaderAttributeLocations.vPos,
                                         _textureShaderAttributeLocations.vNormal,
                                         _textureShaderAttributeLocations.vTexCoord);
}

void FPEngine::_setupBuffers() {
    // ------------------------------------------------------------------------------------------------------
    // generate all of our VAO/VBO/IBO descriptors
    glGenVertexArrays( NUM_VAOS, _vaos );
    glGenBuffers( NUM_VAOS, _vbos );
    glGenBuffers( NUM_VAOS, _ibos );

    // ------------------------------------------------------------------------------------------------------
    // create the platform
    _createQuad(_vaos[VAO_ID::PLATFORM], _vbos[VAO_ID::PLATFORM], _ibos[VAO_ID::PLATFORM], _numVAOPoints[VAO_ID::PLATFORM]);
    _createPlatform(_vaos[VAO_ID::SKYBOX], _vbos[VAO_ID::SKYBOX], _ibos[VAO_ID::SKYBOX], _numVAOPoints[VAO_ID::SKYBOX]);

    for (int i = 0; i < 100; i++) {
        _hero.push_back(new CSCI441::ModelLoader());
        std::string name = "assets/models/Animated/arwingNew" + std::to_string(i) + ".obj";
        char* fileName = &name[0];
        _hero.at(i)->loadModelFile(fileName);
        _hero.at(i)->setAttributeLocations(_textureShaderAttributeLocations.vPos,
                                     _textureShaderAttributeLocations.vNormal,
                                     _textureShaderAttributeLocations.vTexCoord);
    }
}

void FPEngine::_createPlatform(GLuint vao, GLuint vbo, GLuint ibo, GLsizei &numVAOPoints) const {
    struct VertexNormalTextured {
        GLfloat x, y, z;
        GLfloat nx, ny, nz;
        GLfloat s, t;
    };

    // create our platform
    VertexNormalTextured platformVertices[4] = {
            { -10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f, 0, 0 }, // 0 - BL
            {  10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f, 1, 0 }, // 1 - BR
            { -10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f, 0, 1 }, // 2 - TL
            {  10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f, 1, 1 }  // 3 - TR
    };

    GLushort platformIndices[4] = { 0, 1, 2, 3 };
    numVAOPoints = 4;

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData( GL_ARRAY_BUFFER, sizeof( platformVertices ), platformVertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( _textureShaderAttributeLocations.vPos );
    glVertexAttribPointer( _textureShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*) 0 );

    glEnableVertexAttribArray( _textureShaderAttributeLocations.vNormal );
    glVertexAttribPointer( _textureShaderAttributeLocations.vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*) (sizeof(GLfloat) * 3) );

    glEnableVertexAttribArray( _textureShaderAttributeLocations.vTexCoord );
    glVertexAttribPointer( _textureShaderAttributeLocations.vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*) (sizeof(GLfloat) * 6) );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( platformIndices ), platformIndices, GL_STATIC_DRAW );

    fprintf( stdout, "[INFO]: platform read in with VAO/VBO/IBO %d/%d/%d & %d points\n", vao, vbo, ibo, numVAOPoints );
}

void FPEngine::_createQuad(GLuint vao, GLuint vbo, GLuint ibo, GLsizei &numVAOPoints) const {
    struct VertexNormalTextured {
        float x, y, z;
        float nx, ny, nz;
        float s, t;
    };

    // create our custom quad
    VertexNormalTextured quadVertices[4] = {
            { -100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f, 0, 0 }, // 0 - BL
            {  100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f, 25, 0 }, // 1 - BR
            { -100.0f, 0.0f,  100.0f,  0.0f, 1.0f, 0.0f, 0, 25 }, // 2 - TL
            {  100.0f, 0.0f,  100.0f,  0.0f, 1.0f, 0.0f, 25, 25 }  // 3 - TR
    };

    GLushort quadIndices[4] = { 0, 1, 2, 3 };
    numVAOPoints = 4;

    glBindVertexArray( vao );

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), quadVertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( _textureShaderAttributeLocations.vPos );
    glVertexAttribPointer( _textureShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*) 0 );

    glEnableVertexAttribArray( _textureShaderAttributeLocations.vNormal );
    glVertexAttribPointer( _textureShaderAttributeLocations.vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*) (sizeof(float) * 3) );

    glEnableVertexAttribArray( _textureShaderAttributeLocations.vTexCoord );
    glVertexAttribPointer( _textureShaderAttributeLocations.vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*) (sizeof(float) * 6) );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( quadIndices ), quadIndices, GL_STATIC_DRAW );

    fprintf( stdout, "[INFO]: quad read in with VAO/VBO/IBO %d/%d/%d & %d points\n", vao, vbo, ibo, numVAOPoints );
}

void FPEngine::_setupTextures() {
    _texHandles[TEXTURE_ID::TOP] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/textures/sky3.bmp",GL_LINEAR, GL_LINEAR, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    _texHandles[TEXTURE_ID::RIGHT] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/textures/sky1.bmp",GL_LINEAR, GL_LINEAR, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    _texHandles[TEXTURE_ID::LEFT] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/textures/sky2.bmp",GL_LINEAR, GL_LINEAR, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    _texHandles[TEXTURE_ID::BOTTOM] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/textures/sky4.bmp",GL_LINEAR, GL_LINEAR, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    _texHandles[TEXTURE_ID::FRONT] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/textures/sky5.bmp",GL_LINEAR, GL_LINEAR, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    _texHandles[TEXTURE_ID::BACK] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/textures/Ground.png");
}

void FPEngine::_setupScene() {
    // if either shader program is null, do not continue any further to prevent run time errors
    if(!_gouraudShaderProgram || !_flatShaderProgram) {
        return;
    }

    // set up camera
    _arcballCam = new CSCI441::ArcballCam();
    _arcballCam->setLookAtPoint(glm::vec3(0.0f, 5.0f, 0.0f));
    _arcballCam->setTheta(M_PI);
    _arcballCam->setPhi(M_PI/2);
    _arcballCam->setRadius(15.0f);
    _arcballCam->recomputeOrientation();
    _modelCam = new CSCI441::FreeCam();
    _modelCam->setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    _modelCam->setTheta(0.0f);
    _modelCam->setPhi(M_PI / 2.0f);
    _modelCam->setRadius(15.0f);
    _modelCam->recomputeOrientation();
    _firstPersonCam = new CSCI441::FreeCam();
    _firstPersonCam->setPosition(glm::vec3(0.0f, 7.0f, 0.0f));
    _firstPersonCam->setTheta(0.0f);
    _firstPersonCam->setPhi(M_PI / 2.0f);
    _firstPersonCam->setRadius(15.0f);
    _firstPersonCam->recomputeOrientation();
    _gouraudShaderProgram->setProgramUniform(_gouraudShaderProgramUniformLocations.eyePos, _arcballCam->getPosition());

    // set up light info
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    _lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
    _lightDir = glm::vec3(-1.0f, -5.0f, -1.0f);
    _lightAngle = glm::radians(27.5f);
    float lightCutoff = glm::cos(_lightAngle);
    _lightType = 0;

    _gouraudShaderProgram->setProgramUniform(_gouraudShaderProgramUniformLocations.lightColor, lightColor);
    _gouraudShaderProgram->setProgramUniform(_gouraudShaderProgramUniformLocations.lightPos, _lightPos);
    _gouraudShaderProgram->setProgramUniform(_gouraudShaderProgramUniformLocations.lightDir, _lightDir);
    _gouraudShaderProgram->setProgramUniform(_gouraudShaderProgramUniformLocations.lightCutoff, lightCutoff);
    _gouraudShaderProgram->setProgramUniform(_gouraudShaderProgramUniformLocations.lightType, _lightType);

    // set flat shading color
    glm::vec3 flatColor(1.0f, 1.0f, 1.0f);
    _flatShaderProgram->setProgramUniform(_flatShaderProgramUniformLocations.color, flatColor);

    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.sunDir, glm::vec3(-2.0f, -2.0f, 0.5f));
    // _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.sunDir, glm::vec3(0.0f, 0.0f, -1.0f));
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.sunColor, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    // _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.laserOneColor, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    // _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.laserOnePos, glm::vec3(0.0f, 5.0f, -100.0f));

    auto laserPos = glm::vec3(10000.0f);
    auto laserDir = glm::vec3(1.0f);
    glm::vec4 laserColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    for(int i = 0; i < _maxLasers; i++) {
        _lasers[i].laserPos = laserPos;
        _lasers[i].laserDir = glm::normalize(laserDir);
        _lasers[i].laserColor = laserColor;
        _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.laserPos[i], _lasers[i].laserPos);
        _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.laserColor[i], _lasers[i].laserColor);
    }

    // Enemies
    Enemies enemy1;
    enemy1.position = glm::vec3(0.0f, 10.0f, -200.0f);
    enemy1.direction = glm::vec3(0.0f, 0.0f, 1.0f);
    Enemies enemy2;
    enemy2.position = glm::vec3(10.0f, 2.0f, -300.0f);
    enemy2.direction = glm::vec3(0.0f, 0.0f, 1.0f);
    Enemies enemy3;
    enemy3.position = glm::vec3(-10.0f, 20.0f, -300.0f);
    enemy3.direction = glm::vec3(0.0f, 0.0f, 1.0f);
    Enemies enemy4;
    enemy4.position = glm::vec3(-5.0f, 12.0f, -500.0f);
    enemy4.direction = glm::vec3(0.0f, 0.0f, 1.0f);
    Enemies enemy5;
    enemy5.position = glm::vec3(5.0f, 8.0f, -600.0f);
    enemy5.direction = glm::vec3(0.0f, 0.0f, 1.0f);
    _enemies.push_back(enemy1);
    _enemies.push_back(enemy2);
    _enemies.push_back(enemy3);
    _enemies.push_back(enemy4);
    _enemies.push_back(enemy5);

}

//*************************************************************************************
//
// Engine Cleanup

void FPEngine::_cleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _gouraudShaderProgram;
    delete _flatShaderProgram;
    delete _textureShaderProgram;
    delete _glitchShaderProgram;
}

void FPEngine::_cleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( NUM_VAOS, _vaos );

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();
    glDeleteBuffers( NUM_VAOS, _vbos );

    fprintf( stdout, "[INFO]: ...deleting IBOs....\n" );
    glDeleteBuffers( NUM_VAOS, _ibos );

    fprintf( stdout, "[INFO]: ...deleting models..\n" );
    for (int i = 0; i < 100; i++) {
        delete _hero.at(i);
    }
}

void FPEngine::_cleanupTextures() {
    fprintf( stdout, "[INFO]: ...deleting Textures\n" );
    glDeleteTextures(1, &_texHandles[0]);
    glDeleteTextures(1, &_texHandles[1]);
    glDeleteTextures(1, &_texHandles[2]);
    glDeleteTextures(1, &_texHandles[3]);
    glDeleteTextures(1, &_texHandles[4]);
    glDeleteTextures(1, &_texHandles[5]);
}

void FPEngine::_cleanupScene() {
    fprintf( stdout, "[INFO]: ...deleting scene...\n" );
    delete _arcballCam;
    delete _modelCam;
    delete _firstPersonCam;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void FPEngine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) {
    // if either shader program is null, do not continue any further to prevent run time errors
    if(!_gouraudShaderProgram || !_flatShaderProgram || !_textureShaderProgram ||!_glitchShaderProgram) {
        return;
    }

    glm::mat4 modelMatrix = glm::mat4( 1.0f );
    glm::mat4 projectionViewMatrix = projMtx * viewMtx;
    glm::mat4 mvpMatrix = projectionViewMatrix * modelMatrix;

    // use the gouraud shader
    _textureShaderProgram->useProgram();
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.doShading, 1.0f);
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.cameraPos, _arcballCam->getPosition());
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.inverseVPMatrix, glm::inverse(projMtx * viewMtx));

    for(int i = 0; i < _maxLasers; i++) {
        _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.laserPos[i], _lasers[i].laserPos);
    }

    /// BEGIN DRAWING THE HERO ///
    glm::mat4 modelMtx(1.0f);
    //printf("%f, %f, %f | %f, %f, %f\n", viewMtx[0].x, viewMtx[0].y, viewMtx[0].z, modelMtx[0].x, modelMtx[0].y, modelMtx[0].z);

    modelMtx = glm::translate(modelMtx, _modelCam->getLookAtPoint());
    // rotate the plane with our camera theta direction (we need to rotate the opposite direction so that we always look at the back)
    modelMtx = glm::rotate(modelMtx, _angleX, CSCI441::Y_AXIS);
    modelMtx = glm::rotate(modelMtx, _angleZ, CSCI441::Z_AXIS);
    // rotate the plane with our camera phi direction
    modelMtx = glm::rotate(modelMtx, _modelCam->getPhi(), -glm::cross(_modelCam->getPosition() - _modelCam->getLookAtPoint(), _modelCam->getUpVector()));

    modelMtx = glm::rotate( modelMtx, glm::radians(180.0f), CSCI441::Y_AXIS );
    modelMtx = glm::rotate( modelMtx, glm::radians(90.0f), CSCI441::X_AXIS );
    modelMtx = glm::scale(modelMtx, glm::vec3(1.5f));
    mvpMatrix = projMtx * viewMtx * modelMtx;

    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMatrix);
    // draw our plane now

    _hero.at(frame)->draw(_textureShaderProgram->getShaderProgramHandle());

    //printf("%f, %f, %f | %f, %f, %f\n", viewMtx[0].x, viewMtx[0].y, viewMtx[0].z, modelMtx[2].x, modelMtx[2].y, modelMtx[2].z);

    // Ground
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -20.0f, _groundPos));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f));
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::BACK]);
    glBindVertexArray( _vaos[VAO_ID::PLATFORM] );
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::PLATFORM], GL_UNSIGNED_SHORT, (void*)0 );

    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.doShading, 0.0f);
    // Draw lasers
    _flatShaderProgram->useProgram();
    CSCI441::setVertexAttributeLocations(_flatShaderProgramAttributeLocations.vPos);
    for(int i = 0; i < _maxLasers; i++) {
        _flatShaderProgram->setProgramUniform(_flatShaderProgramUniformLocations.color, glm::vec3(_lasers[i].laserColor));
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, _lasers[i].laserPos);
        mvpMatrix = projMtx * viewMtx * modelMatrix;
        _flatShaderProgram->setProgramUniform(_flatShaderProgramUniformLocations.mvpMatrix, mvpMatrix);
        CSCI441::drawSolidSphere(1.0f, 16, 16);
    }

    // Enemies
    _glitchShaderProgram->useProgram();
    CSCI441::setVertexAttributeLocations(_glitchShaderAttributeLocations.vPos);
    for(auto ix = _enemies.begin(); ix != _enemies.end(); ix++) {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, ix->position);
        mvpMatrix = projMtx * viewMtx * modelMatrix;
        _glitchShaderProgram->setProgramUniform(_glitchShaderUniformLocations.mvpMatrix, mvpMatrix);

        CSCI441::drawSolidSphere(2.0f, 8, 8);
    }



    CSCI441::setVertexAttributeLocations(_textureShaderAttributeLocations.vPos,
                                         _textureShaderAttributeLocations.vNormal,
                                         _textureShaderAttributeLocations.vTexCoord);
}

void FPEngine::_renderSkybox(glm::mat4 viewMtx, glm::mat4 projMtx) {
    glm::mat4 modelMatrix = glm::mat4( 1.0f );
    glm::mat4 projectionViewMatrix = projMtx * viewMtx;
    glm::mat4 mvpMatrix = projectionViewMatrix * modelMatrix;

    // use the texture shader
    _textureShaderProgram->useProgram();
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.doShading, 1.0f);
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.cameraPos, _modelCam->getPosition());
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.inverseVPMatrix, glm::inverse(projMtx * viewMtx));

    for(int i = 0; i < _maxLasers; i++) {
        _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.laserPos[i], _lasers[i].laserPos);
    }

    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.doShading, 0.0f);

    /// BEGIN DRAWING SKYBOX ///
    CSCI441::setVertexAttributeLocations(_textureShaderAttributeLocations.vPos,
                                         _textureShaderAttributeLocations.vNormal,
                                         _textureShaderAttributeLocations.vTexCoord);
    glm::vec3 pos;
    if (!_firstPerson){
        pos = _arcballCam->getLookAtPoint();
    }
    else{
        pos = _firstPersonCam->getPosition();
    }

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 200.0f, 0.0f));
    modelMatrix = glm::translate(modelMatrix, pos);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f));
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::TOP]);
    glBindVertexArray( _vaos[VAO_ID::SKYBOX] );
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    // Right
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 0.0f, 0.0f));
    modelMatrix = glm::translate(modelMatrix, pos);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), CSCI441::Y_AXIS);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), CSCI441::X_AXIS);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f));
    mvpMtx = projMtx * viewMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::RIGHT]);
    glBindVertexArray( _vaos[VAO_ID::SKYBOX] );
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    // Back
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 200.0f));
    modelMatrix = glm::translate(modelMatrix, pos);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), CSCI441::X_AXIS);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-180.0f), CSCI441::Z_AXIS);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f));
    mvpMtx = projMtx * viewMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::LEFT]);
    glBindVertexArray( _vaos[VAO_ID::SKYBOX] );
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    // Front
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -200.0f));
    modelMatrix = glm::translate(modelMatrix, pos);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), CSCI441::X_AXIS);
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(-180.0f), CSCI441::Z_AXIS);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f));
    mvpMtx = projMtx * viewMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::FRONT]);
    glBindVertexArray( _vaos[VAO_ID::SKYBOX] );
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    // Front
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-200.0f, 0.0f, 0.0f));
    modelMatrix = glm::translate(modelMatrix, pos);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), CSCI441::Y_AXIS);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), CSCI441::X_AXIS);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f));
    mvpMtx = projMtx * viewMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::BOTTOM]);
    glBindVertexArray( _vaos[VAO_ID::SKYBOX] );
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    //// END DRAWING THE SKY BOX ////
}

void FPEngine::_updateScene() {
    _modelCam->setPosition(_modelCam->getPosition() - glm::vec3(0, 0, 2 * _cameraSpeed.x));
    _modelCam->moveForward(0);
    _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() - glm::vec3(0, 0, 2*_cameraSpeed.x));
    _arcballCam->recomputeOrientation();
    _firstPersonCam->setLookAtPoint(_modelCam->getLookAtPoint());
    _firstPersonCam->setPosition(_modelCam->getPosition() + glm::vec3(0, 2, 0));
    _firstPersonCam->setTheta(_firstPersonCam->getTheta() + _angleX);
    _firstPersonCam->moveForward(0);

    if (!_eeActive) {
        _checkEE();
    }
    else{
        _angleZ += .04f;
        if (_angleZ > 2*M_PI){
            printf("Do a barrel roll!");
            _angleZ = 0;
            _eeActive = false;
            _keyLog.clear();
        }
    }

    int z = _modelCam->getPosition().z;
    if (z % 1000 == 0 && z != 0){
        _groundPos = 1000 * z/1000;
    }
    iterator++;
    if (!isBoosting) {
        _cameraSpeed = glm::vec2(.15, .06);
        if (iterator % 5 == 0) {
            if (frame < 99 && iterator > 0) {
                frame++;
            }
        }
    }
    else {
        _cameraSpeed = glm::vec2(.22, .06);
        if (iterator % 2 == 0) {
            if (frame > 42) {
                frame--;
            }
        }
    }

    position = _modelCam->getPosition();
    // turn right
    if (_keys[GLFW_KEY_D]) {
        if(position.x < 12) {
            //_modelCam->moveForward(_cameraSpeed.x);
            _modelCam->setPosition(_modelCam->getPosition() - glm::vec3(-_cameraSpeed.x, 0, 0));
            _modelCam->moveForward(0);
            if (position.x > 10) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() + glm::vec3(_cameraSpeed.y,0,0));
                _arcballCam->recomputeOrientation();
            }
            if (position.x < -10) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() + glm::vec3(_cameraSpeed.y,0,0));
                _arcballCam->recomputeOrientation();
            }
            if (_angleX > -(M_PI)/4.0f) {
                _angleX -= 0.02f;
                _angleZ -= 0.02f;
            }
        }
    }
    // turn left
    if (_keys[GLFW_KEY_A]) {
        if(position.x > -12) {
            //_modelCam->moveForward(_cameraSpeed.x);
            _modelCam->setPosition(_modelCam->getPosition() - glm::vec3(_cameraSpeed.x, 0, 0));
            _modelCam->moveForward(0);
            if (position.x > 10) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() - glm::vec3(_cameraSpeed.y,0,0));
                _arcballCam->recomputeOrientation();
            }
            if (position.x < -10) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() - glm::vec3(_cameraSpeed.y,0,0));
                _arcballCam->recomputeOrientation();
            }
            if (_angleX < (M_PI)/4.0f) {
                _angleX += 0.02f;
                _angleZ += 0.02f;
            }
        }
    }
    // move forward
    if (_keys[GLFW_KEY_W]) {
        if(position.y < 15) {
            _modelCam->setPosition(_modelCam->getPosition() - glm::vec3(0, -_cameraSpeed.x, 0));
            _modelCam->moveForward(0);
            if (position.y > 10) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() + glm::vec3(0,_cameraSpeed.y,0));
                _arcballCam->recomputeOrientation();
            }
            if (position.y < 5) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() + glm::vec3(0,_cameraSpeed.y,0));
                _arcballCam->recomputeOrientation();
            }
            if (_modelCam->getPhi() < (2.130796)) {
                _modelCam->rotate(0.0f, 0.02f);
            }
        }
    }
    // move backward
    if (_keys[GLFW_KEY_S]) {
        if(position.y > -10) {
            _modelCam->setPosition(_modelCam->getPosition() - glm::vec3(0, _cameraSpeed.x, 0));
            _modelCam->moveForward(0);
            if (position.y < 5) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() - glm::vec3(0,_cameraSpeed.y,0));
                _arcballCam->recomputeOrientation();
            }
            if (position.y > 10) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() - glm::vec3(0,_cameraSpeed.y,0));
                _arcballCam->recomputeOrientation();
            }
            if (_modelCam->getPhi() > (M_PI) / 4.0f) {
                _modelCam->rotate(-0.0f, -0.02f);
            }
        }
    }

    // reset angle to face forward
    if(!_keys[GLFW_KEY_S] && !_keys[GLFW_KEY_W]) {
        if (_modelCam->getPhi() != M_PI / 2) {
            if (_modelCam->getPhi() > M_PI / 2) {
                _modelCam->rotate(0.0f, -0.02f);
            }
            if (_modelCam->getPhi() < M_PI / 2) {
                _modelCam->rotate(0.0f, 0.02f);
            }
        }
    }

    // reset angle to face forward
    if(!_keys[GLFW_KEY_A] && !_keys[GLFW_KEY_D]) {
        if (_angleX != 0) {
            if (_angleX > 0) {
                _angleX += -0.02f;
            }
            if (_angleX < 0) {
                _angleX += 0.02f;
            }
        }
        if (_angleZ != 0) {
            if (_angleZ > 0 && !_eeActive) {
                _angleZ += -0.02f;
            }
            if (_angleZ < 0 && !_eeActive) {
                _angleZ += 0.02f;
            }
        }
    }

    // Change camera
    if (_keys[GLFW_KEY_1]) {
        _firstPerson = false;
    }
    if (_keys[GLFW_KEY_2]) {
        _firstPerson = true;
    }
    if (_keys[GLFW_KEY_0]) {
        _canMoveCam = !_canMoveCam;
    }

    // Lasers
    for(int i = 0; i < _maxLasers; i++) {
        _lasers[i].laserPos += _laserSpeed * _lasers[i].laserDir;
        //TODO: add color if it changes
    }

    // Enemies
    for(auto ix = _enemies.begin(); ix != _enemies.end(); ix++) {
        // Move
        ix->position += ix->direction * 0.1f;

        // Collision
        for(int i = 0; i < _maxLasers; i++) {
            if(glm::distance(ix->position, _lasers[i].laserPos) < 1.0f + 3.0f) {
                ix->position.z -= 400.0f;
                ix--;
                _lasers[i].laserPos = glm::vec3(10000.0f);
                _lasers[i].laserDir = glm::vec3(1.0f);
            }
        }

        // Behind player
        if(ix->position.z > _modelCam->getLookAtPoint().z) {
            ix->position.z -= 400.0f;
        }
    }

}

void FPEngine::run() {
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(_window) ) {	        // check if the window was instructed to be closed
        glEnable( GL_DEPTH_TEST);
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA,
                     GL_ONE_MINUS_SRC_ALPHA );
        glDrawBuffer( GL_BACK );				        // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window


        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // set the projection matrix based on the window size
        // use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        glm::mat4 projectionMatrix = glm::perspective( 45.0f, (GLfloat) framebufferWidth / (GLfloat) framebufferHeight, 0.001f, 1000.0f );

        // set up our look at matrix to position our camera
        glm::mat4 viewMatrix;
        if (!_firstPerson) {
            viewMatrix = _arcballCam->getViewMatrix();
        }
        else {
            viewMatrix = _firstPersonCam->getViewMatrix();
        }

        glDisable(GL_DEPTH_TEST);
        _renderSkybox(viewMatrix, projectionMatrix);
        glEnable(GL_DEPTH_TEST);
        // draw everything to the window
        _renderScene(viewMatrix, projectionMatrix);

        // animate the scene
        _updateScene();

        glfwSwapBuffers(_window);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen
    }
}

void FPEngine::_checkEE() {
    // Correct key input is up, up, down, down, left, right, left, right, A, B (using arrows keys for directions)
    if(!_keyLog.empty() ) {
        if (_keyLog.at(0) == GLFW_KEY_UP) {
            if(_keyLog.size() > 1 ) {
                if(_keyLog.at(1) == GLFW_KEY_UP){
                    if(_keyLog.size() > 2 ) {
                        if(_keyLog.at(2) == GLFW_KEY_DOWN){
                            if(_keyLog.size() > 3 ) {
                                if(_keyLog.at(3) == GLFW_KEY_DOWN){
                                    if(_keyLog.size() > 4 ) {
                                        if(_keyLog.at(4) == GLFW_KEY_LEFT){
                                            if(_keyLog.size() > 5 ) {
                                                if(_keyLog.at(5) == GLFW_KEY_RIGHT){
                                                    if(_keyLog.size() > 6 ) {
                                                        if(_keyLog.at(6) == GLFW_KEY_LEFT){
                                                            if(_keyLog.size() > 7 ) {
                                                                if(_keyLog.at(7) == GLFW_KEY_RIGHT){
                                                                    if(_keyLog.size() > 8 ) {
                                                                        if(_keyLog.at(8) == GLFW_KEY_A){
                                                                            if(_keyLog.size() > 9 ) {
                                                                                if(_keyLog.at(9) == GLFW_KEY_B){
                                                                                    _eeActive = true;
                                                                                }
                                                                                else {
                                                                                    _keyLog.clear();
                                                                                }
                                                                            }
                                                                        }
                                                                        else {
                                                                            _keyLog.clear();
                                                                        }
                                                                    }
                                                                }
                                                                else {
                                                                    _keyLog.clear();
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            _keyLog.clear();
                                                        }
                                                    }
                                                }
                                                else {
                                                    _keyLog.clear();
                                                }
                                            }
                                        }
                                        else {
                                            _keyLog.clear();
                                        }
                                    }
                                }
                                else {
                                    _keyLog.clear();
                                }
                            }
                        }
                        else {
                            _keyLog.clear();
                        }
                    }
                }
                else {
                    _keyLog.clear();
                }
            }
        }
        else {
            _keyLog.clear();
        }
    }
}

//*************************************************************************************
//
// Callbacks

void A4_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (FPEngine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the key and action through to the engine
        engine->handleKeyEvent(key, action);
    }
}

void A4_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (FPEngine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the cursor position through to the engine
        engine->handleCursorPositionEvent(glm::vec2(x, y));
    }
}

void A4_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (FPEngine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the mouse button and action through to the engine
        engine->handleMouseButtonEvent(button, action);
    }
}

void A4_scroll_callback(GLFWwindow *window, double xOffset, double yOffset) {
    auto engine = (FPEngine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the scroll offset through to the engine
        engine->handleScrollEvent(glm::vec2(xOffset, yOffset));
    }
}
