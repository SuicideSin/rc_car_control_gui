//Glut User Interface Source
//	Created By:		Mike Moss
//	Modified On:	12/02/2013

//Required Libraries:
//	ftgl
//	freetype
//	gl
//	glew
//	glu
//	glut/freeglut

//Definitions for "glut_ui.hpp"
#include "glut_ui.hpp"

//Glut Input Header
#include "glut_input.hpp"

#include "string_util.hpp"

#include "time_util.hpp"

//OpenGL Headers
#ifndef __APPLE__
	#include <GL/glew.h>
	#include <GL/glut.h>
#else
	#include <GLEW/glew.h>
	#include <GLUT/glut.h>
#endif

msl::widget::widget(const double x,const double y,const double width,const double height,
	const bool hover,const bool down,const bool pressed,const bool disabled,
	const bool visible,const msl::color& background_color_from,const msl::color& background_color_to,
	const msl::color& outline_color,const msl::color& outline_color_hover,
	const msl::color& outline_color_disabled,const msl::color& text_color,const msl::color& text_color_disabled):
		x(x),y(y),width(width),height(height),display_width(width),display_height(height),hover(hover),down(down),
		pressed(pressed),disabled(disabled),visible(visible),background_color_from(background_color_from),
		background_color_to(background_color_to),outline_color(outline_color),
		outline_color_hover(outline_color_hover),outline_color_disabled(outline_color_disabled),
		text_color(text_color),text_color_disabled(text_color_disabled)
{}

msl::button::button(const std::string& value,const double x,const double y):widget(x,y),value(value),padding(4)
{}

void msl::button::loop(const double dt)
{
	display_width=width;

	if(width<0)
		display_width=msl::text_width(value)+padding*2;

	display_height=height;

	if(height<0)
		display_height=padding*2+msl::text_height("test");

	if(visible)
	{
		if(!disabled)
		{
			bool new_hover=(msl::mouse_x>=x-display_width/2.0&&msl::mouse_x<=x+display_width/2.0&&
				msl::mouse_y>=y-display_height/2.0&&msl::mouse_y<=y+display_height/2.0);

			if(hover&!new_hover)
				glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

			if(!hover&new_hover)
				glutSetCursor(GLUT_CURSOR_INFO);

			hover=new_hover;

			down=hover&&msl::input_check(mb_left);
			pressed=hover&&msl::input_check_released(mb_left);
		}
		else
		{
			hover=false;
			down=false;
			pressed=false;
		}
	}
}

void msl::button::draw()
{
	if(visible)
	{
		msl::color but_col_to=background_color_to;
		msl::color but_col_from=background_color_from;
		msl::color out_col=outline_color;
		msl::color tex_col=text_color;

		if(hover)
			out_col=outline_color_hover;

		if(pressed||down)
			std::swap(but_col_from,but_col_to);

		if(disabled)
		{
			but_col_from=but_col_to;
			out_col=outline_color_disabled;
			tex_col=text_color_disabled;
		}

		double text_width=msl::text_width(value);
		double text_height=msl::text_height(value);

		msl::draw_rectangle_gradient(x,y,display_width,display_height,true,but_col_from,but_col_from,but_col_to,but_col_to);
		msl::draw_rectangle(x,y,display_width,display_height,false,out_col);
		msl::draw_text(x-text_width/2.0,y-text_height/3.0,value,tex_col);
	}
}

msl::checkbox::checkbox(const bool value,const double x,const double y):widget(x,y,12,12),value(value),button_("",x,y)
{}

void msl::checkbox::loop(const double dt)
{
	update_button(dt);

	if(button_.pressed&&!disabled)
		value=!value;
}

void msl::checkbox::draw()
{
	if(visible)
	{
		button_.x=x;
		button_.y=y;

		button_.draw();

		if(value)
		{
			msl::color col=text_color;

			if(disabled)
				col=text_color_disabled;

			msl::draw_circle(x-3,y+0,1.5,col);
			msl::draw_circle(x-2,y-1,1.5,col);

			for(int ii=-1;ii<=3;++ii)
				msl::draw_circle(x+ii,y+ii-1,1.5,col);
		}
	}
}

