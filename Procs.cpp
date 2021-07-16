#include "Procs.hpp"

void Procs::get_drawables()
{
	for (auto itr = drawing_procs.begin(); itr != drawing_procs.end(); itr++)
	{
		drawables.push_back((*itr)->add_animations_to_frame(Sprite((*itr)->frame_render_texture->getTexture())));
	}
}

Procs::Procs(RenderWindow &w):
	window(w)
{

}

void Procs::add_process(Process& pr, string name)
{
	procs.push_back(Proc(pr, name, *this));
}

Proc & Procs::operator[](string name)
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

void Procs::add_events()
{
	Event event;
	while (window.pollEvent(event)) if (!drawing_procs.empty())
	{
		if(drawing_procs.back()->is_anims_ended()) drawing_procs.back()->events.push_front(event);
	}
}

void Procs::update_procs()
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

void Procs::clear()
{
	drawables.clear();
}

void Procs::draw()
{
	window.clear();
	get_drawables();
	for (auto i = drawables.begin(); i != drawables.end(); i++)
	{
		window.draw(*i);
	}
}

void Procs::display()
{
	window.display();
}

