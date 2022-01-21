#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>

// http://rapidjson.org/
#include "../../include/rapidjson/document.h"
#include "../../include/rapidjson/writer.h"
#include "../../include/rapidjson/stringbuffer.h"
#include "../../include/rapidjson/filereadstream.h"

#include "../headers/dataReader.h"
#include "UtilFunctions.cpp"

using namespace rapidjson;
using namespace std;

class DataReader 
{
  Document data;

  FontStyle getFontStyle(string style) {
    if (style == "bold") {
     return FONT_BOLD;
    } else if (style == "italic") {
      return FONT_ITALIC;
    }
    return FONT_REGULAR;
  }

  Align getTextAlign(string align) {
    if (align == "right") {
     return ALIGN_RIGHT;
    } else if (align == "center") {
      return ALIGN_CENTER;
    }
    return ALIGN_LEFT;
  }

  public:

  DataReader ( string json ) {
    data.Parse(json.c_str());
  }

  string getFooterText() {
    return getStringProperty( "footerText", data );
  }

  string getAoLogo() {
    return getStringProperty( "aoLogo", data );
  }

  string getShowName() {
    return getStringProperty( "showName", data );
  }

  string getShowLogo() {
    return getStringProperty( "showLogo", data );
  }

  string getPersonName() {
    return getStringProperty( "personName", data );
  }

  HeaderText getHeaderText() {
    HeaderText headerText;
    if ( !data.HasMember("headerText") || !data["headerText"].IsObject() ) {
      return headerText;
    }
    headerText.subText = getStringProperty( "subText", data["headerText"] );
    headerText.subTextRed = getStringProperty( "subTextRed", data["headerText"] );
    headerText.maxSize = 0;
    if ( !data["headerText"].HasMember("rows") || !data["headerText"]["rows"].IsArray() ) return headerText;
    for ( unsigned int blockNumber = 0; blockNumber < data["headerText"]["rows"].Size(); ++blockNumber ) {
      if ( blockNumber == 2 ) break;
      vector<HeaderTextEntry> headerBlock;
      headerBlock.resize(data["headerText"]["rows"][blockNumber].Size());
      
      for (unsigned int row = 0; row < headerBlock.size(); ++row) {
        if ( static_cast<int>(row) == headerText.maxSize ) headerText.maxSize++;
        headerBlock[row].text = getStringProperty( "text", data["headerText"]["rows"][blockNumber][row] );
        headerBlock[row].fontStyle = getFontStyle(
          getStringProperty( "style", data["headerText"]["rows"][blockNumber][row] )
        );
      }  
      if ( blockNumber == 0 ) {
        headerText.left = headerBlock;
      } else {
        headerText.right = headerBlock;
      }
    }
    return headerText;
  }

  vector<Table> getSections() {
    vector<Table> sections;
    if ( !data.HasMember("sections") || !data["sections"].IsArray() ) return sections;
    sections.resize( data["sections"].Size() );
    // loop through sections
    for (unsigned int i = 0; i < sections.size(); ++i) {
      if ( 
        !data["sections"][i].HasMember("columns") 
        || !data["sections"][i].HasMember("rows") 
        || !data["sections"][i]["columns"].IsArray() 
        || !data["sections"][i]["rows"].IsArray() 
      ) continue;
      sections[i].title = getStringProperty( "title", data["sections"][i] );
      sections[i].width = 0;
      sections[i].columns.resize(data["sections"][i]["columns"].Size());
      sections[i].rows.resize(
        data["sections"][i]["rows"].Size(),
        vector<string>(sections[i].columns.size())
      );

      // loop through columns
      for (unsigned int countCol = 0; countCol < sections[i].columns.size(); ++countCol) {
        sections[i].columns[countCol].text = getStringProperty( 
          "text", data["sections"][i]["columns"][countCol] 
        );
        sections[i].columns[countCol].visible = getBoolProperty( 
          "visible", data["sections"][i]["columns"][countCol], true
        );
        sections[i].columns[countCol].width = 
          ( sections[i].columns[countCol].visible )
          ? getDoubleProperty( "width", data["sections"][i]["columns"][countCol] )
          : 0
        ;
        sections[i].width += sections[i].columns[countCol].width;
        sections[i].columns[countCol].fontStyle = getFontStyle(
          getStringProperty( "style", data["sections"][i]["columns"][countCol] )
        );
        sections[i].columns[countCol].align = getTextAlign(
          getStringProperty( "align", data["sections"][i]["columns"][countCol] )
        );
        // adding cell data, looping through rows
        for (unsigned int countRow = 0; countRow < sections[i].rows.size(); ++countRow) {
          sections[i].rows[countRow][countCol] = 
            ( 
              countCol < data["sections"][i]["rows"][countRow].Size() 
              && data["sections"][i]["rows"][countRow][countCol].IsString()
            )
            ? data["sections"][i]["rows"][countRow][countCol].GetString()
            : ""
          ;
        }
      }
    }
    return sections;
  }

