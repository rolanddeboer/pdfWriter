#include <string>
#include <stdlib.h>
#include <regex>

// https://github.com/galkahana/PDF-Writer/wiki
#include "../../lib/hummus/PDFWriter/PDFWriter.h"
#include "../../lib/hummus/PDFWriter/PDFPage.h"
#include "../../lib/hummus/PDFWriter/PDFUsedFont.h"
#include "../../lib/hummus/PDFWriter/AbstractContentContext.h"
#include "../../lib/hummus/PDFWriter/PageContentContext.h"

#include "../headers/pdfTypes.h"
#include "UtilFunctions.cpp"

using namespace std;

class PdfMaker 
{
  PDFWriter pdfWriter;
  OutputStringBufferStream* outputBuffer;
  vector<PDFPage*> pages;
  vector<PageContentContext*> contents;
  vector<PDFUsedFont*> fonts;
  Font currentFont;
  int currentPageNumber;
  Page page;
  bool writingToFile;

  void init() {
    page.margin.top = page.margin.right = page.margin.bottom = page.margin.left = 35;
    page.height = 842;
    page.width = 595;
    currentPageNumber = -1;
    initFonts();
  }

  void initFonts() {
    string fontFiles[] = {
      "Merriweather/Merriweather-Bold.ttf",
      "Merriweather/Merriweather-Regular.ttf",
      "Roboto/Roboto-Regular.ttf",
      "Roboto/Roboto-Bold.ttf",
      "Roboto/Roboto-Italic.ttf"
    };
    for (int i = 0; i < 5; i++) {
      fontFiles[i] = UtilFunctions::getAssetsDir() + "/fonts/" + fontFiles[i];
      UtilFunctions::mustAccessFile(fontFiles[i]);
      fonts.push_back( pdfWriter.GetFontForFile( fontFiles[i]) );
    }
  }

  AbstractContentContext::TextOptions getTextOptions() {
    AbstractContentContext::TextOptions textOptions(
      fonts[currentFont.style], currentFont.size, AbstractContentContext::eRGB, currentFont.color
    );
    return textOptions;
  }

  public:

  PdfMaker () {
    writingToFile = false;
    outputBuffer = new OutputStringBufferStream;
    pdfWriter.StartPDFForStream(
      outputBuffer,
			ePDFVersion13
		);
    init();
  }

  PdfMaker ( string filename ) {
    writingToFile = true;
    pdfWriter.StartPDF( filename, ePDFVersion13 );
    init();
  }

  void drawFooterAndClose( string footerText, FontStyle fontStyle, int fontSize, int fontColor ) {
    closePage();
    for (int i = 0; i <= currentPageNumber; i++) {
      setFont( fontStyle, fontSize, fontColor );
      string fullFooterText = footerText + " " + to_string(i + 1) + "/" + to_string(currentPageNumber + 1);
      double width = getTextWidth( fullFooterText );
      contents[i]->WriteText( 
        page.width - page.margin.right - width, 
        page.margin.bottom, 
        fullFooterText, 
        getTextOptions() 
      );
      pdfWriter.EndPageContentContext( contents[i] );
      pdfWriter.WritePageAndRelease( pages[i] );
    }
    if ( writingToFile ) {
      pdfWriter.EndPDF();
    } else {
      pdfWriter.EndPDFForStream();
      cout << outputBuffer->ToString();
      delete outputBuffer;
    }
  }

  bool areWeWritingToFile() {
    return writingToFile;
  }

  void closePage() {
    if (currentPageNumber >= 0) pdfWriter.PausePageContentContext( contents[currentPageNumber] );
  }

  void newPage() {
    closePage();

    cout << outputBuffer->ToString();
    outputBuffer->Reset();
    
    pages.push_back( new PDFPage() );
    currentPageNumber ++;
    pages[currentPageNumber]->SetMediaBox( PDFRectangle(0, 0, page.width, page.height) );
    contents.push_back( pdfWriter.StartPageContentContext(pages[currentPageNumber]) );
    page.ypos = page.savedYpos = page.margin.top;
  }

  void setFont ( FontStyle style, int size, double color = 0 ) {
    currentFont.style = style;
    currentFont.size = size;
    currentFont.color = color;
  }

  double getPageWidth() {
    return page.width - page.margin.left - page.margin.right;
  }
  