void msl::checkbox::update_button(const double dt)
{
	hover=button_.hover;
	down=button_.down;
	pressed=button_.pressed;
	button_.disabled=disabled;
	button_.visible=visible;
	button_.width=width;
	button_.height=height;
	button_.display_width=width;
	button_.display_height=height;
	display_width=width;
	display_height=height;
	button_.background_color_from=background_color_from;
	button_.background_color_to=background_color_to;
	button_.outline_color=outline_color;
	button_.outline_color_hover=outline_color_hover;
	button_.outline_color_disabled=outline_color_disabled;
	button_.text_color=text_color;
	button_.text_color_disabled=text_color_disabled;

	button_.loop(dt);
}

msl::dropdown::dropdown(const double x,const double y):widget(x,y,-1,-1),value(-1),padding(4),
	highlighted_background_color(0.2,0.3,1,1),highlighted_text_color(1,1,1,1),button_("",x,y),
	selected(false)
{}
#include <iostream>
void msl::dropdown::loop(const double dt)
{
	//Figure Out Max Width
	double max_width=0;

	for(unsigned int ii=0;ii<options.size();++ii)
		if(msl::text_width(options[ii])>max_width)
			max_width=msl::text_width(options[ii]);

	//Set Width (With padding between text and triangle)
	if(width<0)
		button_.width=max_width+padding*5;
	else
		button_.width=width;

	//Update if Visible
	if(visible)
	{
		//Update Button
		update_button(dt);

		//If Pressed, Toggle Selected
		if(pressed)
			selected=!selected;

		//If Selected, Button Stays Down
		if(selected)
			//Button is Down
			button_.down=true;

		//Check If Click Outside
		if(!hover&&(msl::input_check_released(mb_left)||msl::input_check_released(mb_middle)||msl::input_check_released(mb_right)))
			selected=false;
	}
}

void msl::dropdown::draw()
{
	//To draw or not to draw...
	if(visible)
	{
		//Figure Out Colors
		msl::color out_col=outline_color;
		msl::color tex_col=text_color;

		if(hover)
			out_col=outline_color_hover;

		if(disabled)
		{
			out_col=outline_color_disabled;
			tex_col=text_color_disabled;
		}

		//Draw Button
		button_.draw();

		//Draw Triangle
		double triangle_draw_x=x+display_width/2.0-padding*2;
		msl::draw_triangle(triangle_draw_x,y-padding/2.0,triangle_draw_x+padding,y+padding/2.0,triangle_draw_x-padding,y+padding/2.0,true,tex_col);

		//Setup Text Drawing Coordinates
		double text_height=msl::text_height("Give Me Height!");
		double text_draw_x=x-display_width/2.0+padding;
		double text_draw_y=y-text_height/3.0;

		//Draw Selected Option Text
		if(value>=0&&value<options.size())
			msl::draw_text(text_draw_x,text_draw_y,options[value],tex_col);

		//Draw Menu
		if(selected)
		{
			//Figure Out Draw Coordinates and Dimensions
			double drop_menu_width=display_width;
			double drop_menu_height=options.size()*display_height;
			double drop_menu_y=y-display_height/2.0;
			double drop_menu_draw_y=y-display_height/2.0-drop_menu_height/2.0;

			//Draw Menu Background
			msl::draw_rectangle(x,drop_menu_draw_y,drop_menu_width,drop_menu_height,true,msl::color(0.7,0.7,0.7,1));

			//Figure Out Highlight Index
			double diff=drop_menu_y-mouse_y;
			int index=diff/button_.display_height;

			if((unsigned int)index>=options.size()||mouse_x<x-display_width/2.0||mouse_x>x+display_width/2.0||diff<0)
			{
				index=-1;
				hover=false;
			}

			//Draw Options
			for(unsigned int ii=0;ii<options.size();++ii)
			{
				//Text Color
				msl::color option_col=tex_col;

				//If Selected
				if(ii==(unsigned int)index)
				{
					//Change Text Color
					option_col=highlighted_text_color;

					//Figure Out Selection Rectangle Y
					double selection_y=drop_menu_y-display_height*(ii+0.5);

					//Draw Selection Background
					msl::draw_rectangle(x,selection_y,display_width,display_height,true,highlighted_background_color);
				}

				//Draw Option Text
				msl::draw_text(text_draw_x,text_draw_y-display_height*(ii+1),options[ii],option_col);
			}

			//Choose selected option...breaks rules of graphics...I can live with it though...
			if(index!=-1&&msl::input_check_released(mb_left))
				value=index;

			//Draw Menu Border
			msl::draw_rectangle(x,drop_menu_draw_y,drop_menu_width,drop_menu_height,false,msl::color(0,0,0,1));
		}
	}
}