  Totals getTotals() {
    Totals totals;
    bool hasReductions;
    if ( 
      !data.HasMember("totals") 
      || !data["totals"].IsObject() 
      || !data["totals"].HasMember("costs") 
      || !data["totals"]["costs"].IsArray()
    ) return totals;
    totals.title = getStringProperty( "title", data["totals"] );
    totals.costs.width = 0;
    totals.finalText = getStringProperty( "finalText", data["totals"] );
    totals.finalValue = getStringProperty( "finalValue", data["totals"] );
    if ( !data["totals"].HasMember("columns") || !data["totals"]["columns"].IsArray() ) return totals;
    totals.costs.columns.resize(data["totals"]["columns"].Size());
    totals.costs.rows.resize(
      data["totals"]["costs"].Size(),
      vector<string>(totals.costs.columns.size())
    );
    hasReductions = ( data["totals"].HasMember("discounts") && data["totals"]["discounts"].IsArray() );
    if ( hasReductions ) {
      totals.reductions.columns.resize(totals.costs.columns.size());
      totals.reductions.rows.resize(
        data["totals"]["discounts"].Size(),
        vector<string>(totals.costs.columns.size())
      );
      totals.reductions.title = getStringProperty( "discountsTitle", data["totals"] );
    } else {
      totals.reductions.rows.resize( 0 );
    }
    // loop through columns
    for (unsigned int countCol = 0; countCol < totals.costs.columns.size(); ++countCol) {
      totals.costs.columns[countCol].text = getStringProperty( "text", data["totals"]["columns"][countCol] );
      totals.costs.columns[countCol].visible = getBoolProperty(
        "visible", data["totals"]["columns"][countCol]
      );
      totals.costs.columns[countCol].width = 
        ( totals.costs.columns[countCol].visible )
        ? getDoubleProperty( "width", data["totals"]["columns"][countCol] )
        : 0
      ;
      totals.costs.width += totals.costs.columns[countCol].width;
      totals.costs.columns[countCol].fontStyle = getFontStyle(
        getStringProperty( "style", data["totals"]["columns"][countCol] )
      );
      totals.costs.columns[countCol].align = getTextAlign(
        getStringProperty( "align", data["totals"]["columns"][countCol] )
      );
      if ( hasReductions ) {
        totals.reductions.columns[countCol].text = totals.costs.columns[countCol].text;
        totals.reductions.columns[countCol].width = totals.costs.columns[countCol].width;
        totals.reductions.columns[countCol].fontStyle = totals.costs.columns[countCol].fontStyle;
        totals.reductions.columns[countCol].align = totals.costs.columns[countCol].align;
        totals.reductions.columns[countCol].visible = totals.costs.columns[countCol].visible;
      }
      // Costs: adding cell data, looping through rows
      for (unsigned int countRow = 0; countRow < totals.costs.rows.size(); ++countRow) {
        totals.costs.rows[countRow][countCol] = 
          ( 
            countCol < data["totals"]["costs"][countRow].Size() 
            && data["totals"]["costs"][countRow][countCol].IsString()
          )
          ? data["totals"]["costs"][countRow][countCol].GetString()
          : ""
        ;
      }
      // Reductions: adding cell data, looping through rows
      if ( hasReductions ) {
        for (unsigned int countRow = 0; countRow < totals.reductions.rows.size(); ++countRow) {
          totals.reductions.rows[countRow][countCol] = 
            ( 
              countCol < data["totals"]["discounts"][countRow].Size() 
              && data["totals"]["discounts"][countRow][countCol].IsString()
            )
            ? data["totals"]["discounts"][countRow][countCol].GetString()
            : ""
          ;
        }
      }
    }
    if ( hasReductions ) totals.reductions.width = totals.costs.width;
    return totals;
  }

