#include "../headers/dataReader.h"
#include "../headers/pdfTypes.h"
#include "../headers/entryFormTypes.h"

#include "PdfMaker.cpp"
#include "UtilFunctions.cpp"

using namespace std;

class EntryFormMaker {
  LogoHeight logoHeight;
  EntryFormFont font;
  Line line;
  string aoLogoFilename, footerText;
  PdfMaker* pdfMaker;

  void init() {
    logoHeight.show = 75;
    logoHeight.ao = 30;

    footerText = "";
    
    font.size.title = 26;
    font.size.heading = 22;
    font.size.topBar = 10;
    font.size.tableHead = 10;
    font.size.tableBody = 9;
    font.size.text = 11;
    font.size.extras = 10;
    font.size.footer = 10;
    font.size.sectionTitle = 18;
    font.size.totalsFooter = 16;

    font.color.text = 0;
    font.color.title = 0x4B4B4B;
    font.color.heading = 0x323232;
    font.color.headingSubText = 0x323232;
    font.color.footer = 0x969696;
    font.color.red = 0xbb2222;
    
    font.lineHeight.topBar = 12;
    font.lineHeight.tableBody = 18;
    font.lineHeight.extras = 15;

    line.thickness.header = 1;
    line.thickness.tableHead = .5;
    line.thickness.tableBody = .5;
    line.color.header = 0x444444;
    line.color.tableHead = 0x111111;
    line.color.tableBody = 0xAAAAAA;
  }

  void drawTitleBar(string text) {
    pdfMaker->newPageAt( 120 );
    pdfMaker->drawHorizontalLine( line.thickness.header, line.color.header );
    pdfMaker->moveDown(27);
    pdfMaker->setFont( FONT_HEADING, font.size.heading, font.color.heading );
    pdfMaker->writeText( text, ALIGN_CENTER );
    pdfMaker->moveDown(11);
    pdfMaker->drawHorizontalLine( line.thickness.header, line.color.header );
  }

  void drawTableHeader(Table table, double widthMultiplier) 
  {
    pdfMaker->setFont( FONT_BOLD, font.size.tableHead );
    double xpos = 0;
    double extraWidth = 0;
    for ( int countCol = 0; countCol < (int) table.columns.size(); countCol++ ) {
      if (! table.columns[countCol].visible ) continue;
      double columnWidth = table.columns[countCol].width * widthMultiplier;
      double textWidth = pdfMaker->getTextWidth( table.columns[countCol].text );
      pdfMaker->writeText( 
        table.columns[countCol].text,
        table.columns[countCol].align,
        0,
        xpos,
        xpos + columnWidth
      );
      updateTablePos( xpos, extraWidth, textWidth, columnWidth );
    }
    pdfMaker->moveDown( 5 );
    pdfMaker->drawHorizontalLine( line.thickness.tableBody, line.color.tableHead );
  }

  void drawTable(Table table) 
  {
    double widthMultiplier = pdfMaker->getPageWidth() / table.width;
    drawTableHeader(table, widthMultiplier);
    for ( int countRow = 0; countRow < (int) table.rows.size(); countRow++ ) {
      if ( pdfMaker->newPageAt( 35, 8 ) ) drawTableHeader(table, widthMultiplier);
      pdfMaker->moveDown( font.lineHeight.tableBody - 5);
      double xpos = 0;
      double extraWidth = 0;
      for ( int countCol = 0; countCol < (int) table.columns.size(); countCol++ ) {
        if (! table.columns[countCol].visible ) continue;
        pdfMaker->setFont( table.columns[countCol].fontStyle, font.size.tableBody );
        double columnWidth = table.columns[countCol].width * widthMultiplier;
        double textWidth = pdfMaker->getTextWidth( table.rows[countRow][countCol] );
        pdfMaker->writeText( 
          table.rows[countRow][countCol],
          table.columns[countCol].align,
          0,
          xpos,
          xpos + columnWidth
        );
        updateTablePos( xpos, extraWidth, textWidth, columnWidth );
      }
      pdfMaker->moveDown( 5 );
      pdfMaker->drawHorizontalLine( line.thickness.tableBody, line.color.tableBody );
    }
    pdfMaker->moveDown( 40 );
  }

  void updateTablePos( double& xpos, double& extraWidth, double textWidth, double columnWidth ) {
    xpos += columnWidth;
    if ( textWidth > columnWidth ) {
      extraWidth += textWidth - columnWidth + 10;
      xpos += extraWidth;
    } else if ( extraWidth > 0) {
      double lessWidth = columnWidth - textWidth + 10;
      if ( lessWidth > extraWidth ) lessWidth = extraWidth;
      xpos -= lessWidth;
      extraWidth -= lessWidth;
    }
  }

  public:

  EntryFormMaker ( ) {
    pdfMaker = new PdfMaker( );
    init();
  }

  EntryFormMaker ( string filename ) {
    pdfMaker = new PdfMaker( filename );
    init();
  }

