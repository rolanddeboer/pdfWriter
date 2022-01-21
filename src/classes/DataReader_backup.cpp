#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>

// http://rapidjson.org/
#include "../../lib/rapidjson/document.h"
#include "../../lib/rapidjson/writer.h"
#include "../../lib/rapidjson/stringbuffer.h"
#include "../../lib/rapidjson/filereadstream.h"

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

  string getShowName() {
    if ( data.HasMember("showName") && data["showName"].IsString() ) {
      return data["showName"].GetString();
    } else {
      return "";
    }
  }

  string getShowLogo() {
    if ( data.HasMember("showLogo") && data["showLogo"].IsString() ) {
      return data["showLogo"].GetString();
    } else {
      return "";
    }
  }

  string getPersonName() {
    if ( data.HasMember("personName") && data["personName"].IsString() ) {
      return data["personName"].GetString();
    } else {
      return "";
    }
  }

  HeaderText getHeaderText() {
    HeaderText headerText;
    if ( !data.HasMember("headerText") || !data["headerText"].IsObject() ) {
      return headerText;
    }
    headerText.subText = 
      ( data["headerText"].HasMember("subText") && data["headerText"]["subText"].IsString() )
      ? data["headerText"]["subText"].GetString()
      : ""
    ;
    headerText.subTextRed = 
      ( data["headerText"].HasMember("subTextRed") && data["headerText"]["subTextRed"].IsString() )
      ? data["headerText"]["subTextRed"].GetString()
      : ""
    ;
    headerText.maxSize = 0;
    if ( !data["headerText"].HasMember("rows") || !data["headerText"]["rows"].IsArray() ) return headerText;
    for ( unsigned int blockNumber = 0; blockNumber < data["headerText"]["rows"].Size(); ++blockNumber ) {
      if ( blockNumber == 2 ) break;
      vector<HeaderTextEntry> headerBlock;
      headerBlock.resize(data["headerText"]["rows"][blockNumber].Size());
      
      for (unsigned int row = 0; row < headerBlock.size(); ++row) {
        if ( static_cast<int>(row) == headerText.maxSize ) headerText.maxSize++;
        headerBlock[row].text = 
          ( 
            data["headerText"]["rows"][blockNumber][row].HasMember("text")
            && data["headerText"]["rows"][blockNumber][row]["text"].IsString()
          )
          ? data["headerText"]["rows"][blockNumber][row]["text"].GetString()
          : ""
        ;
        headerBlock[row].fontStyle = getFontStyle(
          ( 
            data["headerText"]["rows"][blockNumber][row].HasMember("style") 
            && data["headerText"]["rows"][blockNumber][row]["style"].IsString() 
          )
          ? data["headerText"]["rows"][blockNumber][row]["style"].GetString()
          : ""
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
      sections[i].title = 
        ( data["sections"][i].HasMember("title") && data["sections"][i]["title"].IsString() )
        ? data["sections"][i]["title"].GetString()
        : ""
      ;
      sections[i].width = 0;
      sections[i].columns.resize(data["sections"][i]["columns"].Size());
      sections[i].rows.resize(
        data["sections"][i]["rows"].Size(),
        vector<string>(sections[i].columns.size())
      );

      // loop through columns
      for (unsigned int countCol = 0; countCol < sections[i].columns.size(); ++countCol) {
        sections[i].columns[countCol].text = 
          (
             data["sections"][i]["columns"][countCol].HasMember("text")
             && data["sections"][i]["columns"][countCol]["text"].IsString()
          )
          ? data["sections"][i]["columns"][countCol]["text"].GetString()
          : ""
        ;
        sections[i].columns[countCol].visible = 
          (
             data["sections"][i]["columns"][countCol].HasMember("visible")
             && data["sections"][i]["columns"][countCol]["visible"].IsBool()
          )
          ? data["sections"][i]["columns"][countCol]["visible"].GetBool()
          : true
        ;
        sections[i].columns[countCol].width = 
          (
            data["sections"][i]["columns"][countCol].HasMember("width")
            && data["sections"][i]["columns"][countCol]["width"].IsNumber()
            && sections[i].columns[countCol].visible
          )
          ? data["sections"][i]["columns"][countCol]["width"].GetDouble()
          : 0
        ;
        sections[i].width += sections[i].columns[countCol].width;
        sections[i].columns[countCol].fontStyle = getFontStyle(
          (
            data["sections"][i]["columns"][countCol].HasMember("style")
            && data["sections"][i]["columns"][countCol]["style"].IsString()
          )
          ? data["sections"][i]["columns"][countCol]["style"].GetString()
          : ""
        );
        sections[i].columns[countCol].align = getTextAlign(
          (
            data["sections"][i]["columns"][countCol].HasMember("align")
            && data["sections"][i]["columns"][countCol]["align"].IsString()
          )
          ? data["sections"][i]["columns"][countCol]["align"].GetString()
          : ""
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
    totals.title = 
      ( data["totals"].HasMember("title") && data["totals"]["title"].IsString() )
      ? data["totals"]["title"].GetString()
      : ""
    ;
    totals.costs.width = 0;
    totals.finalText = 
      ( data["totals"].HasMember("finalText") && data["totals"]["finalText"].IsString() )
      ? data["totals"]["finalText"].GetString()
      : ""
    ;
    totals.finalValue = 
      ( data["totals"].HasMember("finalValue") && data["totals"]["finalValue"].IsString() )
      ? data["totals"]["finalValue"].GetString()
      : ""
    ;
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
      totals.reductions.title = 
        ( data["totals"].HasMember("discountsTitle") && data["totals"]["discountsTitle"].IsString() )
        ? data["totals"]["discountsTitle"].GetString()
        : ""
      ;
    } else {
      totals.reductions.rows.resize( 0 );
    }
    // loop through columns
    for (unsigned int countCol = 0; countCol < totals.costs.columns.size(); ++countCol) {
      totals.costs.columns[countCol].text = 
        ( 
          data["totals"]["columns"][countCol].HasMember("text")
          && data["totals"]["columns"][countCol]["text"].IsString()
        )
        ? data["totals"]["columns"][countCol]["text"].GetString()
        : ""
      ;
      totals.costs.columns[countCol].visible = 
        ( 
          data["totals"]["columns"][countCol].HasMember("visible")
          && data["totals"]["columns"][countCol]["visible"].IsBool()
        )
        ? data["totals"]["columns"][countCol]["visible"].GetBool()
        : true
      ;
      totals.costs.columns[countCol].width = 
        (
          data["totals"]["columns"][countCol].HasMember("width")
          && data["totals"]["columns"][countCol]["width"].IsNumber()
          && totals.costs.columns[countCol].visible
        )
        ? data["totals"]["columns"][countCol]["width"].GetDouble()
        : 0
      ;
      totals.costs.width += totals.costs.columns[countCol].width;
      totals.costs.columns[countCol].fontStyle = getFontStyle(
        (
          data["totals"]["columns"][countCol].HasMember("style")
          && data["totals"]["columns"][countCol]["style"].IsString()
        )
        ? data["totals"]["columns"][countCol]["style"].GetString()
        : ""
      );
      totals.costs.columns[countCol].align = getTextAlign(
        (
          data["totals"]["columns"][countCol].HasMember("align")
          && data["totals"]["columns"][countCol]["align"].IsString()
        )
        ? data["totals"]["columns"][countCol]["align"].GetString()
        : ""
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
      ( data["extras"].HasMember("title") && data["extras"]["title"].IsString() )
      ? data["extras"]["title"].GetString()
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

  string getFooterText() {
    if ( data.HasMember("footerText") && data["footerText"].IsString() ) {
      return data["footerText"].GetString();
    } else return "";
  }

  string getAoLogo() {
    if ( data.HasMember("aoLogo") && data["aoLogo"].IsString() ) {
      return data["aoLogo"].GetString();
    } else return "";
  }

  // string getMetadataCommand() {
  //   // echo "-json" | exiftool -@ - test.pdf
  //   if ( getExifJsonName() == "" || getFileName() == "" ) return "";
  //   return string("exiftool -overwrite_original -fast3 ")
  //   + "-json=" + getExifJsonName() + " "
  //   + getFileName() 
  //   + " >/dev/null 2>&1";
  // }

  string getFileName() {
    if ( data.HasMember("fileName") && data["fileName"].IsString() ) {
      return string("output/") + data["fileName"].GetString() + ".pdf";
    } else  return "";
  }

  string getExifJsonName() {
    if ( data.HasMember("fileName") && data["fileName"].IsString() ) {
    return string("/tmp/") + data["fileName"].GetString() + ".json";
    } else  return "";
  }

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
};