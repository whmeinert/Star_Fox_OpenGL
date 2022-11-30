#include "FPEngine.hpp"

#include <CSCI441/TextureUtils.hpp>
#include <CSCI441/objects.hpp>

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
}

void FPEngine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if(action == GLFW_PRESS) {
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

            default: break; // suppress CLion warning
        }
    }
    if (action == GLFW_RELEASE){
        _pause = !_pause;
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
            //_arcballCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
            //                    (_mousePosition.y - currMousePosition.y) * 0.005f);
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
    // NOTE: we do not query an attribute locations because in our shader we have set the locations to be the same as
    // the Gouraud Shader attribute locations

    _textureShaderProgram = new CSCI441::ShaderProgram("shaders/textureShader.v.glsl", "shaders/textureShader.f.glsl" );
    // query uniform locations
    _textureShaderUniformLocations.mvpMatrix       = _textureShaderProgram->getUniformLocation("mvpMatrix");
    // TODO #12A
    _textureShaderUniformLocations.textureMap      = _textureShaderProgram->getUniformLocation("textureMap");

    // query attribute locations
    _textureShaderAttributeLocations.vPos          = _textureShaderProgram->getAttributeLocation("vPos");
    _textureShaderAttributeLocations.vNormal       = _textureShaderProgram->getAttributeLocation("vNormal");
    // TODO #12B
    _textureShaderAttributeLocations.vTexCoord     = _textureShaderProgram->getAttributeLocation("vTexCoord");

    // set static uniforms
    // TODO #13
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.textureMap,0);

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

    _hero = new CSCI441::ModelLoader();
    _hero->loadModelFile( "assets/models/arwing/arwing.obj");
    _hero->setAttributeLocations( _textureShaderAttributeLocations.vPos,
                                   _textureShaderAttributeLocations.vNormal,
                                   _textureShaderAttributeLocations.vTexCoord);
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
            { -10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f, 0, 0 }, // 0 - BL
            {  10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f, 5, 0 }, // 1 - BR
            { -10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f, 0, 5 }, // 2 - TL
            {  10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f, 5, 5 }  // 3 - TR
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
    _fpCam = new CSCI441::FreeCam();
    _fpCam->setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    _fpCam->setTheta(0.0f);
    _fpCam->setPhi(M_PI/2.0f);
    _fpCam->setRadius(15.0f);
    _fpCam->recomputeOrientation();
    _bezCam = new CSCI441::FreeCam();
    _bezCam->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    _bezCam->setTheta(0.0f);
    _bezCam->setPhi(0.0f);
    _bezCam->setRadius(0.0f);
    _bezCam->recomputeOrientation();
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


}

//*************************************************************************************
//
// Engine Cleanup

void FPEngine::_cleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _gouraudShaderProgram;
    delete _flatShaderProgram;
    delete _textureShaderProgram;
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
    delete _hero;
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
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void FPEngine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) {
    // if either shader program is null, do not continue any further to prevent run time errors
    if(!_gouraudShaderProgram || !_flatShaderProgram || !_textureShaderProgram) {
        return;
    }

    glm::mat4 modelMatrix = glm::mat4( 1.0f );
    glm::mat4 projectionViewMatrix = projMtx * viewMtx;
    glm::mat4 mvpMatrix = projectionViewMatrix * modelMatrix;

    // use the gouraud shader
    _textureShaderProgram->useProgram();

    /// BEGIN DRAWING THE HERO ///
    glm::mat4 modelMtx(1.0f);
    //printf("%f, %f, %f | %f, %f, %f\n", viewMtx[0].x, viewMtx[0].y, viewMtx[0].z, modelMtx[0].x, modelMtx[0].y, modelMtx[0].z);

    modelMtx = glm::translate(modelMtx, _fpCam->getLookAtPoint());
    // rotate the plane with our camera theta direction (we need to rotate the opposite direction so that we always look at the back)
    modelMtx = glm::rotate(modelMtx, _angleX, CSCI441::Y_AXIS);
    modelMtx = glm::rotate(modelMtx, _angleZ, CSCI441::Z_AXIS);
    // rotate the plane with our camera phi direction
    modelMtx = glm::rotate(modelMtx, _fpCam->getPhi(), -glm::cross(_fpCam->getPosition()-_fpCam->getLookAtPoint(), _fpCam->getUpVector()));



    modelMtx = glm::rotate( modelMtx, glm::radians(180.0f), CSCI441::Y_AXIS );
    modelMtx = glm::rotate( modelMtx, glm::radians(90.0f), CSCI441::X_AXIS );
    mvpMatrix = projMtx * viewMtx * modelMtx;


    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMatrix);
    // draw our plane now
    _hero->draw(_textureShaderProgram->getShaderProgramHandle());
    //printf("%f, %f, %f | %f, %f, %f\n", viewMtx[0].x, viewMtx[0].y, viewMtx[0].z, modelMtx[2].x, modelMtx[2].y, modelMtx[2].z);


    /// BEGIN DRAWING SKYBOX ///
    CSCI441::setVertexAttributeLocations(_textureShaderAttributeLocations.vPos,
                                         _textureShaderAttributeLocations.vNormal,
                                         _textureShaderAttributeLocations.vTexCoord);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 200.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f));
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::TOP]);
    glBindVertexArray( _vaos[VAO_ID::SKYBOX] );
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    // Right
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 0.0f, 0.0f));
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
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), CSCI441::Y_AXIS);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), CSCI441::X_AXIS);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f));
    mvpMtx = projMtx * viewMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::BOTTOM]);
    glBindVertexArray( _vaos[VAO_ID::SKYBOX] );
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    // Top
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -20.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f));
    mvpMtx = projMtx * viewMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::BACK]);
    glBindVertexArray( _vaos[VAO_ID::PLATFORM] );
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::PLATFORM], GL_UNSIGNED_SHORT, (void*)0 );

    //// END DRAWING THE SKY BOX ////
}

