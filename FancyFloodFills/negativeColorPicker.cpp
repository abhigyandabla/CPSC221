/*
*  File: negativeColorPicker.cpp
*  Implements the negative color picker for CPSC 221 PA2.
*
*/

#include "negativeColorPicker.h"

NegativeColorPicker::NegativeColorPicker(PNG& inputimg)
{ 
  img = inputimg;
  
}

HSLAPixel NegativeColorPicker::operator()(PixelPoint p)
 { 
  HSLAPixel * img_pixel = img.getPixel(p.x, p.y);
  double h = (img_pixel->h >= 180) ? img_pixel->h - 180 : img_pixel->h + 180;
  double l = 1 - img_pixel->l;
  return HSLAPixel(h, img_pixel->s, l, img_pixel->a);
}
