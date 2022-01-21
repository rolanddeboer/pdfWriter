#ifndef PDFTYPES_H
#define PDFTYPES_H

enum FontStyle {
  FONT_TITLE,
  FONT_HEADING,
  FONT_REGULAR,
  FONT_BOLD,
  FONT_ITALIC
};
enum Align {
  ALIGN_LEFT,
  ALIGN_RIGHT,
  ALIGN_CENTER,
  ALIGN_NONE
};
struct Margin {
  double top, right, bottom, left;
};
struct Page {
  Margin margin;
  double height, width;
  double xpos, ypos, savedYpos;
};
struct Font {
  FontStyle style;
  int size;
  double color;
};

#endif