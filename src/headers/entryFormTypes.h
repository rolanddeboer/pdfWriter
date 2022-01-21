#ifndef ENTRYFORMTYPES_H
#define ENTRYFORMTYPES_H

  struct LogoHeight {
    double show, ao;
  };
  struct EntryFormFont {
    struct FontSize {
      int text, title, heading, topBar, sectionTitle, tableHead, tableBody, totalsFooter, extras, footer;
    } size;
    struct FontColor {
      int text, title, heading, footer, headingSubText, red;
    } color;
    struct FontLineHeight {
      int topBar, tableBody, extras;
    } lineHeight;
  };
  struct Line {
    struct LineThickness {
      double header, tableHead, tableBody;
    } thickness;
    struct LineColor {
      long int header, tableHead, tableBody;
    } color;
  };

#endif