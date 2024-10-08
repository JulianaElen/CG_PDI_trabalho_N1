// Elo Maluco
// main.cpp

#include <iostream>
#include <thread>

#include "wraps.hpp"
#include "application.hpp"

     
// Programa Principal 
int main(int argc, char** argv)
{
     int time=0;

     Application app(argc,argv);
     ptr_ = &app;

     glutDisplayFunc(draw);
     glutReshapeFunc(resize);
     glutKeyboardFunc(KeyboardHandle);
     glutMouseFunc(MouseHandle);
     glutSpecialFunc(SpecialKeyHandle); 
     glutTimerFunc(30,update,time);
     glutMainLoop();

     return 0;
}
