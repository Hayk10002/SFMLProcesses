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
	Anim(shared_ptr<Anim_type>, Time, Time off = seconds(0)); // constructor, args: animation type pointer, duration, starting offset
	void launch();
	virtual Sprite modify(Sprite); // modifies and returns sprite, args: sprite which will be modified 
	bool is_ended(); // if animation ended returns true, else returns false
	bool is_started(); // if animation started returns true, else returns false
	Time get_duration(); // returns duration of animation

};
class Run_anim : public Anim // base class for animations, which are used while process is running
{
public:
	Run_anim(shared_ptr<Run_anim_type>, Time, Time off = seconds(0)); // constructor, args: animation type pointer, duration, starting offset
protected:

};
class Stop_anim : public Anim // base class for animations, which are used while process is stopping
{
public:
	Stop_anim(shared_ptr<Stop_anim_type>, Time, Time off = seconds(0)); // constructor, args: animation type pointer, duration, starting offset
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


