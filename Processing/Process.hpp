#pragma once
using namespace std;
class Process
{
public:
	virtual void loop() {} // calling when process is running
	virtual void on_run() {} // calling when process is starting
	virtual void on_pause() {} // calling when process is pausing
	virtual void on_resume() {} // calling when process is resuming
	virtual void on_stop() {} // calling when process is stopping
};

