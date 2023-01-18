#ifndef __MYOS__GUI__WIDGET_H
#define __MYOS__GUI__WIDGET_H

#include <common/types.h>
#include <common/graphicscontext.h>

namespace myos{
	namespace gui{
		class Widget{
		protected:
			Widget* parent;
			myos::common::int32_t x;
			myos::common::int32_t y;
			myos::common::int32_t w;
			myos::common::int32_t h;

			myos::common::int8_t r;
			myos::common::int8_t g;
			myos::common::int8_t b;
			bool Focusable;
		public:
			Widget(Widget* parent, myos::common::int32_t x, myos::common::int32_t y, myos::common::int32_t w, myos::common::int32_t h, myos::common::int8_t r, myos::common::int8_t g, myos::common::int8_t b);
			~Widget();
			virtual void GetFocus(Widget* widget);
			virtual void ModelToScreen(myos::common::int32_t &x,myos::common::int32_t &y);
			virtual void Draw(myos::common::GraphicsContext* gc);
			virtual void OnMouseDown(myos::common::int32_t x, myos::common::int32_t y);
			virtual void OnMouseUp(myos::common::int32_t x, myos::common::int32_t y);
			virtual void OnMouseMove(myos::common::int32_t oldx, myos::common::int32_t oldy,myos::common::int32_t newx, myos::common::int32_t newy);

			virtual void OnKeyDown(myos::common::int32_t x, myos::common::int32_t y);
			virtual void OnKeyUp(myos::common::int32_t x, myos::common::int32_t y);
		};
	}
}

#endif // !__MYOS__GUI__WIDGET_H
