/****************************************************************************
 *
 * $Id: servoSimuPoint3DCamVelocity.cpp,v 1.3 2007-04-20 14:22:15 asaunier Exp $
 *
 * Copyright (C) 1998-2006 Inria. All rights reserved.
 *
 * This software was developed at:
 * IRISA/INRIA Rennes
 * Projet Lagadic
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * http://www.irisa.fr/lagadic
 *
 * This file is part of the ViSP toolkit
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE included in the packaging of this file.
 *
 * Licensees holding valid ViSP Professional Edition licenses may
 * use this file in accordance with the ViSP Commercial License
 * Agreement provided with the Software.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Contact visp@irisa.fr if any conditions of this licensing are
 * not clear to you.
 *
 * Description:
 * Simulation of a 3D visual servoing on a 3D point.
 *
 * Authors:
 * Eric Marchand
 * Fabien Spindler
 *
 *****************************************************************************/

/*!
  \file servoSimuPoint3DCamVelocity.cpp
  \brief Simulation of a 3D visual servoing:
  - servo a 3D point,
  - eye-in-hand control law,
  - velocity computed in the camera frame,
  - no display.

*/

/*!
  \example servoSimuPoint3DCamVelocity.cpp
  Simulation of a 3D visual servoing:
  - servo a 3D point,
  - eye-in-hand control law,
  - velocity computed in the camera frame,
  - no display.

*/

#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpFeaturePoint3D.h>
#include <visp/vpPoint.h>
#include <visp/vpServo.h>
#include <visp/vpRobotCamera.h>
#include <visp/vpDebug.h>
#include <visp/vpParseArgv.h>

// List of allowed command line options
#define GETOPTARGS	"h"

/*!

Print the program options.

  \param name : Program name.
  \param badparam : Bad parameter name.

*/
void usage(char *name, char *badparam)
{
  fprintf(stdout, "\n\
Simulation of a 3D visual servoing:\n\
- servo a 3D point,\n\
- eye-in-hand control law,\n\
- velocity computed in the camera frame,\n\
- without display.\n\
\n\
SYNOPSIS\n\
  %s [-h]\n", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
\n\
  -h\n\
     Print the help.\n");

}

/*!

Set the program options.

  \param argc : Command line number of parameters.
  \param argv : Array of command line parameters.
  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, char **argv)
{
  char *optarg;
  int	c;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg)) > 1) {

    switch (c) {
    case 'h': usage(argv[0], NULL); return false; break;

    default:
      usage(argv[0], optarg);
      return false; break;
    }
  }

  if ((c == 1) || (c == -1)) {
    // standalone param or error
    usage(argv[0], NULL);
    std::cerr << "ERROR: " << std::endl;
    std::cerr << "  Bad argument " << optarg << std::endl << std::endl;
    return false;
  }

  return true;
}

int
main(int argc, char ** argv)
{
  // Read the command line options
  if (getOptions(argc, argv) == false) {
    exit (-1);
  }

  vpServo task ;
  vpRobotCamera robot ;

  std::cout << std::endl ;
  std::cout << "-------------------------------------------------------" << std::endl ;
  std::cout << " Test program for vpServo "  <<std::endl ;
  std::cout << " Eye-in-hand task control, velocity computed in the camera frame" << std::endl ;
  std::cout << " Simulation " << std::endl ;
  std::cout << " task : servo a 3D point " << std::endl ;
  std::cout << "-------------------------------------------------------" << std::endl ;
  std::cout << std::endl ;


  vpTRACE("sets the initial camera location " ) ;
  vpHomogeneousMatrix cMo ;
  cMo[0][3] = 0.1 ;
  cMo[1][3] = 0.2 ;
  cMo[2][3] = 2 ;
  robot.setPosition(cMo) ;


  vpTRACE("sets the point coordinates in the world frame "  ) ;
  vpPoint point ;
  point.setWorldCoordinates(0,0,0) ;


  vpTRACE("project : computes  the point coordinates in the camera frame "  ) ;
  point.track(cMo) ;

  std::cout << point.cP.t() ;

  vpFeaturePoint3D p ;
  p.buildFrom(point) ;

  vpTRACE("sets the desired position of the point ") ;
  vpFeaturePoint3D pd ;
  pd.set_XYZ(0,0,1) ;



  vpTRACE("define the task") ;
  vpTRACE("\t we want an eye-in-hand control law") ;
  vpTRACE("\t robot is controlled in the camera frame") ;
  task.setServo(vpServo::EYEINHAND_CAMERA) ;

  vpTRACE("\t we want to see a point on a point..") ;
  std::cout << std::endl ;
  task.addFeature(p,pd) ;

  vpTRACE("\t set the gain") ;
  task.setLambda(0.1) ;


  vpTRACE("Display task information " ) ;
  task.print() ;

  int iter=0 ;
  vpTRACE("\t loop") ;
  while(iter++<200)
    {
      std::cout << "---------------------------------------------" << iter <<std::endl ;
      vpColVector v ;

      if (iter==1) vpTRACE("\t\t get the robot position ") ;
      robot.getPosition(cMo) ;
      if (iter==1) vpTRACE("\t\t new point position ") ;
      point.track(cMo) ;
      p.buildFrom(point) ;
      //   std::cout << p.cP.t() ;
      //   std::cout << (p.get_s()).t() ;


      if (iter==1) vpTRACE("\t\t compute the control law ") ;
      v = task.computeControlLaw() ;
      //  vpTRACE("computeControlLaw" ) ;

      if (iter==1) vpTRACE("\t\t send the camera velocity to the controller ") ;
      robot.setVelocity(vpRobot::CAMERA_FRAME, v) ;

      //   vpTRACE("\t\t || s - s* || ") ;
      std::cout << task.error.sumSquare() <<std::endl ; ;
    }

  vpTRACE("Display task information " ) ;
  task.print() ;
}

