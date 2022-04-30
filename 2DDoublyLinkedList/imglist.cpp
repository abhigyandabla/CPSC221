// File:        imglist.cpp
// Date:        2022-01-27 10:21
// Description: Contains partial implementation of ImgList class
//              for CPSC 221 2021W2 PA1
//              Function bodies to be completed by yourselves
//
// ADD YOUR FUNCTION IMPLEMENTATIONS IN THIS FILE
//
#include "imglist.h"
#include <iostream>
#include <math.h> // provides fmin and fabs functions
#include <cmath> 
using namespace std;
/**************************
* MISCELLANEOUS FUNCTIONS *
**************************/

/*
* This function is NOT part of the ImgList class,
* but will be useful for one of the ImgList functions.
* Returns the "difference" between two hue values.
* PRE: hue1 is a double between [0,360).
* PRE: hue2 is a double between [0,360).
* 
* The hue difference is the absolute difference between two hues,
* but takes into account differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
double HueDiff(double hue1, double hue2) {
  return fmin(fabs(hue1 - hue2), fabs(360 + fmin(hue1, hue2) - fmax(hue1, hue2)));
}
/*********************
* CONSTRUCTORS, ETC. *
*********************/

/*
* Default constructor. Makes an empty list
*/
ImgList::ImgList() {
  // set appropriate values for all member attributes here
  northwest = nullptr;
  southeast = nullptr;
}

/*
* Creates a list from image data
* PRE: img has dimensions of at least 1x1
*/


ImgList::ImgList(PNG& img) {
  // build the linked node structure and set the member attributes appropriately
  ImgNode* top0;
  ImgNode* last;
  ImgNode* curr0;
  ImgNode* curr;
  for (unsigned y = 0; y < img.height(); y++) {
    for (unsigned x = 0; x < img.width(); x++) {
      HSLAPixel *pixel = img.getPixel(x, y);
       if(x == 0 && y== 0){
            northwest = new ImgNode();
            northwest->colour.h =  pixel->h;
            northwest->colour.s =  pixel->s;
            northwest->colour.l =  pixel->l;
            northwest->colour.a =  pixel->a;
            last = northwest;
            top0 = northwest;      
          }
        else if(x==0){
            curr = new ImgNode();
            curr->colour.h =  pixel->h;
            curr->colour.s =  pixel->s;
            curr->colour.l =  pixel->l;
            curr->colour.a =  pixel->a;
            last = curr;
            curr0 = curr;
        }
          
          else{
            curr = new ImgNode();
            curr->colour.h =  pixel->h;
            curr->colour.s =  pixel->s;
            curr->colour.l =  pixel->l;
            curr->colour.a =  pixel->a;
            last->east = curr;
            curr->west = last;
            last = curr;
            if(x == img.width()-1 && y != 0){
              ImgNode* n = curr0;
              while(curr0 != nullptr && top0 != nullptr){
                curr0->north = top0;
                top0->south = curr0;
                top0 = top0->east;
                curr0 = curr0->east;
              }
              top0 = n;
            }
          }
    }
    }
    southeast = last;
}
/*
* Copy constructor.
* Creates this this to become a separate copy of the data in otherlist
*/
ImgList::ImgList(const ImgList& otherlist) {
  // build the linked node structure using otherlist as a template
  Copy(otherlist);
}

/*
* Assignment operator. Enables statements such as list1 = list2;
*   where list1 and list2 are both variables of ImgList type.
* POST: the contents of this list will be a physically separate copy of rhs
*/
ImgList& ImgList::operator=(const ImgList& rhs) {
  // Re-build any existing structure using rhs as a template
  
  if (this != &rhs) { // if this list and rhs are different lists in memory
    // release all existing heap memory of this list
    Clear();    
    
    // and then rebuild this list using rhs as a template
    Copy(rhs);
  }
  
  return *this;
}

/*
* Destructor.
* Releases any heap memory associated with this list.
*/
ImgList::~ImgList() {
  // Ensure that any existing heap memory is deallocated
  Clear();
}

/************
* ACCESSORS *
************/

/*
* Returns the horizontal dimension of this list (counted in nodes)
* Note that every row will contain the same number of nodes, whether or not
*   the list has been carved.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionX() const {
  // replace the following line with your implementation
  ImgNode* counter = northwest;
  int count = 0;
  while (counter != nullptr){
    counter = counter->east;
    count +=1;
  }
  return count;
}

/*
* Returns the vertical dimension of the list (counted in nodes)
* It is useful to know/assume that the grid will never have nodes removed
*   from the first or last columns. The returned value will thus correspond
*   to the height of the PNG image from which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   y dimension.
*/
unsigned int ImgList::GetDimensionY() const {
  ImgNode* counter = northwest;
  int count = 0;
  while (counter != nullptr){
    int t  =  1 + counter->skipdown;
    counter = counter->south;
    count += t ;
  }
  return count;
}