  void saveYpos () { page.savedYpos = page.ypos; }
  void restoreYpos() { page.ypos = page.savedYpos; }

  void writeText ( 
    string text, 
    Align align = ALIGN_LEFT, 
    double moveDownAmount = 0, 
    double left = 0,
    double right = 0
  ) {
    if ( right == 0 ) right = getPageWidth();
    double x = left;
    if (align != ALIGN_LEFT) {
      double width = getTextWidth( text );
      if (align == ALIGN_CENTER) x = (right + left - width) / 2;
      if (align == ALIGN_RIGHT) x = right - width;
    }
    contents[currentPageNumber]->WriteText( 
      x + page.margin.left, 
      page.height - page.ypos, 
      text, 
      getTextOptions() 
    );
    moveDown ( moveDownAmount );
  }

  void drawImage(string filename, double height, Align align = ALIGN_LEFT, bool moveDownAfter = true) {
    UtilFunctions::mustAccessFile( filename );
    DoubleAndDoublePair imageDimensions = pdfWriter.GetImageDimensions( filename );
    AbstractContentContext::ImageOptions imageOptions;
    imageOptions.transformationMethod = AbstractContentContext::eFit;
    imageOptions.boundingBoxHeight = height;
    imageOptions.boundingBoxWidth = (height / imageDimensions.second) * imageDimensions.first;
    int left = page.margin.left;
    if ( align == ALIGN_CENTER ) left = (page.width - imageOptions.boundingBoxWidth) / 2;
    if ( align == ALIGN_RIGHT ) left = page.width - page.margin.right - imageOptions.boundingBoxWidth;
    contents[currentPageNumber]->DrawImage(
      left, 
      page.height - page.ypos - imageOptions.boundingBoxHeight, 
      filename, 
      imageOptions
    );
    if ( moveDownAfter ) moveDown( height );
  }

  void moveDown ( double height ) {
    page.ypos += height;
  }

  bool newPageAt ( double maxFromBottom, double yposNewPage = 0 ) {
    if ( page.height - page.margin.bottom <= page.ypos + maxFromBottom ) {
      newPage();
      page.ypos += yposNewPage;
      return true;
    }
    return false;
  }

  void drawHorizontalLine( double thickness, int color ) {
    AbstractContentContext::GraphicOptions pathStrokeOptions(
      AbstractContentContext::eStroke,
      AbstractContentContext::eRGB,
      color,
      thickness
    );
    DoubleAndDoublePairList pathPoints;
    pathPoints.push_back(DoubleAndDoublePair( page.margin.left, page.height - page.ypos ));
    pathPoints.push_back(DoubleAndDoublePair( page.width - page.margin.right, page.height - page.ypos ));
    contents[currentPageNumber]->DrawPath(pathPoints,pathStrokeOptions);
  }
  
  double getTextWidth ( string text ) {
    PDFUsedFont::TextMeasures textDimensions;
    textDimensions = fonts[currentFont.style]->CalculateTextDimensions( text, currentFont.size );
    return textDimensions.width;
  }

  void writeMultiText( string text, int fontSize, double lineHeight ) {
    text = regex_replace( text, regex("<bold>"), " <bold> " );
    text = regex_replace( text, regex("</bold>"), " </bold> " );
    text = regex_replace( text, regex("<italic>"), " <italic> " );
    text = regex_replace( text, regex("</italic>"), " </italic> " );
    text = regex_replace( text, regex("  "), " " );
    // split the text up in words
    istringstream iss(text);
    vector<string> words(
      istream_iterator<string>{iss}, 
      istream_iterator<string>()
    );
    double xpos = 0;
    moveDown( lineHeight );
    setFont( FONT_REGULAR, fontSize );
    for ( int i = 0; i < (int) words.size(); i++) {
      if (words[i] == "<bold>" ) {
        setFont( FONT_BOLD, fontSize );
        continue;
      }
      if (words[i] == "<italic>" ) {
        setFont( FONT_ITALIC, fontSize );
        continue;
      }
      if (words[i] == "</bold>" || words[i] == "</italic>") {
        setFont( FONT_REGULAR, fontSize );
        continue;
      }
      double width = .5 + getTextWidth( " " + words[i] );
      if ( xpos + width > getPageWidth() ) {
        xpos = 0;
        moveDown( lineHeight );
      }
      writeText( words[i], ALIGN_LEFT, 0, xpos );
      xpos += width;
    }
  }

};
