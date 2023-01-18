#include <gui/widget.h>
using namespace myos::common;
using namespace myos::gui;

Widget::Widget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h,
               int8_t r, int8_t g, int8_t b) {
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->r = r;
    this->g = g;
    this->b = b;
	this->Focusable = true;
}
Widget::~Widget() {}

void Widget::GetFocus(Widget* widget) {
	if(parent!=0){
		parent->GetFocus(widget);
	}
}
void Widget::ModelToScreen(int32_t& x, int32_t& y) {
	if(parent!=0){
		parent->ModelToScreen(x, x);
	}
	x += this->x;
	y += this->y;
}
void Widget::Draw(GraphicsContext* gc) {
	int X =0;
	int Y =0;
	ModelToScreen(X,Y);
	gc->FillRectangle(X,Y,w,h,r,g,b);
}
void Widget::OnMouseDown(int32_t x, int32_t y) {
	if(Focusable){
		GetFocus(this);
	}
}
void Widget::OnMouseUp(int32_t x, int32_t y) {}
void Widget::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx,
                         int32_t newy) {}

void Widget::OnKeyDown(int32_t x, int32_t y) {}
void Widget::OnKeyUp(int32_t x, int32_t y) {}
