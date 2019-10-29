// alpha_rectangle.cc

#include "alpha.h"

namespace alpha {
   rectangle::rectangle()
      : x_(0.0f)
      , y_(0.0f)
      , width_(0.0f)
      , height_(0.0f)
   {
   }

   rectangle::rectangle(float x, float y, float width, float height)
      : x_(x)
      , y_(y)
      , width_(width)
      , height_(height)
   {
   }

   void rectangle::set_size(const vector2 size) {
      width_ = size.x_;
      height_ = size.y_;
   }

   void rectangle::set_position(const vector2 pos) {
      x_ = pos.x_;
      y_ = pos.y_;
   }
} // !alpha
