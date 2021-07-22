#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <any>
#include <deque>
#include <string>
#include "Anim.hpp"
#include "Process.hpp"
using namespace sf;
using namespace std;
class Procs; //class for array of processes
class Proc // class for one process
{
	enum Proc_state { NONE, STOP, IS_STOPPING, PAUSE, IS_RUNNING, RUN }; // process states
	Procs &procs; // reference to Procs object, which keeping this object
	Process& proc; // events of this process
	string name; // name of this process
	deque <Event> events; // list of events
	shared_ptr<RenderTexture> frame_render_texture, frame_render_texture_temp;
	Proc_state state = STOP;// state -- current state
	deque<pair<Proc_state, deque<shared_ptr<Anim>>>> states_temp_with_anim_ptrs = {}; // queue of states which will be set to this process
	deque<shared_ptr<Anim>> anims_ptr_temp;
	deque<any> parameters, return_values; 
	Proc(Process&, string, Procs&); // constructor, args: pointer of class Process, name of this process and reference to Procs object, which keeping this object  
	Sprite add_animations_to_frame(Sprite); // animating current frame which will be drawn on screen
	Time get_anims_max_dur(); // returns maximum of durations of animations
	bool is_anims_started(); // returns true if all animations started, else returns false
	bool is_anims_ended(); // returns true if all animations ended, else returns false
	void update(); // doing all logic of process
	void Stop(); // main logic of function stop() 
	void Run(); // main logic of function run()
	void erase_this_process_from_drawing_procs();
	void add_this_process_to_drawing_procs();
	Proc_state get_state_temp();
	void erase_state_temp();
	deque<shared_ptr<Anim>> get_anims_ptr_deque();
	void launch_animations();
public:
	string get_name(); // returns name of this process
	bool get_event(Event&); // getting event from events, if there is no event returns false, else returns true
	void clear(); // clearing current frame, which will be drawn on screen
	void draw(Drawable&); // modifying current frame, which will be drawn on screen
	void display(); // drawing current frame on screen
#define _ASSERT_(x) \
	if (!is_base_of<x##_anim_type, x##_anim_type_class>::value)\
	{\
		cout << "Assertion: type of first, third, fifth... parameters of function stop/run must be derived from "#x"__anim_type_class" << endl;\
		abort();\
	}

#define TYPE_TO_SHPTR(x, a, b, c) \
	shared_ptr<Anim>(new x##_anim(shared_ptr<x##_anim_type_class>(new x##_anim_type_class(a)), b, c))

	 //all stop() functions for stopping this process, args: stopping animations and durations of that animations

	template<class Stop_anim_type_class = None>
	void stop(Stop_anim_type_class anim_type = None(), Time dur = Time::Zero)
	{
		_ASSERT_(Stop); // asserting if animation type is not a child class of Stop_anim_type class


		//adding animations to anims_ptr

		anims_ptr_temp.push_back(TYPE_TO_SHPTR(Stop, anim_type, dur, seconds(0)));

		Stop(); // stopping the process
	}
	template<class Stop_anim_type_class>
	void stop(Stop_anim_type_class anim_type, pair<Time, Time> dur_off)
	{

	
		_ASSERT_(Stop)

		anims_ptr_temp.push_back(TYPE_TO_SHPTR(Stop, anim_type, dur_off.first, dur_off.second));

		Stop();
	}
	template<class Stop_anim_type_class, class ...Args>
	void stop(Stop_anim_type_class anim_type, Time dur, Args... args)
	{
		_ASSERT_(Stop)

		anims_ptr_temp.push_back(TYPE_TO_SHPTR(Stop, anim_type, dur, seconds(0)));

		stop(args...);
	}
	template<class Stop_anim_type_class, class ...Args>
	void stop(Stop_anim_type_class anim_type, pair<Time, Time> dur_off, Args... args)
	{
		_ASSERT_(Stop)

		anims_ptr_temp.push_back(TYPE_TO_SHPTR(Stop, anim_type, dur_off.first, dur_off.second));

		stop(args...);
	}


	void pause(); // pausing this process


	//all run() functions for running this process, args: running animations and durations / {duraions, offsets} of that animations

	template<class Run_anim_type_class = None>
	void run(Run_anim_type_class anim_type = None(), Time dur = Time::Zero)
	{
		_ASSERT_(Run) // asserting if animation type is not a child class of Run_anim_type class


		//adding animations to anims_ptr if animation was stopped

		if (state != PAUSE) anims_ptr_temp.push_back(TYPE_TO_SHPTR(Run, anim_type, dur, seconds(0)));

		Run(); // running this process
	}
	template<class Run_anim_type_class>
	void run(Run_anim_type_class anim_type, pair<Time, Time> dur_off)
	{
		_ASSERT_(Run)

		if (state != PAUSE) anims_ptr_temp.push_back(TYPE_TO_SHPTR(Run, anim_type, dur_off.first, dur_off.second));

		Run();
	}
	template<class Run_anim_type_class, class ...Args>
	void run(Run_anim_type_class anim_type, Time dur, Args... args)
	{
		_ASSERT_(Run)

		if (state != PAUSE) anims_ptr_temp.push_back(TYPE_TO_SHPTR(Run, anim_type, dur, seconds(0)));

		run(args...);
	}
	template<class Run_anim_type_class, class ...Args>
	void run(Run_anim_type_class anim_type, pair<Time, Time> dur_off, Args... args)
	{
		_ASSERT_(Run)

		if(state != PAUSE) anims_ptr_temp.push_back(TYPE_TO_SHPTR(Run, anim_type, dur_off.first, dur_off.second));

		run(args...);
	}


	void add_parametr(any); // adding parametr to parametrs
	void add_return_value(any); // adding return value tu return_values


	template<class T>
	bool get_parametr(T& value) // getting parametr from parametrs, if it's impossible returns false, else returns true
	{
		for (auto itr = parameters.begin(); itr != parameters.end(); itr++)
		{
			T* ptr = any_cast<T>(&(*itr));
			if (ptr)
			{
				value = *ptr;
				parameters.erase(itr);
				return 1;
			}
		}
		return 0;
	}
	template<class T>


	bool get_return_value(T& value)  // getting return value from return_values, if it's impossible returns false, else returns true
	{
		for (auto itr = return_values.begin(); itr != return_values.end(); itr++)
		{
			T* ptr = any_cast<T>(&(*itr));
			if (ptr)
			{
				value = *ptr;
				return_values.erase(itr);
				return 1;
			}
		}
		return 0;
	}


	friend class Procs;
};