  Extras getExtras() {
    Extras extras;
    extras.title = 
      ( data["extras"].HasMember("title") )
      ? getStringProperty( "title", data["extras"] )
      : ""
    ;
    if ( !data["extras"].HasMember("lines") || !data["extras"]["lines"].IsArray() ) return extras;
    extras.lines.resize( data["extras"]["lines"].Size());
    for (unsigned int i = 0; i < extras.lines.size(); ++i) {
      extras.lines[i] = 
        ( data["extras"]["lines"][i].IsString() )
        ? data["extras"]["lines"][i].GetString()
        : ""
      ;
    }
    return extras;
  }

  // string getMetadataCommand() {
  //   // echo "-json" | exiftool -@ - test.pdf
  //   if ( getExifJsonName() == "" || getFileName() == "" ) return "";
  //   return string("exiftool -overwrite_original -fast3 ")
  //   + "-json=" + getExifJsonName() + " "
  //   + getFileName() 
  //   + " >/dev/null 2>&1";
  // }

  // string getFileName() {
  //   if ( data.HasMember("fileName") && data["fileName"].IsString() ) {
  //     return string("output/") + data["fileName"].GetString() + ".pdf";
  //   } else  return "";
  // }

  // string getExifJsonName() {
  //   if ( data.HasMember("fileName") && data["fileName"].IsString() ) {
  //   return string("/tmp/") + data["fileName"].GetString() + ".json";
  //   } else  return "";
  // }

  // void writeMetadataJson() {
  //   if ( data.HasMember("metadata") & data["metadata"].IsObject() ) {
  //     ofstream file;
  //     file.open ( getExifJsonName() );
  //     StringBuffer buffer;
  //     Writer<StringBuffer> writer(buffer);
  //     data["metadata"].AddMember("author", "Animal Office", data.GetAllocator());
  //     data["metadata"].AddMember("creator", "Animal Office", data.GetAllocator());
  //     data["metadata"].AddMember("sourceFile", "*", data.GetAllocator());
  //     data["metadata"].Accept(writer);
  //     file << buffer.GetString();
  //     file.close();
  //   }
  // }

  string getStringProperty( string property, const auto &data ) 
  {
    if ( data.HasMember( property.c_str() ) && data[ property.c_str() ].IsString() ) {
      return data[ property.c_str() ].GetString();
    } else {
      return "";
    }
  }

  bool getBoolProperty( string property, const auto &data, bool defaultValue = false )
  {
    if ( data.HasMember( property.c_str() ) && data[ property.c_str() ].IsBool() ) {
      return data[ property.c_str() ].GetBool();
    } else {
      return defaultValue;
    }
  }

  double getDoubleProperty( string property, const auto &data )
  {
    if ( data.HasMember( property.c_str() ) && data[ property.c_str() ].IsNumber() ) {
      return data[ property.c_str() ].GetDouble();
    } else {
      return 0;
    }
  }
};