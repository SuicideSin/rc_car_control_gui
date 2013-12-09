//RC Controller Source
//	Created By:		Mike Moss
//	Modified On:	12/07/2013

//Serial Sync Protocol
//0		Movement Direction	('f'=forward/'n'=neutral/'r'=reverse)
//1		Movement Speed		(0-255)
//2		Turn Direction		(50=left/90=straight/130=right)

//Steering Defines
#define LEFT 50
#define STRAIGHT 90
#define RIGHT 130

//2D Header
#include "msl/2d.hpp"

//GUI Header
#include "msl/glut_ui.hpp"

//Serial Utility Header
#include "msl/serial_util.hpp"

//Serial Sync Header
#include "msl/serial_sync.hpp"

//String Header
#include <string>

//String Utility Header
#include "msl/string_util.hpp"

//Main
int main()
{
	//Start 2D System
	msl::start_2d("RC Controller",640,480,false);

	//Return gracefully...never happens...
	return 0;
}

//Global Spacing Variable
double spacing=10;

//Create Button Objects
msl::button forward_button("Forward");
msl::button reverse_button("Reverse");
msl::button right_button("Turn Right");
msl::button left_button("Turn Left");

//Create Speed Slider
msl::slider speed_slider(0,0,255);
std::string speed_text;
double speed_text_draw_x;
double speed_text_draw_y;

//Create Serial Port Dropdowns
msl::dropdown serial_ports;
msl::dropdown serial_baud;

//Create Serial Port Buttons
msl::button serial_connect("Connect");
msl::button serial_refresh("Refresh");

//Create Serial Sync Object
msl::serial_sync arduino_link;

//Close Arduino Link Function
void close_arduino_link()
{
	//Kill Serial Connect
	arduino_link.close();

	//Enable Serial Dropdowns
	serial_ports.disabled=false;
	serial_baud.disabled=false;
	serial_refresh.disabled=false;

	//Change Button Label
	serial_connect.value="Connect";
}

//Update Serial List Function
void update_serial_list()
{
	serial_ports.options=msl::list_serial_ports();
	serial_ports.options.insert(serial_ports.options.begin(),"Serial Port");
}

//Setup Function (Happens once)
void setup()
{
	//Create Serial Baud Rates
	serial_baud.options.push_back("Serial Baud Rate");
	serial_baud.options.push_back("9600");
	serial_baud.options.push_back("19200");
	serial_baud.options.push_back("38400");
	serial_baud.options.push_back("57600");
	serial_baud.options.push_back("115200");
	serial_baud.value=0;

	//Set Font
	msl::set_text_font("Ubuntu-B.ttf");
	msl::set_text_size(14);

	//Set Button Padding
	forward_button.padding=spacing*2;
	reverse_button.padding=spacing*2;
	right_button.padding=spacing*2;
	left_button.padding=spacing*2;
	serial_ports.padding=spacing;
	serial_baud.padding=spacing;
	serial_connect.padding=spacing;
	serial_refresh.padding=spacing;
	serial_connect.width=96;

	//Update Buttons Before Setting Positions
	forward_button.loop(0);
	reverse_button.loop(0);
	right_button.loop(0);
	left_button.loop(0);
	serial_ports.loop(0);
	serial_baud.loop(0);
	serial_connect.loop(0);
	serial_refresh.loop(0);

	//Set Button X Positions
	forward_button.x=0;
	reverse_button.x=0;
	right_button.x=reverse_button.display_width/2.0+spacing+right_button.display_width/2.0;
	left_button.x=-reverse_button.display_width/2.0-spacing-left_button.display_width/2.0;

	//Set Button Y Positions
	forward_button.y=spacing/2.0+forward_button.display_height/2.0;
	reverse_button.y=-spacing/2.0-reverse_button.display_height/2.0;
	right_button.y=reverse_button.y;
	left_button.y=reverse_button.y;

	//Set Speed Slider Length
	speed_slider.length=left_button.display_width+spacing+reverse_button.display_width+spacing+right_button.display_width+spacing;

	//Set Speed Slider Position
	speed_slider.x=-speed_slider.length/2.0;
	speed_slider.y=forward_button.y+forward_button.display_height/2.0+spacing*4;
	speed_text_draw_y=speed_slider.y+speed_slider.display_height+spacing-msl::text_height(speed_text)/3.0;

	//Set Dropdown Width
	serial_ports.width=240;

	//Update Serial List
	update_serial_list();
}