  void newPage() {
    pdfMaker->newPage();
  }

  void setFooterText ( string footer ) {
    footerText = footer;
  }

  void setAoLogo ( string aoLogo ) {
    aoLogoFilename = aoLogo;
  }

  void drawLogos(string filename) {
    pdfMaker->drawImage( aoLogoFilename, logoHeight.ao, ALIGN_RIGHT, false );
    pdfMaker->drawImage( filename, logoHeight.show );
    pdfMaker->moveDown( 30 );
  }

  void drawShowName( string showName ) {
    pdfMaker->setFont( FONT_TITLE, font.size.title, font.color.title );
    pdfMaker->writeText( showName, ALIGN_LEFT, 56 );
  }

  void drawPersonName (string name) {
    pdfMaker->setFont( FONT_HEADING, font.size.heading, font.color.heading );
    pdfMaker->writeText( name, ALIGN_LEFT, 2);
  }

  void drawHeader(HeaderText headerText) 
  {
    // red text
    pdfMaker->setFont( FONT_BOLD, font.size.topBar, font.color.red );
    pdfMaker->writeText( headerText.subTextRed, ALIGN_RIGHT );
    pdfMaker->moveDown( 8 );

    // top line
    pdfMaker->drawHorizontalLine( line.thickness.header, line.color.header );
    pdfMaker->moveDown( 15 );
    
    // left text
    pdfMaker->saveYpos();
    for ( int i = 0; i < (int) headerText.left.size(); i++ ) {
      pdfMaker->setFont( headerText.left[i].fontStyle, font.size.topBar );
      pdfMaker->writeText( headerText.left[i].text, ALIGN_LEFT, font.lineHeight.topBar );
    }   

    // right text
    pdfMaker->restoreYpos();
    for ( int i = 0; i < (int) headerText.right.size(); i++ ) {
      pdfMaker->setFont( headerText.right[i].fontStyle, font.size.topBar );
      pdfMaker->writeText( headerText.right[i].text, ALIGN_RIGHT, font.lineHeight.topBar );
    }   

    // bottom line
    pdfMaker->restoreYpos();
    pdfMaker->moveDown( headerText.maxSize * font.lineHeight.topBar - 3);
    pdfMaker->drawHorizontalLine( line.thickness.header, line.color.header );
    pdfMaker->moveDown( 15 );

    // sub text
    pdfMaker->setFont( FONT_ITALIC, font.size.topBar, font.color.headingSubText );
    pdfMaker->writeText( headerText.subText, ALIGN_RIGHT );
  }

  void drawSections(vector<Table> sections)
  {
    pdfMaker->moveDown( 28 );
    for ( int i = 0; i < (int) sections.size(); i++ ) {
      pdfMaker->newPageAt( 85, 15 );
      // draw title
      pdfMaker->setFont( FONT_HEADING, font.size.sectionTitle, font.color.heading );
      pdfMaker->writeText( sections[i].title );
      pdfMaker->moveDown ( 26 );
      // draw table
      drawTable( sections[i] );
    }
  }

  void drawTotals(Totals totals) {
    drawTitleBar( totals.title );
    pdfMaker->moveDown( 30 );
    drawTable( totals.costs );
    if ( totals.reductions.rows.size() > 0 ) {
      pdfMaker->moveDown( -10 );
      pdfMaker->newPageAt( 75, 15 );
      pdfMaker->setFont( FONT_HEADING, font.size.totalsFooter, font.color.heading );
      pdfMaker->writeText( totals.reductions.title );
      pdfMaker->moveDown( 20 );
      drawTable( totals.reductions );  
    }
    pdfMaker->moveDown( -10 );
    pdfMaker->newPageAt( 10, 15 );
    pdfMaker->setFont( FONT_HEADING, font.size.totalsFooter, font.color.heading );
    pdfMaker->writeText( totals.finalText );
    pdfMaker->writeText( totals.finalValue, ALIGN_RIGHT );
    pdfMaker->moveDown( 40 );
  }

  void drawExtras( Extras extras ) {
    drawTitleBar( extras.title );
    pdfMaker->moveDown( 5 );
    pdfMaker->setFont( FONT_REGULAR, font.size.extras );
    for ( int i = 0; i < (int) extras.lines.size(); ++i ) {
      pdfMaker->moveDown( 5 );
      pdfMaker->newPageAt( 60, -7 );
      pdfMaker->writeMultiText( "• " + extras.lines[i], font.size.extras, font.lineHeight.extras );
    }
  }

  void close() {
    pdfMaker->drawFooterAndClose( footerText, FONT_ITALIC, font.size.footer, font.color.footer );
    delete pdfMaker;
  }

  void writeMetadata( string command, string exifJsonName ) {
    if ( system( command.c_str() ) != 0 ) {
      cerr << "Problem setting PDF metadata\n";
    };
    remove( exifJsonName.c_str() );
  }

};