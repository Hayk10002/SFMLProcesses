#pragma once
#include <SFML/Graphics.hpp>
#include <deque>
#include <string>
#include "Proc.hpp"
#include "Process.hpp"
using namespace std;
using namespace sf;
class Procs // class for array of processes
{
private:
	RenderWindow& window; // reference of window, in which will be drawing from all processes of this object
	deque<Proc> procs; // processes
	deque<Proc*> drawing_procs, drawing_procs_temp; // current and next list of processes, which aren't stopped
	deque<Sprite> drawables; // frames from all processes, which will be drawn on window
	void get_drawables(); // getting drawables from drawing_procs
public:
	Procs(RenderWindow&); // constructor, args: reference of window
	void add_process(Process&, string); // adding process to procs
	Proc& operator[](string); // for finding processes from procs with name
	void add_events(); // adding events to processes
	void update_procs(); // updating all processes
	void clear(); // clearing, drawing and displaying current frame on window
	void draw(); 
	void display();
	friend class Proc;
};

