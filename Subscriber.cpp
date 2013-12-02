#include "Subscriber.h"



template <class Con_Template, class Msg_Template>
void SubscribeServer<Con_Template, Msg_Template>::on_newConnection(Con_Template con)
{
  std::unique_lock<std::mutex> lock(_action_lock);
  //std::cout << "on_open" << std::endl;
  _actions.push(new action_t(SIGNIN,con));
  lock.unlock();
  _action_cond.notify_one();
}

template <class Con_Template, class Msg_Template>
void SubscribeServer<Con_Template, Msg_Template>::on_closingConnection(Con_Template con) 
{
  std::unique_lock<std::mutex> lock(_action_lock);
  //std::cout << "on_close" << std::endl;
  _actions.push(new action_t(SIGNOUT,con));
  lock.unlock();
  _action_cond.notify_one();
}

template <class Con_Template, class Msg_Template>
void SubscribeServer<Con_Template, Msg_Template>::on_recieve(Con_Template con, Msg_Template msg)
{
  // queue message up for sending by processing thread
  std::unique_lock<std::mutex> lock(_action_lock);
  //std::cout << "on_message" << std::endl;
  _actions.push(new action_t(MESSAGE,msg));
  lock.unlock();
  _action_cond.notify_one();
}

template <class Con_Template, class Msg_Template>
void SubscribeServer<Con_Template, Msg_Template>::process_messages()
{
  while(1) 
  {
    std::unique_lock<std::mutex> lock(_action_lock);

	while(_actions.empty()) _action_cond.wait(lock);

	action_t a = _actions.front();
	_actions.pop();

	lock.unlock();

    if (a.type == SIGNIN)  //add new Client and send it the current scene
	{ 
	  std::unique_lock<std::mutex> lock(_connection_lock);
	  _connections.insert(a.hdl);
	  lock.unlock();
	} 
	else if (a.type == SIGNOUT) //remove client from list
	{ 
	  std::unique_lock<std::mutex> lock(_connection_lock);
	  _connections.erase(a.hdl);
	  lock.unlock();
	} 
	else if (a.type == MESSAGE) //store changes to scene in queue
	{ 
	  std::unique_lock<std::mutex> lock(_msg_lock);
	  _msgs.push(a.msg);
	  lock.unlock();
	} 
	else 
	{
	       // undefined.
	}
  }
}
    