/*
* Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
* The returned value will thus correspond to the width of the PNG image from
*   which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionFullX() const {
  // replace the following line with your implementation
  ImgNode* counter = northwest;
  int count = 0;
  while (counter != nullptr){
    int t  =  1 + counter->skipright;
    counter = counter->east;
    count += t;
  }
  return count;
}


/*
* Returns a pointer to the node which best satisfies the specified selection criteria.
* The first and last nodes in the row cannot be returned.
* PRE: rowstart points to a row with at least 3 physical nodes
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rowstart - pointer to the first node in a row
* PARAM: selectionmode - criterion used for choosing the node to return
*          0: minimum luminance across row, not including extreme left or right nodes
*          1: node with minimum total of "hue difference" with its left neighbour and with its right neighbour.
*        In the (likely) case of multiple candidates that best match the criterion,
*        the left-most node satisfying the criterion (excluding the row's starting node)
*        will be returned.
* A note about "hue difference": For PA1, consider the hue value to be a double in the range [0, 360).
* That is, a hue value of exactly 360 should be converted to 0.
* The hue difference is the absolute difference between two hues,
* but be careful about differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
ImgNode* ImgList::SelectNode(ImgNode* rowstart, int selectionmode) {
  // add your implementation below
  ImgNode* curr = rowstart;
  double min = 100000000.0;
  ImgNode* minNode = nullptr;
  while (curr != nullptr){
    if(curr->east == nullptr || curr->west == nullptr){
      curr = curr->east;
    }
    else{
      if (selectionmode == 0){
        if (curr->colour.l < min){
          min = curr->colour.l;
          minNode = curr;
        }
        curr = curr->east;

      }
      else{
        double diff = HueDiff(curr->east->colour.h, curr->colour.h) + HueDiff(curr->west->colour.h, curr->colour.h);
        if(diff < min){
          min = diff;
          minNode = curr;
        }
         curr = curr->east;

      }

    }
  }
  return minNode;
}

// double ImgList::averageHue(double hueone, double huetwo) const {
//   double total = hueone + huetwo;
//   if (total >= 360.0) {
//     while (total >= 360.0) {
//       total = total - 360.0;
//     }
//   }
//   return (total / 2.0);
// }
double ImgList::averageHue(double hueone, double huetwo) const{
  double total = hueone + huetwo;
  if (180 == HueDiff(hueone, huetwo)){
      if (total >= 360.0) {
    while (total >= 360.0) {
      total = total - 360.0;
    }
  }
  return (total / 2.0);
  }else{
      double used;
      if(fabs(hueone - huetwo) < fabs(360 + fmin(hueone, huetwo) - fmax(hueone, huetwo))){
          cout<<"ran"<<endl;
          if(hueone > huetwo){
              used = huetwo;
          }
          else{
              used =  hueone;
          }
          double ans = used + HueDiff(hueone,huetwo)/2.0;
          if(ans  >= 360 ){
              ans -= 360;
          }
          return ans;
      }else{
          if(hueone < huetwo){
              used = huetwo;
          }
          else{
              used =  hueone;
          }
          double ans = huetwo + fabs(360 + fmin(hueone, huetwo) - fmax(hueone, huetwo))/2.0;
          if(ans  >= 360 ){
              ans -= 360;
          }
          return ans;
      }
}
}

/*
* Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
* PRE: fillmode is an integer in the range of [0,2]
* PARAM: fillgaps - whether or not to fill gaps caused by carving
*          false: render one pixel per node, ignores fillmode
*          true: render the full width of the original image,
*                filling in missing nodes using fillmode
* PARAM: fillmode - specifies how to fill gaps
*          0: solid, uses the same colour as the node at the left of the gap
*          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
*             Note that "average" for hue will use the closer of the angular distances,
*             e.g. average  10 and 350 will be 0, instead of 180.
*             Average of diametric hue values will use the smaller of the two averages
*             e.g. average of 30 and 210 will be 120, and not 300
*                  average of 170 and 350 will be 80, and not 260
*          2: *** OPTIONAL - FOR BONUS ***
*             linear gradient between the colour (all channels) of the nodes at the left and right of the gap
*             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
*             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
*             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
*             Like fillmode 1, use the smaller difference interval for hue,
*             and the smaller-valued average for diametric hues
*/
PNG ImgList::Render(bool fillgaps, int fillmode) const {
  PNG* p;
  unsigned int x = 0;
  unsigned int y = 0;
  if (fillgaps) {
    p = new PNG(GetDimensionFullX(), GetDimensionY());
    if (fillmode == 0) {
      ImgNode* w = northwest;
      ImgNode* h = northwest;
      while(h != nullptr) {
        while(w != nullptr) {
          HSLAPixel* pixel =  p->getPixel(x, y);
          pixel->h = w->colour.h;
          pixel->s = w->colour.s;
          pixel->l = w->colour.l;
          pixel->a = w->colour.a;
          if (w->skipright != 0) {
            for (unsigned int i = 0; i < w->skipright; i++) {
              x++;
              HSLAPixel* pixelgap =  p->getPixel(x, y);
              pixelgap->h = w->colour.h;
              pixelgap->s = w->colour.s;
              pixelgap->l = w->colour.l;
              pixelgap->a = w->colour.a;
            }
          }
          x++;
          w = w->east;
        }
        h = h->south;
        w = h;
        x = 0;
        y++;
      }
    } else {
      ImgNode* w = northwest;
      ImgNode* h = northwest;
      while(h != nullptr) {
        while(w != nullptr) {
          HSLAPixel* pixel =  p->getPixel(x, y);
          pixel->h = w->colour.h;
          pixel->s = w->colour.s;
          pixel->l = w->colour.l;
          pixel->a = w->colour.a;
          if (w->skipright != 0) {
            for (unsigned int i = 0; i < w->skipright; i++) {
              x++;
              HSLAPixel* pixelgap =  p->getPixel(x, y);
              pixelgap->h = averageHue(w->colour.h, w->east->colour.h);
              pixelgap->s = (double)(w->colour.s + w->east->colour.s)/2.0;
              pixelgap->l = (double)(w->colour.l + w->east->colour.l)/2.0;
              pixelgap->a = (double)(w->colour.a + w->east->colour.a)/2.0;
            }
          }
          x++;
          w = w->east;
        }
        h = h->south;
        w = h;
        x = 0;
        y++;
      }
    } 
  } else {
    ImgNode * w = northwest;
    ImgNode * h = northwest;
    p = new PNG(GetDimensionX(), GetDimensionY());
    while(h != nullptr) {
      while(w != nullptr) {
        HSLAPixel* pixel =  p->getPixel(x, y);
        pixel->h = w->colour.h;
        pixel->s = w->colour.s;
        pixel->l = w->colour.l;
        pixel->a = w->colour.a;
        x++;
        w = w->east;
      }
      h = h->south;
      w = h;
      x = 0;
      y++;
    }
  // }
  }
  PNG outpng = *p;
  delete p;
  p = nullptr;
  return outpng;
}



