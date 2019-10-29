// alpha_transform.cc

#include "alpha.h"

namespace alpha {
   transform::transform()
      : scale_(1.0f, 1.0f)
      , rotation_(0.0f)
   {
   }

   void transform::set_origin(const vector2 &origin) {
      origin_ = origin;
   }

   void transform::set_position(const vector2 &position) {
      position_ = position;
   }

   void transform::set_scale(const vector2 &scale) {
      scale_ = scale;
   }

   void transform::set_rotation(const float rotation) {
      rotation_ = rotation;
   }
} // !alpha
