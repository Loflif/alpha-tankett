// alpha_sprite.cc

#include "alpha.h"

namespace alpha {
   sprite::sprite()
      : image_(nullptr)
      , color_(0xffffffff)
   {
   }

   sprite::sprite(const uint32 color)
      : image_(nullptr)
      , color_(color)
   {
   }

   sprite::sprite(texture &image, vector2 size)
      : image_(&image)
      , color_(0xffffffff)
   {
	   set_size(size);

	   rectangle source(0, 0, (float)image.width_, (float)image.height_);
	   set_source(source);
   }

   void sprite::set_color(const uint32 color) {
      color_ = color;
   }

   void sprite::set_texture(const texture &image) {
      image_ = &image;
   }

   void sprite::set_source(const rectangle &rect) {
      float u0 = 0.0f;
      float v0 = 0.0f;
      float u1 = 0.0f;
      float v1 = 0.0f;

      if (image_) {
         const float iw = 1.0f / image_->width_;
         const float ih = 1.0f / image_->height_;
         u0 = rect.x_ * iw;
         v0 = rect.y_ * ih;
         u1 = rect.width_ * iw;
         v1 = rect.height_ * ih;
      }

      source_ = { u0, v0, u1, v1 };
   }

   void sprite::set_size(const vector2 &size) {
      size_ = size;
   }
} // !alpha
