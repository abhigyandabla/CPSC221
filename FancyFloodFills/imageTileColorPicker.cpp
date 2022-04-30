/*
*  File: imageTileColorPicker.cpp
*  Implements the image tile color picker for CPSC 221 PA2.
*
*/

#include "imageTileColorPicker.h"

ImageTileColorPicker::ImageTileColorPicker(PNG& otherimage) {
  img_other = otherimage;  
}

HSLAPixel ImageTileColorPicker::operator()(PixelPoint p) {
  int w = img_other.width();
  int h = img_other.height();
  int x = p.x % w;
  int y = p.y % h;
  return (*img_other.getPixel(x, y));  
}