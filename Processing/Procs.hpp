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
class Procs // class for array of processes
{

public:

	class Proc // class for one process
	{
		enum Proc_state { NONE, STOP, IS_STOPPING, PAUSE, IS_RUNNING, RUN }; // process states
		Procs &procs; // reference to Procs object, which keeping this object
		Process& proc;
		string name; // name of this process
		deque <Event> events; // list of events
		shared_ptr<RenderTexture> frame_render_texture, frame_render_texture_temp;
		Proc_state state = STOP;// state -- current state
		deque<pair<Proc_state, deque<shared_ptr<Anim>>>> states_temp_with_anim_ptrs = {}; // queue of states which will be set to this process
		deque<shared_ptr<Anim>> anims_ptr_temp;
		deque<any> parameters, return_values;
		Proc(Process& pr, string name, Procs & procs) : // constructor, args: pointer of class Process, name of this process and reference to Procs object, which keeping this object  
			proc(pr),
			name(name),
			procs(procs),
			frame_render_texture(shared_ptr<RenderTexture>(new RenderTexture)),
			frame_render_texture_temp(shared_ptr<RenderTexture>(new RenderTexture))
		{
			frame_render_texture->create(procs.window.getSize().x, procs.window.getSize().y);
			frame_render_texture_temp->create(procs.window.getSize().x, procs.window.getSize().y);
		}

		Sprite add_animations_to_frame(Sprite frame) // animating current frame which will be drawn on screen
		{
			Sprite res(frame);
			deque<shared_ptr<Anim>> anims_ptr = get_anims_ptr_deque();
			for (auto itr = anims_ptr.begin(); itr != anims_ptr.end(); itr++)
			{
				res = (*itr)->modify(res);
			}
			return res;
		}

		Time get_anims_max_dur() // returns maximum of durations of animations
		{
			Time max_t = Time::Zero;
			deque<shared_ptr<Anim>> anims_ptr = get_anims_ptr_deque();
			for (auto itr = anims_ptr.begin(); itr != anims_ptr.end(); itr++) if (max_t < (*itr)->get_duration()) max_t = (*itr)->get_duration();
			return max_t;
		}

		bool is_anims_started() // returns true if all animations started, else returns false
		{
			bool started = 1;
			deque<shared_ptr<Anim>> anims_ptr = get_anims_ptr_deque();
			for (auto itr = anims_ptr.begin(); itr != anims_ptr.end(); itr++) started &= (*itr)->is_started();
			return started;
		}

		bool is_anims_ended() // returns true if all animations ended, else returns false
		{
			bool ended = 1;
			deque<shared_ptr<Anim>> anims_ptr = get_anims_ptr_deque();
			for (auto itr = anims_ptr.begin(); itr != anims_ptr.end(); itr++) ended &= (*itr)->is_ended();
			return ended;
		}

		void update() // doing all logic of process, setting new state to process
		{
			Proc_state state_temp = get_state_temp();
			if (state == STOP && state_temp == STOP) erase_state_temp();
			else if (state == STOP && state_temp == PAUSE) erase_state_temp();
			else if (state == STOP && state_temp == RUN) { add_this_process_to_drawing_procs(); proc.on_run(); state = IS_RUNNING; launch_animations(); }
			else if (state == STOP && state_temp == NONE);
			else if (state == IS_STOPPING && state_temp == STOP) { if (is_anims_ended()) { erase_this_process_from_drawing_procs(); proc.on_stop(); state = STOP; erase_state_temp(); } }
			else if (state == IS_STOPPING && state_temp == PAUSE);
			else if (state == IS_STOPPING && state_temp == RUN);
			else if (state == IS_STOPPING && state_temp == NONE);
			else if (state == PAUSE && state_temp == STOP) { state = IS_STOPPING; launch_animations(); }
			else if (state == PAUSE && state_temp == PAUSE) erase_state_temp();
			else if (state == PAUSE && state_temp == RUN) { proc.on_resume(); state = RUN; erase_state_temp(); }
			else if (state == PAUSE && state_temp == NONE);
			else if (state == IS_RUNNING && state_temp == STOP);
			else if (state == IS_RUNNING && state_temp == PAUSE);
			else if (state == IS_RUNNING && state_temp == RUN) { proc.loop(); if (is_anims_ended()) { state = RUN; erase_state_temp(); } }
			else if (state == IS_RUNNING && state_temp == NONE);
			else if (state == RUN && state_temp == STOP) { state = IS_STOPPING; launch_animations(); }
			else if (state == RUN && state_temp == PAUSE) { proc.on_pause(); state = PAUSE; erase_state_temp(); }
			else if (state == RUN && state_temp == RUN) { proc.loop(); erase_state_temp(); }
			else if (state == RUN && state_temp == NONE) proc.loop();
		}

		void Stop() // main logic of function stop() 
		{
			states_temp_with_anim_ptrs.push_back({ STOP, anims_ptr_temp });
			anims_ptr_temp.clear();
		}

		void Run() // main logic of function run() 
		{
			states_temp_with_anim_ptrs.push_back({ RUN, anims_ptr_temp });
			anims_ptr_temp.clear();
		}

