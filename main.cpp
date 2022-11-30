/*
 *  CSCI 441, Computer Graphics, Fall 2022
 *
 *  Project: lab08
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the basic setup to evaluate Bézier curves.
 *
 *  Author: Dr. Paone, Colorado School of Mines, 2022
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
