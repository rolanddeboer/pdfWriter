#ifndef DATAREADER_H
#define DATAREADER_H

#include <string>
#include <vector>
#include "pdfTypes.h"

using namespace std;

struct HeaderTextEntry {
  FontStyle fontStyle;
  string text;
};
struct HeaderText {
  vector<HeaderTextEntry> left, right;
  int maxSize;
  string subText;
  string subTextRed;
};

struct Table {
  struct Column {
    FontStyle fontStyle;
    string text;
    double width;
    Align align;
    bool visible;
    bool light;
  };
  string title;
  double width;
  vector<Column> columns;
  vector<vector<string>> rows;
};

struct Totals {
  Table costs, reductions;
  string finalText, finalValue, title;
};

struct Extras {
  string title;
  vector<string> lines;
};

#endif