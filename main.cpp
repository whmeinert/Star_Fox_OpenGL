/*
 *  CSCI 441, Computer Graphics, Fall 2022
 *
 *  Project: Final Project
 *  File: main.cpp
 *
 *  Description:
 *      This file contains an emulation of a rail movement game like Star Fox 64.
 *
 *  Authors: Will Meinert, Emily Hepperlen, Mark Holladay, Colorado School of Mines, 2022
 *
 */

#include "FPEngine.hpp"

///*****************************************************************************
//
// Our main function
int main() {

    auto assingEngine = new FPEngine();
    assingEngine->initialize();
    if (assingEngine->getError() == CSCI441::OpenGLEngine::OPENGL_ENGINE_ERROR_NO_ERROR) {
        assingEngine->run();
    }
    assingEngine->shutdown();
    delete assingEngine;

    return EXIT_SUCCESS;
}
