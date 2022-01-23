#ifndef UTILFUNCTIONS_CPP
#define UTILFUNCTIONS_CPP

#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <iostream>

using namespace std;

class UtilFunctions
{

  public:

  UtilFunctions () { }

  static string getAssetsDir() {
    return "/usr/local/share/ao/";
  }

  static void mustAccessFile( string filename ) {
    if ( access( filename.c_str(), 0 ) != 0 ) {
      cerr << "Unable to access file: " << filename << "\n";
      exit (EXIT_FAILURE);
    };
  }

};

#endif