/************
* MODIFIERS *
************/

/*
* Removes exactly one node from each row in this list, according to specified criteria.
* The first and last nodes in any row cannot be carved.
* PRE: this list has at least 3 nodes in each row
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: selectionmode - see the documentation for the SelectNode function.
* POST: this list has had one node removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(int selectionmode) {
ImgNode* y = northwest;
while(y != nullptr){
  ImgNode* s =  SelectNode(y, selectionmode);

  if(s->north == nullptr && s->south != nullptr){
    s->south->skipup += 1;
    s->west->skipright += 1;
    s->east->skipleft += 1;
    s->west->east = s->east;
    s->east->west = s->west;
    s->south->north = nullptr;
  } else if(s->south == nullptr && s->north != nullptr){
    s->north->skipdown += 1;
    s->west->skipright += 1;
    s->east->skipleft += 1;
    s->north->south = nullptr;
    s->west->east = s->east;
    s->east->west = s->west;
  } else if (s->south == nullptr && s->north == nullptr){
    s->west->skipright += 1;
    s->east->skipleft += 1;
    s->west->east = s->east;
    s->east->west = s->west;
  }
    else{
    s->north->skipdown += 1;
    s->west->skipright += 1;
    s->east->skipleft += 1;
    s->south->skipup += 1;
    s->west->east = s->east;
    s->east->west = s->west;
    s->north->south = s->south;
    s->south->north = s->north;
  }
  delete s;
  s = nullptr;
  y = y->south;
}
}

// note that a node on the boundary will never be selected for removal
/*
* Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
* based on specific selection criteria.
* Note that this should remove one node from every row, repeated "rounds" times,
* and NOT remove "rounds" nodes from one row before processing the next row.
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rounds - number of nodes to remove from each row
*        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
*        i.e. Ensure that the final list has at least two nodes in each row.
* POST: this list has had "rounds" nodes removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(unsigned int rounds, int selectionmode) {
  if (rounds > GetDimensionX()-2 ){
    rounds = GetDimensionX()-2;
  }
  for (unsigned int i=0; i<rounds; i++){
    Carve(selectionmode);
  }
  // add your implementation here

}


/*
* Helper function deallocates all heap memory associated with this list,
* puts this list into an "empty" state. Don't forget to set your member attributes!
* POST: this list has no currently allocated nor leaking heap memory,
*       member attributes have values consistent with an empty list.
*/
void ImgList::Clear() {
ImgNode* y = northwest;
ImgNode* x = northwest;
ImgNode* last = northwest;
while(y != nullptr){
  y = y-> south;
  while(x != nullptr){
    x = x->east;
    delete last;
    last = x;
  }
  x = y;
  last = x;
}
northwest = nullptr;
southeast = nullptr;
last = nullptr;
}

/* ************************
*  * OPTIONAL - FOR BONUS *
** ************************
* Helper function copies the contents of otherlist and sets this list's attributes appropriately
* PRE: this list is empty
* PARAM: otherlist - list whose contents will be copied
* POST: this list has contents copied from by physically separate from otherlist
*/
void ImgList::Copy(const ImgList& otherlist) {
  // add your implementation here
  
}

/*************************************************************************************************
* IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
*************************************************************************************************/