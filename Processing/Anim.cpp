#include "Anim.hpp"

Anim::Anim(shared_ptr<Anim_type> t_p, Time dur, Time off) :
	duration(dur),
	offset(off),
	type_ptr(t_p)
{
	if (duration == Time::Zero) duration = microseconds(1); // duration of animation cannot be zero
}

void Anim::launch()
{
	timer_ptr = shared_ptr<Clock>(new Clock());
}

Sprite Anim::modify(Sprite spr)
{
	if (!timer_ptr.get()) return spr;
	if (!is_started()) return type_ptr->modify(spr, seconds(0), duration);
	if (is_ended()) return type_ptr->modify(spr, duration, duration);
	Time elapsed = timer_ptr->getElapsedTime() - offset;
	return type_ptr->modify(spr, elapsed, duration);
}

bool Anim::is_ended()
{
	if (!timer_ptr.get()) return 0;
	return timer_ptr->getElapsedTime() > duration + offset;
}

bool Anim::is_started()
{
	if (!timer_ptr.get()) return 0;
	return timer_ptr->getElapsedTime() > offset;
}

Time Anim::get_duration()
{
	return duration;
}

Run_anim::Run_anim(shared_ptr<Run_anim_type> t_p, Time dur, Time off) :
	Anim(shared_ptr<Anim_type>(t_p), dur, off)
{

}

Stop_anim::Stop_anim(shared_ptr<Stop_anim_type> t_p, Time dur, Time off) :
	Anim(shared_ptr<Anim_type>(t_p), dur, off)
{

}

