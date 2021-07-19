///////////////////////////////////////////////////
/*************************************************/
/*                                               */
/*                                               */
/*           example for this project            */
/*                                               */
/*                                               */
/*************************************************/
///////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <atomic>
#include <deque>
#include <any>
#include "Procs.hpp"
#include "Proc.hpp"
#include "Process.hpp"
#include "Global.hpp"
using namespace sf;
class Start_menu :public Process
{
public:
	Sprite s;
	RectangleShape r;
	Start_menu() :
		s(t_start),
		r(Vector2f(100, 100)) // creating rectangle with sizes (100, 100) pixels   (square with side 100px)
	{
		r.setPosition(0, 0); // set the position of the square to coordinates (0, 0)
		r.setFillColor(Color::Green); // set the color of the square green
	}
	void on_run() // this function will be called when this process start
	{
		r.setPosition(0, 0); // set the position of the square to coordinates (0, 0)
		r.setScale(1, 1); // set the scale of the square to (1x, 1x)
	}
	void on_resume() // this function will be called when this process continue his work after being paused
	{
		r.scale(2.f, 2.f); // multiplying size of square by 2
	}
	void loop() // this function calling while this process running
	{
		Event event;
		while (procs["start"].get_event(event)) // getting events from global variable procs
		{
			if (event.type == Event::Closed) // if window's close button was pressed
			{
				procs["quit"].add_parametr(&procs["start"]); // adding pointer of this process to process 'quit' as running parametr
				procs["start"].pause(); // pause this process
				procs["quit"].run(From_point(300, 300), seconds(0.5)); // starting process 'quit' whith animation 'From point with coordinates (300, 300)(center coordinates of window)' and duration of the animation -> 0.5 seconds
			}
			if (event.type == Event::KeyPressed) // if any key pressed on keyboard
			{
				if (event.key.code == Keyboard::Key::L) // if on keyboard pressed key L
				{
					procs["start"].stop(To_point(300, 300), seconds(0.5)); // stopping this process with animation 'To point with coordinates (300, 300)(center coordinates of window)' and duration of the animation -> 0.5 seconds
					procs["level_select"].run(From_point(300, 300), { seconds(0.5), seconds(0.5) }); // running process 'level_select' with animation 'From point with coordinates (300, 300)(center coordinates of window)' and duration of the animation -> 0.5 seconds and offset of the animation -> 0.5 seconds 
				}
				else if (event.key.code == Keyboard::Key::Q) // if on keyboard pressed key Q
				{
					procs["quit"].add_parametr(&procs["start"]); // adding pointer of this process to process 'quit' as running parametr
					procs["start"].pause(); // pause this process
					procs["quit"].run(From_point(300, 300), seconds(0.5)); // starting process 'quit' whith animation 'From point with coordinates (300, 300)(center coordinates of window)' and duration of the animation -> 0.5 seconds
				}
			}
		}
		r.move(1, 0); // move green square to rightby 1 pixel
		procs["start"].clear(); // clear screen
		procs["start"].draw(s); // draw on screen anything you want
		procs["start"].draw(r);
		procs["start"].display(); // confirm your drawings
	}
	
};
class Level_select :public Process // for more detailed comments view previous class comments ^
{
public:
	Sprite s;
	Level_select() :
		s(t_level)
	{

	}
	void loop()
	{
		Event event;
		while (procs["level_select"].get_event(event))
		{
			if (event.type == Event::Closed) // if window's close button pressed, pause this process and run process 'quit'
			{
				procs["quit"].add_parametr(&procs["level_select"]);
				procs["level_select"].pause();
				procs["quit"].run(From_point(300, 300), seconds(0.5));
			}
			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Key::M) // if pressed key M, stop this process and run process 'start_menu'
				{
					procs["level_select"].stop(To_point(300, 300), seconds(0.5));
					procs["start"].run(From_point(300, 300), { seconds(0.5), seconds(0.5) });
				}
				else if (event.key.code == Keyboard::Key::Q) // if pressed key Q, pause this process and run process 'quit'
				{
					procs["quit"].add_parametr(&procs["level_select"]);
					procs["level_select"].pause();
					procs["quit"].run(From_point(300, 300), seconds(0.5));
				}
			}
		}
		procs["level_select"].clear();
		procs["level_select"].draw(s);
		procs["level_select"].display();
	}
};
class Quit :public Process
{
public:
	Sprite s;
	Proc* quit_call_proc_ptr;
	Quit() :
		s(t_quit)
	{
		s.setPosition(150, 225);
	}
	void on_run() // when this process started his work 
	{
		procs["quit"].get_parametr(quit_call_proc_ptr); // get pointer of process in which started this function in parametr 'quit_call_proc_ptr'
	}
	void loop()
	{
		Event event;
		while (procs["quit"].get_event(event))
		{
			if (event.type == Event::Closed) // if window's close button pressed, close window
			{
				window.close();
			}
			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Key::Y) // if key Y pressed, close window
				{
					window.close();
				}
				else if (event.key.code == Keyboard::Key::N) // if key N pressed, stop this process and run (or resume) process in which this process was started
				{
					procs["quit"].stop(To_point(300, 300), seconds(0.5));
					quit_call_proc_ptr->run();
				}
			}
		}
		procs["quit"].clear();
		procs["quit"].draw(s);
		procs["quit"].display();
	}
};
int main()
{
	window.create(VideoMode(600, 600), "Title");
	t_start.loadFromFile("start_menu.png"); 
	t_level.loadFromFile("level_select.png");
	t_quit.loadFromFile("quit.png");
	window.setFramerateLimit(60);
	Start_menu start_menu_proc; // creating processes 'start', 'level_select', 'quit'
	Level_select level_select_proc;
	Quit quit_proc;
	procs.add_process(start_menu_proc, "start"); // adding this processes to 'procs' to handle them in future
	procs.add_process(level_select_proc, "level_select");
	procs.add_process(quit_proc, "quit");
	procs["start"].run(From_point(300, 300), seconds(0.5)); // run process 'start' on start of program
	while (window.isOpen()) // while window is open, do the main logic of program
	{
		procs.add_events(); // adding events to the last process drawing on screen
		procs.update_procs(); // updating states of processes 
		procs.clear(); //clear window
		procs.draw(); //draw everithing on window
		procs.display(); //display changes on computer's monitor
	}
	return 0;
}