		void erase_this_process_from_drawing_procs()
		{
			auto itr = find(procs.drawing_procs_temp.begin(), procs.drawing_procs_temp.end(), this);
			if (itr != procs.drawing_procs_temp.end()) procs.drawing_procs_temp.erase(itr);
		}

		void add_this_process_to_drawing_procs()
		{
			procs.drawing_procs_temp.push_back(this);
		}

		Proc_state get_state_temp()
		{
			return states_temp_with_anim_ptrs.empty() ? NONE : states_temp_with_anim_ptrs[0].first;
		}

		void erase_state_temp()
		{
			if (!states_temp_with_anim_ptrs.empty()) states_temp_with_anim_ptrs.pop_front();
		}

		deque<shared_ptr<Anim>> get_anims_ptr_deque()
		{
			return states_temp_with_anim_ptrs.empty() ? deque<shared_ptr<Anim>>({}) : states_temp_with_anim_ptrs[0].second;
		}

		void launch_animations()
		{
			deque<shared_ptr<Anim>> anims_ptr = get_anims_ptr_deque();
			for (auto itr = anims_ptr.begin(); itr != anims_ptr.end(); itr++) (*itr)->launch();
		}

	public:
		string get_name() // returns name of this process
		{
			return name;
		}

		bool get_event(Event &event) // getting event from events, if there is no event returns false, else returns true
		{
			if (events.empty()) return 0;
			event = events.back();
			events.pop_back();
			return 1;
		}

		void clear() // clearing current frame, which will be drawn on screen
		{
			frame_render_texture_temp->clear(Color::Transparent);
		}

		void draw(Drawable & drawable) // modifying current frame, which will be drawn on screen
		{
			frame_render_texture_temp->draw(drawable);
		}

		void display() // drawing current frame on screen
		{
			frame_render_texture_temp->display();
			frame_render_texture->clear(Color::Transparent);
			frame_render_texture->draw(Sprite(frame_render_texture_temp->getTexture()));
			frame_render_texture->display();
		}

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


		void pause() // pausing this process
		{
			states_temp_with_anim_ptrs.push_back({ PAUSE, {} });
		}


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

				if (state != PAUSE) anims_ptr_temp.push_back(TYPE_TO_SHPTR(Run, anim_type, dur_off.first, dur_off.second));

			run(args...);
		}


		void add_parametr(any value) // adding parametr to parametrs
		{
			parameters.push_back(value);
		}

		void add_return_value(any value) // adding return value tu return_values
		{
			return_values.push_back(value);
		}


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

private:

	RenderWindow& window; // reference of window, in which will be drawing from all processes of this object
	deque<Proc> procs; // processes
	deque<Proc*> drawing_procs, drawing_procs_temp; // current and next list of processes, which aren't stopped
	deque<Sprite> drawables; // frames from all processes, which will be drawn on window
	
	void get_drawables() // getting drawables from drawing_procs
	{
		for (auto itr = drawing_procs.begin(); itr != drawing_procs.end(); itr++)
		{
			drawables.push_back((*itr)->add_animations_to_frame(Sprite((*itr)->frame_render_texture->getTexture())));
		}
	}

public:
	Procs(RenderWindow &w): // constructor, args: reference of window
		window(w)
	{

	}

	void add_process(Process& pr, string name) // adding process to procs
	{
		procs.push_back(Proc(pr, name, *this));
	}

	Proc & operator[](string name) // for finding processes from procs with name
	{
		auto proc_itr = procs.end();
		for (auto itr = procs.begin(); itr != procs.end(); itr++)
		{
			if (itr->name == name)
			{
				proc_itr = itr;
				break;
			}
		}
		if (proc_itr == procs.end()) // asserting if process with wanted name not found
		{
			cout << "Assertion: there is no process named " << name << endl;
			abort();
		}
		return *proc_itr;
	}

	void add_events() // adding events to processes
	{
		Event event;
		while (window.pollEvent(event)) if (!drawing_procs.empty())
		{
			if (drawing_procs.back()->is_anims_ended()) drawing_procs.back()->events.push_front(event);
		}
	}

	void update_procs() // updating all processes
	{
		drawing_procs = drawing_procs_temp;
		for (auto itr = procs.begin(); itr != procs.end(); itr++)
		{
			itr->update();

		}
		for (auto itr = drawing_procs.begin(); itr != drawing_procs.end();)
		{
			if (find(drawing_procs_temp.begin(), drawing_procs_temp.end(), *itr) == drawing_procs_temp.end())
			{
				itr = drawing_procs.erase(itr);
				continue;
			}
			itr++;
		}

	}
	
	void clear() // clearing, drawing and displaying current frame on window
	{
		drawables.clear();
	}
	
	void draw()
	{
		window.clear();
		get_drawables();
		for (auto i = drawables.begin(); i != drawables.end(); i++)
		{
			window.draw(*i);
		}
	}

	void display()
	{
		window.display();
	}

};

typedef Procs::Proc Proc;
