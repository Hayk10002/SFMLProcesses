#include "Proc.hpp"
#include "Procs.hpp"

Proc::Proc(Process& pr, string name, Procs & procs) :
	proc(pr),
	name(name),
	procs(procs),
	frame_render_texture(shared_ptr<RenderTexture>(new RenderTexture)), 
	frame_render_texture_temp(shared_ptr<RenderTexture>(new RenderTexture))
{
	frame_render_texture->create(procs.window.getSize().x, procs.window.getSize().y);
	frame_render_texture_temp->create(procs.window.getSize().x, procs.window.getSize().y);
}

Sprite Proc::add_animations_to_frame(Sprite frame)
{
	Sprite res(frame);
	deque<shared_ptr<Anim>> anims_ptr = get_anims_ptr_deque();
	for (auto itr = anims_ptr.begin(); itr != anims_ptr.end(); itr++)
	{
		res = (*itr)->modify(res);
	}
	return res;
}

Time Proc::get_anims_max_dur()
{
	Time max_t = Time::Zero;
	deque<shared_ptr<Anim>> anims_ptr = get_anims_ptr_deque();
	for (auto itr = anims_ptr.begin(); itr != anims_ptr.end(); itr++) if (max_t < (*itr)->get_duration()) max_t = (*itr)->get_duration();
	return max_t;
}

bool Proc::is_anims_started()
{
	bool started = 1;
	deque<shared_ptr<Anim>> anims_ptr = get_anims_ptr_deque();
	for (auto itr = anims_ptr.begin(); itr != anims_ptr.end(); itr++) started &= (*itr)->is_started();
	return started;
}

bool Proc::is_anims_ended()
{
	bool ended = 1;
	deque<shared_ptr<Anim>> anims_ptr = get_anims_ptr_deque();
	for (auto itr = anims_ptr.begin(); itr != anims_ptr.end(); itr++) ended &= (*itr)->is_ended();
	return ended;
}

void Proc::update() // setting new state to process
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

void Proc::Stop()
{
	states_temp_with_anim_ptrs.push_back({ STOP, anims_ptr_temp });
	anims_ptr_temp.clear();
}

void Proc::Run()
{
	states_temp_with_anim_ptrs.push_back({ RUN, anims_ptr_temp });
	anims_ptr_temp.clear();
}

void Proc::erase_this_process_from_drawing_procs()
{
	auto itr = find(procs.drawing_procs_temp.begin(), procs.drawing_procs_temp.end(), this);
	if (itr != procs.drawing_procs_temp.end()) procs.drawing_procs_temp.erase(itr);
}

void Proc::add_this_process_to_drawing_procs()
{
	procs.drawing_procs_temp.push_back(this);
}

Proc::Proc_state Proc::get_state_temp()
{
	return states_temp_with_anim_ptrs.empty()?NONE: states_temp_with_anim_ptrs[0].first;
}

void Proc::erase_state_temp()
{
	if (!states_temp_with_anim_ptrs.empty()) states_temp_with_anim_ptrs.pop_front();
}

deque<shared_ptr<Anim>> Proc::get_anims_ptr_deque()
{
	return states_temp_with_anim_ptrs.empty() ? deque<shared_ptr<Anim>>({}) : states_temp_with_anim_ptrs[0].second;

}

void Proc::launch_animations()
{
	deque<shared_ptr<Anim>> anims_ptr = get_anims_ptr_deque();
	for (auto itr = anims_ptr.begin(); itr != anims_ptr.end(); itr++) (*itr)->launch();
}

void Proc::add_parametr(any value)
{
	parameters.push_back(value);
}

void Proc::add_return_value(any value)
{
	return_values.push_back(value);
}

string Proc::get_name()
{
	return name;
}

bool Proc::get_event(Event &event)
{
	if (events.empty()) return 0;
	event = events.back();
	events.pop_back();
	return 1;
}

void Proc::clear()
{
	frame_render_texture_temp->clear(Color::Transparent);
}

void Proc::draw(Drawable & drawable)
{
	frame_render_texture_temp->draw(drawable);
}

void Proc::display()
{
	frame_render_texture_temp->display();
	frame_render_texture->clear(Color::Transparent);
	frame_render_texture->draw(Sprite(frame_render_texture_temp->getTexture()));
	frame_render_texture->display();
}



void Proc::pause()
{
	states_temp_with_anim_ptrs.push_back({ PAUSE, {} });
}


