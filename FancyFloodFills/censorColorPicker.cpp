/*
*  File: censorColorPicker.cpp
*  Implements the censor color picker for CPSC 221 PA2.
*
*/

#include "censorColorPicker.h"

#include <math.h> // gives access to sqrt function

/*
*  Useful function for computing the Euclidean distance between two PixelPoints
*/
double PointDistance(PixelPoint a, PixelPoint b) {
  unsigned int diff_x, diff_y;
  if (a.x > b.x)
    diff_x = a.x - b.x;
  else
    diff_x = b.x - a.x;
  if (a.y > b.y)
    diff_y = a.y - b.y;
  else
    diff_y = b.y - a.y;
  return sqrt(diff_x * diff_x + diff_y * diff_y);
}

CensorColorPicker::CensorColorPicker(unsigned int b_width, PixelPoint ctr, unsigned int rad, PNG& inputimage)
{
  blockwidth = b_width;
  center = ctr;
  radius = rad;
  img = inputimage;
  blockyimg = PNG(img.height(), img.width());
  for (unsigned x = 0; x < blockyimg.width(); x += blockwidth) {
    for (unsigned y = 0; y < blockyimg.height(); y += blockwidth) {
        unsigned int w = 0;
        unsigned int h = 0;
        int count = blockwidth * blockwidth;
        double hu = 0;
        double lu = 0;
        double sa = 0;
        double al = 0;
        while(w < blockwidth && x < img.width()){
          while(h < blockwidth && y < img.height()){
             HSLAPixel *pixel = img.getPixel(x+w, y+h);
             hu += pixel->h;
             lu += pixel->l;
             sa += pixel->s;
             al += pixel->a;
             h++;
          }
          w++;
          h = 0;
    }
    hu /= count;
    lu /= count;
    sa /= count;
    al /= count;
    w = 0;
    h = 0;
    while(w < blockwidth && x < img.width()){
          while(h < blockwidth && y < img.height()){
             HSLAPixel *p = blockyimg.getPixel(x+w, y+h);
             p->h = hu;
             p->l = lu;
             p->s = sa;
             p->a = al;
             h++;
          }
          w++;
          h = 0;
    }
    }
  }
}

HSLAPixel CensorColorPicker::operator()(PixelPoint p)
{ 
  if(PointDistance(p, center) <= radius){
    return *(blockyimg.getPixel(p.x, p.y));
  }
  return *(img.getPixel(p.x, p.y));

  
}
