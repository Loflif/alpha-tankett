// alpha_mouse.cc

#include "alpha.h"

namespace alpha {
   mouse::mouse()
      : x_(0)
      , y_(0)
      , delta_(0)
      , buttons_{}
   {
   }

   bool mouse::is_down(mouse_button index) const {
      return buttons_[index].down_;
   }

   bool mouse::is_pressed(mouse_button index) const {
      return buttons_[index].pressed_;
   }

   bool mouse::is_released(mouse_button index) const {
      return buttons_[index].released_;
   }
} // !alpha