void msl::dropdown::update_button(const double dt)
{
	button_.height=height;
	hover=button_.hover;
	down=button_.down;
	pressed=button_.pressed;
	button_.disabled=disabled;
	button_.visible=visible;
	display_width=button_.display_width;
	display_height=button_.display_height;
	button_.x=x;
	button_.y=y;
	button_.background_color_from=background_color_from;
	button_.background_color_to=background_color_to;
	button_.outline_color=outline_color;
	button_.outline_color_hover=outline_color_hover;
	button_.outline_color_disabled=outline_color_disabled;
	button_.text_color=text_color;
	button_.text_color_disabled=text_color_disabled;
	button_.padding=padding;

	button_.loop(dt);
}

msl::slider::slider(const double value,const double min,const double max,const double x,const double y,
	const bool vertical,const double length):
		widget(x,y,12,16),value(value),min(min),max(max),vertical(vertical),length(length),
		track_color(msl::color(0.3,0.3,0.3,1)),track_color_disabled(msl::color(0.4,0.4,0.4,1)),
		button_("",x,y),drag_(false)
{}

void msl::slider::loop(const double dt)
{
	if(visible)
	{
		update_button(dt);

		if(button_.down)
			drag_=true;

		if(msl::input_check_released(mb_left))
			drag_=false;

		if(drag_)
			down=true;

		double* pos=&x;
		double* button_pos=&button_.x;
		double* mouse_pos=&msl::mouse_x;

		if(vertical)
		{
			pos=&y;
			button_pos=&button_.y;
			mouse_pos=&msl::mouse_y;
		}

		if(drag_)
			*button_pos=*mouse_pos;
		else
			*button_pos=*pos+(value-min)/(max-min)*length;

		if(*button_pos<*pos)
			*button_pos=*pos;

		if(*button_pos>*pos+length)
			*button_pos=*pos+length;

		value=(*button_pos-*pos)/length*(max-min)+min;
	}
}

void msl::slider::draw()
{
	if(visible)
	{
		if(vertical)
		{
			button_.x=x;
		}
		else
		{
			button_.y=y;
		}

		msl::color track_col=track_color;

		if(disabled)
			track_col=track_color_disabled;

		if(vertical)
			msl::draw_rectangle(x,y+length/2.0,4,length,true,track_col);
		else
			msl::draw_rectangle(x+length/2.0,y,length,4,true,track_col);

		button_.draw();
	}
}

void msl::slider::update_button(const double dt)
{
	button_.down=down;
	button_.pressed=pressed;
	button_.disabled=disabled;
	button_.visible=visible;

	if(vertical)
	{
		button_.width=height;
		button_.height=width;
	}
	else
	{
		button_.width=width;
		button_.height=height;
	}

	button_.display_width=width;
	button_.display_height=height;
	display_width=width;
	display_height=height;

	button_.background_color_from=background_color_from;
	button_.background_color_to=background_color_to;
	button_.outline_color=outline_color;
	button_.outline_color_hover=outline_color_hover;
	button_.outline_color_disabled=outline_color_disabled;
	button_.text_color=text_color;
	button_.text_color_disabled=text_color_disabled;

	button_.loop(dt);
}

