#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include <iostream>
#include <set>
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

// typedef websocketpp::server<websocketpp::config::asio> server;

enum action_type 
{
  SIGNIN,
  SIGNOUT,
  MESSAGE
};

template <class Con_Template, class Msg_Template>
struct action 
{
  action(action_type t, Con_Template h)
    : type(t)
    , hdl(h)
    {}
  action(action_type t, Msg_Template m)
    : type(t)
    , msg(m)
    {}

  action_type type;
	
  Con_Template hdl;
  Msg_Template msg;
};

template <class Con_Template, class Msg_Template>
class SubscribeServer
{
public:    
  typedef action<Con_Template, Msg_Template> action_t;

  SubscribeServer() {}

  //what to do when a new client connects
  void on_newConnection(Con_Template con); 

  //what to do when a client discs
  void on_closingConnection(Con_Template con);

  //what to do when a message is recieved
  void on_recieve(Con_Template con, Msg_Template msg);

  void process_messages();
 
private:
  typedef std::set<Con_Template,std::owner_less<Con_Template>> con_list;

  std::queue<action_t> _actions; //queue of actions to go through
  std::queue<Msg_Template> _msgs; //stores incoming messages
  con_list _connections; //list of connected clients


  //for every SubscribeTopic, there is also a seperate Client List
  con_list _source_subs;
  con_list _global_subs;
  con_list _reference_subs;
  con_list _masterlevel_subs;
  con_list _sourcelevel_subs;
  con_list _loudspeakerlevel_subs;

  //mutexes and condition variables for acess purposes
  std::mutex _action_lock;
  std::mutex _connection_lock;
  std::mutex _msg_lock;

  std::condition_variable _action_cond;
};




template <class Con_Template, class Msg_Template>
void SubscribeServer<Con_Template
  , Msg_Template>::on_newConnection(Con_Template con)
{
  std::unique_lock<std::mutex> lock(_action_lock);
  //std::cout << "on_open" << std::endl;
  _actions.push(new action_t(SIGNIN,con));
  lock.unlock();
  _action_cond.notify_one();
}

template <class Con_Template, class Msg_Template>
void SubscribeServer<Con_Template
  , Msg_Template>::on_closingConnection(Con_Template con) 
{
  std::unique_lock<std::mutex> lock(_action_lock);
  //std::cout << "on_close" << std::endl;
  _actions.push(new action_t(SIGNOUT,con));
  lock.unlock();
  _action_cond.notify_one();
}

template <class Con_Template, class Msg_Template>
void SubscribeServer<Con_Template
  , Msg_Template>::on_recieve(Con_Template con, Msg_Template msg)
{
  // queue message up for sending by processing thread
  std::unique_lock<std::mutex> lock(_action_lock);
  //std::cout << "on_message" << std::endl;
  _actions.push(new action_t(MESSAGE,msg));
  lock.unlock();
  _action_cond.notify_one();
}

template <class Con_Template, class Msg_Template>
void SubscribeServer<Con_Template
  , Msg_Template>::process_messages()
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


#endif /* SUBSCRIBER_H_ */