//Loop Function (Keeps happening)
void loop(const double dt)
{
	//Update Serial Ports Selection
	if((int)serial_ports.value<0||serial_ports.value>=serial_ports.options.size())
	{
		serial_ports.value=0;
		close_arduino_link();
	}

	//Update Serial Connect Button
	if(serial_connect.value=="Connect"&&(serial_ports.value==0||serial_baud.value==0))
	{
		serial_connect.disabled=true;
	}
	else
		serial_connect.disabled=false;

	//If Connect Button is Pressed
	if(serial_connect.pressed)
	{
		//If Connect
		if(serial_connect.value=="Connect")
		{
			//Kill Serial Connection
			close_arduino_link();

			//Disable Serial Dropdowns
			serial_ports.disabled=true;
			serial_baud.disabled=true;
			serial_refresh.disabled=true;

			//Set Serial Connect Button Text
			serial_connect.value="Trying...";

			//Assign Selected Serial Values If Valid
			if(serial_ports.value>=0&&serial_ports.value<serial_ports.options.size()&&serial_baud.value>=0&&
				serial_baud.value<serial_baud.options.size())
			{
				//Create Link
				arduino_link=msl::serial_sync(serial_ports.options[serial_ports.value],
					msl::to_int(serial_baud.options[serial_baud.value]));

				//Setup Link
				arduino_link.setup();

				//Check For Good Connection
				if(arduino_link.good())
				{
					serial_connect.value="Disconnect";
				}

				//Check For Bad Connection
				else
				{
					serial_connect.value="Connect";
					serial_ports.disabled=false;
					serial_baud.disabled=false;
					serial_refresh.disabled=false;
				}
			}
		}

		//If Disconnect
		else
		{
			close_arduino_link();
		}
	}

	//If Refresh Button is Pressed
	if(serial_refresh.pressed)
		update_serial_list();

	//Escape Key Escapes
	if(msl::input_check(kb_escape))
		exit(0);

	//Update Buttons
	forward_button.loop(dt);
	reverse_button.loop(dt);
	right_button.loop(dt);
	left_button.loop(dt);

	//Update Speed Slider
	speed_slider.loop(dt);

	//Limit Speed Slider to Integers
	speed_slider.value=(int)speed_slider.value;

	//Update Speed Slider Text and X Position
	speed_text="Move Speed:  "+msl::to_string(speed_slider.value);
	speed_text_draw_x=-msl::text_width(speed_text)/2.0;

	//Update Serial GUI Elements
	serial_ports.loop(dt);
	serial_baud.loop(dt);
	serial_connect.loop(dt);
	serial_refresh.loop(dt);

	//Set Serial Port GUI Element X Positions
	serial_ports.x=-msl::view_width/2.0+spacing+serial_ports.display_width/2.0;
	serial_baud.x=serial_ports.x+serial_ports.display_width/2.0+spacing+serial_baud.display_width/2.0;
	serial_connect.x=serial_baud.x+serial_baud.display_width/2.0+spacing+serial_connect.display_width/2.0;
	serial_refresh.x=serial_connect.x+serial_connect.display_width/2.0+spacing+serial_refresh.display_width/2.0;

	//Set Serial Port GUI Element Y Positions
	serial_ports.y=msl::view_height/2.0-spacing-serial_ports.display_height/2.0;
	serial_baud.y=msl::view_height/2.0-spacing-serial_baud.display_height/2.0;
	serial_connect.y=msl::view_height/2.0-spacing-serial_connect.display_height/2.0;
	serial_refresh.y=msl::view_height/2.0-spacing-serial_refresh.display_height/2.0;

	//Set Navigation Button Enables
	speed_slider.disabled=!serial_ports.disabled;
	forward_button.disabled=!serial_ports.disabled;
	reverse_button.disabled=!serial_ports.disabled;
	right_button.disabled=!serial_ports.disabled;
	left_button.disabled=!serial_ports.disabled;

	//Update Arduino Link RX
	arduino_link.update_rx();

	//Set Movement Direction
	arduino_link.set(0,'n');

	//Set Movement Speed
	arduino_link.set(1,speed_slider.value);

	//Set Turn Direction
	arduino_link.set(2,STRAIGHT);

	//W/Up Key
	if(msl::input_check(kb_w)||msl::input_check(kb_up))
		forward_button.down=true;

	//S/Down Key
	if(msl::input_check(kb_s)||msl::input_check(kb_down))
		reverse_button.down=true;

	//D/Right Key
	if(msl::input_check(kb_d)||msl::input_check(kb_right))
		right_button.down=true;

	//A/Left Key
	if(msl::input_check(kb_a)||msl::input_check(kb_left))
		left_button.down=true;

	//Forward
	if(forward_button.down&&!reverse_button.down)
		arduino_link.set(0,'f');

	//Reverse
	if(reverse_button.down&&!forward_button.down)
		arduino_link.set(0,'r');

	//Turn Right
	if(right_button.down&&!left_button.down)
		arduino_link.set(2,LEFT);

	//Turn Left
	if(left_button.down&&!right_button.down)
		arduino_link.set(2,RIGHT);

	//Update Arduino Link TX
	arduino_link.update_tx();
}

//Draw Function (Keeps happening)
void draw()
{
	//Draw Buttons
	forward_button.draw();
	reverse_button.draw();
	right_button.draw();
	left_button.draw();

	//Draw Speed Slider
	speed_slider.draw();
	msl::draw_text(speed_text_draw_x,speed_text_draw_y,speed_text,msl::color(0,0,0,1));

	//Draw Serial Port Dropdowns
	serial_ports.draw();
	serial_baud.draw();
	serial_connect.draw();
	serial_refresh.draw();
}