msl::textbox::textbox(const std::string& value,const double x,const double y):widget(x,y,-1,-1),focus(false),
	value(value),max_length(-1),readonly(false),padding(4),cursor(0),view_start(1),view_end(1),background_color(1,1,1,1),
	background_color_disabled(0.8,0.8,0.8,1),blink_timer_(0),blink_wait_(500),blink_show_(false),repeat_timer_(0),
	repeat_initial_wait_(500),repeat_key_wait_(80),repeat_key_(-1)
{}

void msl::textbox::loop(const double dt)
{
	//Limit Size
	if(max_length>=0&&value.size()>(unsigned int)max_length)
		value.resize(max_length);

	//Get Rid of Newlines...
	for(unsigned int ii=0;ii<value.size();++ii)
			if(value[ii]=='\n')
				value.erase(ii,1);

	//Figure Out Dimensions
	display_width=width+padding*2;

	if(width<0)
		display_width=msl::text_width(value)+padding*2;

	display_height=height+padding*2;

	if(height<0)
		display_height=msl::text_height(value)+padding*2;

	//Only Work When Enabled and Writable
	if(!disabled&&!readonly)
	{
		//Determine Mouse Hover
		bool new_hover=(msl::mouse_x>=x-display_width/2.0&&msl::mouse_x<=x+display_width/2.0&&
			msl::mouse_y>=y-display_height/2.0&&msl::mouse_y<=y+display_height/2.0);

		//Check For Mouse Leave
		if(hover&!new_hover)
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

		//Check For Mouse Enter
		if(!hover&new_hover)
			glutSetCursor(GLUT_CURSOR_TEXT);

		//Set Hover
		hover=new_hover;

		//Determine Down and Pressed
		down=hover&&msl::input_check(mb_left);
		pressed=hover&&msl::input_check_released(mb_left);

		//Determine Repeat Keys
		bool repeat=false;

		if(msl::millis()>repeat_timer_)
		{
			repeat_timer_=msl::millis()+repeat_key_wait_;
			repeat=true;
		}

		//Check for Mouse Clicks Inside of Textbox
		if(msl::input_check_pressed(mb_left))
		{
			//Set Focus
			focus=hover;

			//If Focusing, Put Cursor in the Right Spot
			if(focus)
			{
				//Get Cursor Difference
				double text_start_x=x-display_width/2.0+padding;
				double cursor_difference=mouse_x-text_start_x;

				//Put Cursor All the Way Back
				cursor=-1;

				//Find Cursor Position
				for(int ii=0;ii<view_end-view_start;++ii)
				{
					if(msl::text_width(value.substr(view_start,ii))>=cursor_difference)
					{
						cursor=view_start+ii;
						break;
					}
				}

				//If Cursor is -1, then Cursor is at the End
				if(cursor<0)
					cursor=view_end;
			}
		}

		//Check for Mouse Clicks Outside of Textbox
		if(!hover&&(msl::input_check_released(mb_left)||msl::input_check_released(mb_middle)||msl::input_check_released(mb_right)))
			focus=false;

		//If Focused
		if(focus)
		{
			//Blink Cursor
			if(msl::millis()>blink_timer_)
			{
				blink_timer_=msl::millis()+blink_wait_;
				blink_show_=!blink_show_;
			}

			//Move Cursor Right
			if(msl::input_check_pressed(kb_right)||(repeat_key_==kb_right&&repeat==true))
			{
				++cursor;
				repeat_check(kb_right);
			}

			//Move Cursor Left
			if(msl::input_check_pressed(kb_left)||(repeat_key_==kb_left&&repeat==true))
			{
				--cursor;
				repeat_check(kb_left);
			}

			//Move Cursor to Start
			if(msl::input_check_pressed(kb_home)||(repeat_key_==kb_home&&repeat==true))
			{
				cursor=0;
				repeat_check(kb_home);
			}

			//Move Cursor to End
			if(msl::input_check_pressed(kb_end)||(repeat_key_==kb_end&&repeat==true))
			{
				cursor=value.size();
				repeat_check(kb_end);
			}

			//Backspace
			if(msl::input_check_pressed(kb_backspace)||(repeat_key_==kb_backspace&&repeat==true))
			{
				if(cursor-1>=0)
				{
					--cursor;
					value.erase(cursor,1);
					find_end();
					repeat_check(kb_backspace);
				}
			}

			//Delete
			if(msl::input_check_pressed(kb_delete)||(repeat_key_==kb_delete&&repeat==true))
			{
				if((unsigned int)cursor<value.size())
				{
					value.erase(cursor,1);
					find_end();
					repeat_check(kb_delete);
				}
			}

			//Key Input
			for(int ii=32;ii<=126;++ii)
			{
				if(msl::input_check_pressed(ii)||(repeat_key_==ii&&repeat==true))
				{
					if(cursor>=0&&(unsigned int)cursor<=value.size())
					{
						value.insert(value.begin()+cursor,(char)ii);
						++cursor;
						find_end();
						repeat_check(ii);
					}
				}
			}
		}

		//If Not Focused
		else
		{
			cursor=0;
		}
	}

	//Check For Repeat Key Release
	if(msl::input_check_released(repeat_key_))
		repeat_key_=-1;

	//Limit Cursor
	if(cursor<0)
		cursor=0;

	if((unsigned int)cursor>value.size())
		cursor=value.size();

	//Limit View Start
	if(cursor<view_start)
	{
		view_start=cursor;
		find_end();
	}

	//Limit View End
	if(cursor>view_end)
	{
		view_end=cursor;
		find_start();
	}

	if((unsigned int)view_end>value.size())
		view_end=value.size();
}

