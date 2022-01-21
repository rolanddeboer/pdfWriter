#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include "headers/dataReader.h"

#include "classes/EntryFormMaker.cpp"
#include "classes/DataReader.cpp"

using namespace std;

int main( ) 
{
  DataReader* dataReader;
  EntryFormMaker* entryFormMaker;
  string line;

  entryFormMaker = new EntryFormMaker;

// The first line of input contains only the footer text and the Animal Office logo location.
  getline( cin, line );
  dataReader = new DataReader ( line );
  entryFormMaker->setFooterText ( dataReader->getFooterText() );
  entryFormMaker->setAoLogo ( dataReader->getAoLogo() );
  delete dataReader;

  // entryFormMaker->setFooterText ( "hi" );
  // entryFormMaker->setAoLogo ( "/var/www/html/ao2/server/public/images/logo/pdf-logo-ao.png" );

// Every input line that comes next contains one entry form.
  while ( getline(cin, line) ) {

    dataReader = new DataReader ( line );

    entryFormMaker->newPage();
    entryFormMaker->drawLogos( dataReader->getShowLogo() );
    entryFormMaker->drawShowName( dataReader->getShowName() );
    entryFormMaker->drawPersonName( dataReader->getPersonName() );
    entryFormMaker->drawHeader( dataReader->getHeaderText() );
    entryFormMaker->drawSections( dataReader->getSections() );
    entryFormMaker->drawTotals( dataReader->getTotals() );
    entryFormMaker->drawExtras( dataReader->getExtras() );

    delete dataReader;
  }
  
  entryFormMaker->close();

  delete entryFormMaker;

  return 0;

}