void FPEngine::_updateScene() {
    _fpCam->setPosition(_fpCam->getPosition() - glm::vec3(0, 0, 2*_cameraSpeed.x));
    _fpCam->moveForward(0);
    _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() - glm::vec3(0, 0, 2*_cameraSpeed.x));
    _arcballCam->recomputeOrientation();

    position = _fpCam->getPosition();
    // turn right
    if (_keys[GLFW_KEY_D]) {
        if(position.x < 12) {
            //_fpCam->moveForward(_cameraSpeed.x);
            _fpCam->setPosition(_fpCam->getPosition() - glm::vec3(-_cameraSpeed.x, 0, 0));
            _fpCam->moveForward(0);
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
            //_fpCam->moveForward(_cameraSpeed.x);
            _fpCam->setPosition(_fpCam->getPosition() - glm::vec3(_cameraSpeed.x, 0, 0));
            _fpCam->moveForward(0);
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
            _fpCam->setPosition(_fpCam->getPosition() - glm::vec3(0, -_cameraSpeed.x, 0));
            _fpCam->moveForward(0);
            if (position.y > 10) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() + glm::vec3(0,_cameraSpeed.y,0));
                _arcballCam->recomputeOrientation();
            }
            if (position.y < 5) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() + glm::vec3(0,_cameraSpeed.y,0));
                _arcballCam->recomputeOrientation();
            }
            if (_fpCam->getPhi() < (2.130796)) {
                _fpCam->rotate(0.0f, 0.02f);
            }
        }
    }
    // move backward
    if (_keys[GLFW_KEY_S]) {
        if(position.y > -10) {
            _fpCam->setPosition(_fpCam->getPosition() - glm::vec3(0, _cameraSpeed.x, 0));
            _fpCam->moveForward(0);
            if (position.y < 5) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() - glm::vec3(0,_cameraSpeed.y,0));
                _arcballCam->recomputeOrientation();
            }
            if (position.y > 10) {
                _arcballCam->setLookAtPoint(_arcballCam->getLookAtPoint() - glm::vec3(0,_cameraSpeed.y,0));
                _arcballCam->recomputeOrientation();
            }
            if (_fpCam->getPhi() > (M_PI)/4.0f) {
                _fpCam->rotate(-0.0f, -0.02f);
            }
        }
    }

    if(!_keys[GLFW_KEY_S] && !_keys[GLFW_KEY_W]) {
        if (_fpCam->getPhi() != M_PI/2) {
            if (_fpCam->getPhi() > M_PI/2) {
                _fpCam->rotate(0.0f, -0.02f);
            }
            if (_fpCam->getPhi() < M_PI/2) {
                _fpCam->rotate(0.0f, 0.02f);
            }
        }
    }

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
            if (_angleZ > 0) {
                _angleZ += -0.02f;
            }
            if (_angleZ < 0) {
                _angleZ += 0.02f;
            }
        }
    }
}

void FPEngine::run() {
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(_window) ) {	        // check if the window was instructed to be closed
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
        glm::mat4 viewMatrix = _arcballCam->getViewMatrix();

        // draw everything to the window
        _renderScene(viewMatrix, projectionMatrix);

        // animate the scene
        _updateScene();

        glfwSwapBuffers(_window);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen
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