void msl::textbox::draw()
{
	//To draw or not to draw...
	if(visible)
	{
		//Figure Out Colors
		msl::color bg_col=background_color;
		msl::color out_col=outline_color;
		msl::color tex_col=text_color;

		if(hover)
			out_col=outline_color_hover;

		if(disabled)
		{
			bg_col=background_color_disabled;
			out_col=outline_color_disabled;
			tex_col=text_color_disabled;
		}

		//Draw Background
		msl::draw_rectangle(x,y,display_width,display_height,true,bg_col);

		//Draw Border
		msl::draw_rectangle(x,y,display_width,display_height,false,out_col);

		//Draw Text
		std::string display_text=value.substr(view_start,view_end-view_start);
		double display_text_x=x-display_width/2.0+padding;
		double display_text_y=y-msl::text_height(display_text)/3.0;
		msl::draw_text(display_text_x,display_text_y,display_text,tex_col);

		//Draw Cursor
		if(focus&&blink_show_)
		{
			double cursor_x_start=x-display_width/2.0+padding;
			std::string cursor_x_text=value.substr(view_start,cursor-view_start);
			double cursor_x_text_width=msl::text_width(cursor_x_text);
			double cursor_x=cursor_x_start+cursor_x_text_width;
			double cursor_height=msl::text_height(display_text);
			msl::draw_line(cursor_x,y+cursor_height/2.0,cursor_x,y-cursor_height/2.0,msl::color(0,0,0,1));
		}
	}
}

void msl::textbox::find_end()
{
	//Determine Max Text Width
	double max_text_width=display_width-padding*2;

	//Check if Everything Fits
	if(msl::text_width(value.substr(view_start,value.size()-view_start))<=max_text_width)
	{
		view_end=value.size()-view_start;
		return;
	}

	//Find View End
	for(unsigned int ii=view_start;ii<value.size();++ii)
	{
		if(msl::text_width(value.substr(view_start,ii-view_start))<=max_text_width)
			view_end=ii;
		else
			break;
	}
}

void msl::textbox::find_start()
{
	//Determine Max Text Width
	double max_text_width=display_width-padding*2;

	//Find View Start
	for(int ii=0;ii<view_end;++ii)
	{
		if(msl::text_width(value.substr(ii,view_end-ii))<=max_text_width)
		{
			view_start=ii;
			break;
		}
	}
}

void msl::textbox::repeat_check(const int key)
{
	if(repeat_key_==-1)
	{
		repeat_key_=key;
		repeat_timer_=msl::millis()+repeat_initial_wait_;
	}
}