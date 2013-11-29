#include "Subscriber.h"

void SubscribeServer::on_newConnection(Con_Template con)
{
  unique_lock<mutex> lock(_action_lock);
  //std::cout << "on_open" << std::endl;
  _m_actions.push(action(SIGNIN,con));
  lock.unlock();
  _action_cond.notify_one();
}

void SubscribeServer::on_closingConnection(Con_Template con) 
{
  unique_lock<mutex> lock(_action_lock);
  //std::cout << "on_close" << std::endl;
  _actions.push(action(SIGNOUT,con));
  lock.unlock();
  _action_cond.notify_one();
}

void SubscribeServer::on_recieve(Msg_Template msg)
{
  // queue message up for sending by processing thread
  unique_lock<mutex> lock(_action_lock);
  //std::cout << "on_message" << std::endl;
  _m_actions.push(action(MESSAGE,Msg_Template));
  lock.unlock();
  _action_cond.notify_one();
}

void SubsrcibeServer::process_messages()
{
  while(1) 
  {
    unique_lock<mutex> lock(_action_lock);

	while(_actions.empty()) _action_cond.wait(lock);

	action a = _actions.front();
	_actions.pop();

	lock.unlock();

    if (a.type == SIGNIN)  //add new Client and send it the current scene
	{ 
	  unique_lock<mutex> lock(_connection_lock);
	  connections.insert(a.hdl);
	  lock.unlock();
	} 
	else if (a.type == SIGNOUT) //remove client from list
	{ 
	  unique_lock<mutex> lock(_connection_lock);
	  _connections.erase(a.hdl);
	  lock.unlock();
	} 
	else if (a.type == MESSAGE) //store changes to scene in queue
	{ 
	  unique_lock<mutex> lock(_msg_lock);
	  _msgs.push(a.msg);
	  lock.unlock();
	} 
	else 
	{
	       // undefined.
	}
  }
}
    