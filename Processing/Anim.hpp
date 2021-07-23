#pragma once
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;
class Anim_type // base class of animation types
{
public:
	virtual Sprite modify(Sprite, Time, Time) = 0; // modifies and returns sprite, args: sprite which will be modified, elapsed time from begining of animation, duration of animation  
};
class Run_anim_type : public Anim_type // base class of animation types, which are used while process is running 
{
public:
	virtual Sprite modify(Sprite s, Time, Time) { return s; } // modifies and returns sprite, args: sprite which will be modified, elapsed time from begining of animation, duration of animation  
};
class Stop_anim_type : public Anim_type // base class of animation types, which are used while process is stopping 
{
public:
	virtual Sprite modify(Sprite s, Time, Time) { return s; } // modifies and returns sprite, args: sprite which will be modified, elapsed time from begining of animation, duration of animation  
};
class None : public Run_anim_type, public Stop_anim_type // animation type : None
{
	Sprite modify(Sprite s, Time, Time) { return s; } // modifies and returns sprite, args: sprite which will be modified, elapsed time from begining of animation, duration of animation  
};
class Anim // base class for animations
{
protected:
	Time duration, offset; // duration of animation and starting offset
	shared_ptr<Clock> timer_ptr{ nullptr };
	shared_ptr<Anim_type> type_ptr; // animation type pointer
public:

	Anim(shared_ptr<Anim_type> t_p, Time dur, Time off = seconds(0)) : // constructor, args: animation type pointer, duration, starting offset
		duration(dur),
		offset(off),
		type_ptr(t_p)
	{
		if (duration == Time::Zero) duration = microseconds(1); // duration of animation cannot be zero
	} 

	void launch()
	{
		timer_ptr = shared_ptr<Clock>(new Clock());
	}

	Sprite modify(Sprite spr) // modifies and returns sprite, args: sprite which will be modified 
	{
		if (!timer_ptr.get()) return spr;
		if (!is_started()) return type_ptr->modify(spr, seconds(0), duration);
		if (is_ended()) return type_ptr->modify(spr, duration, duration);
		Time elapsed = timer_ptr->getElapsedTime() - offset;
		return type_ptr->modify(spr, elapsed, duration);
	} 

	bool is_ended() // if animation ended returns true, else returns false
	{
		if (!timer_ptr.get()) return 0;
		return timer_ptr->getElapsedTime() > duration + offset;
	}
	
	bool is_started() // if animation started returns true, else returns false
	{
		if (!timer_ptr.get()) return 0;
		return timer_ptr->getElapsedTime() > offset;
	} 

	Time get_duration() // returns duration of animation
	{
		return duration;
	}

};
class Run_anim : public Anim // base class for animations, which are used while process is running
{
public:

	Run_anim(shared_ptr<Run_anim_type> t_p, Time dur, Time off = seconds(0)) : // constructor, args: animation type pointer, duration, starting offset
		Anim(shared_ptr<Anim_type>(t_p), dur, off)
	{

	}

protected:

};
class Stop_anim : public Anim // base class for animations, which are used while process is stopping
{
public:

	Stop_anim(shared_ptr<Stop_anim_type> t_p, Time dur, Time off = seconds(0)) : // constructor, args: animation type pointer, duration, starting offset
		Anim(shared_ptr<Anim_type>(t_p), dur, off)
	{

	} 

protected:
};









class From_point : public Run_anim_type
{
private:
	Vector2f point;
public:
	From_point(Vector2f p)
	{
		point = p;
	}
	From_point(float x, float y)
	{
		point.x = x;
		point.y = y;
	}
	Sprite modify(Sprite spr, Time elapsed, Time duration)
	{
		Sprite res = spr;
		res.setPosition((spr.getPosition() - point) * (elapsed / duration) + point);
		res.scale(elapsed / duration, elapsed / duration);
		return res;
	}
};

class To_point : public Stop_anim_type
{
private:
	Vector2f point;
public:
	To_point(Vector2f p)
	{
		point = p;
	}
	To_point(float x, float y)
	{
		point.x = x;
		point.y = y;
	}
	Sprite modify(Sprite spr, Time elapsed, Time duration)
	{
		Sprite res = spr;
		res.setPosition((spr.getPosition() - point) * (1.f - elapsed / duration) + point);
		res.scale(1.f - elapsed / duration, 1.f - elapsed / duration);
		return res;
	}
};


