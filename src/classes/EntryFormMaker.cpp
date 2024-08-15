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
    logoHeight.show = 125;
    logoHeight.ao = 30;

    footerText = "";
    
    font.size.title = 30;
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
    font.color.name = 0xab5e2b;
    font.color.title = 0xc5a15f;
    font.color.heading = 0xc5a15f;
    font.color.headingLight = 0xab5e2b;
    font.color.headingSubText = 0x727272;
    font.color.footer = 0x969696;
    font.color.red = 0xbb2222;
    // font.color.bold = 0x964007;
    font.color.bold = 0xab5e2b;
    font.color.light = 0xab5e2b;
    font.color.italic = 0x525252;
    
    font.lineHeight.topBar = 12;
    font.lineHeight.tableBody = 18;
    font.lineHeight.extras = 15;

    line.thickness.header = .5;
    line.thickness.tableHead = .5;
    line.thickness.tableBody = .5;
    line.color.header = 0xc5a15f;
    line.color.tableHead = 0xd1c9ba;
    line.color.tableBody = 0xe1d9ca;
  }

  void drawTitleBar(string text) {
    pdfMaker->newPageAt( 120 );
    pdfMaker->drawHorizontalLine( line.thickness.header, line.color.header );
    pdfMaker->moveDown(27);
    pdfMaker->setFont( FONT_HEADING, font.size.heading, font.color.headingLight );
    pdfMaker->writeText( text, ALIGN_CENTER );
    pdfMaker->moveDown(11);
    pdfMaker->drawHorizontalLine( line.thickness.header, line.color.header );
  }

  void drawTableHeader(Table table, double widthMultiplier) 
  {
    pdfMaker->setFont( FONT_BOLD, font.size.tableHead, font.color.bold );
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
      if (countCol < (int) table.columns.size() - 1) {
        updateTablePos( xpos, extraWidth, textWidth, columnWidth );
      }
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
        double color = font.color.text;
        if ( table.columns[countCol].light ) color = font.color.light;
        pdfMaker->setFont( table.columns[countCol].fontStyle, font.size.tableBody, color );
        double columnWidth = table.columns[countCol].width * widthMultiplier;
        double textWidth = pdfMaker->getTextWidth( table.rows[countRow][countCol] );
        pdfMaker->writeText( 
          table.rows[countRow][countCol],
          table.columns[countCol].align,
          0,
          xpos,
          xpos + columnWidth
        );
        if (countCol < (int) table.columns.size() - 1) {
          updateTablePos( xpos, extraWidth, textWidth, columnWidth );
        }
      }
      pdfMaker->moveDown( 5 );
      pdfMaker->drawHorizontalLine( line.thickness.tableBody, line.color.tableBody );
    }
    pdfMaker->moveDown( 40 );
  }

  void updateTablePos( double& xpos, double& extraWidth, double textWidth, double columnWidth ) {
    int maxSpaceBetweenColumns = 6;
    xpos += columnWidth;
    if ( textWidth > columnWidth - maxSpaceBetweenColumns) {
      double newExtraWidth = textWidth - columnWidth;
      xpos += newExtraWidth;
      extraWidth += newExtraWidth + addHorSpacingToTableField(xpos);
    } else if ( extraWidth > 0) {
      double lessWidth = columnWidth - textWidth - maxSpaceBetweenColumns;
      if ( lessWidth > extraWidth ) lessWidth = extraWidth;
      xpos -= lessWidth;
      extraWidth -= lessWidth;
      if ( extraWidth > 0 ) {
        xpos -= maxSpaceBetweenColumns;
        extraWidth += addHorSpacingToTableField(xpos) - maxSpaceBetweenColumns;
      }
    }
  }

  double addHorSpacingToTableField(double& xpos) {
    pdfMaker->setFontColor( font.color.heading);
    pdfMaker->writeText( 
      " | ",
      ALIGN_LEFT,
      0,
      xpos
    );
    double spacing = pdfMaker->getTextWidth(" | ");
    xpos += spacing;
    return spacing;
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
    // pdfMaker->drawImage( aoLogoFilename, logoHeight.ao, ALIGN_RIGHT, false );
    pdfMaker->drawImage( filename, logoHeight.show, ALIGN_RIGHT );
    pdfMaker->moveDown( 30 );
  }

  void drawShowName( string showName ) {
    pdfMaker->setFont( FONT_TITLE, font.size.title, font.color.title );
    pdfMaker->writeText( showName, ALIGN_LEFT, 56 );
  }

  void drawPersonName (string name) {
    pdfMaker->setFont( FONT_HEADING, font.size.heading, font.color.name );
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
      double color = font.color.text;
      if (headerText.right[i].fontStyle == FONT_BOLD) color = font.color.bold;
      pdfMaker->setFont( headerText.right[i].fontStyle, font.size.topBar, color );
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
      pdfMaker->setFont( FONT_REGULAR, font.size.extras );
      pdfMaker->writeMultiText( "<accent>—</accent> " + extras.lines[i], font.size.extras, font.lineHeight.extras, font.color.heading );
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