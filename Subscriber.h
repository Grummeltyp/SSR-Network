#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include <iostream>

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
  SubscribeServer() {}

  void on_newConnection(Con_Template con); //what to do when a new client connects

  void on_closingConnection(Con_Template con); //what to do when a client discs

  void on_recieve(Msg_Template msg); //what to do when a message is recieved

  void process_messages();
 
private:
  typedef std::set<Con_Template,std::owner_less<Con_Template>> con_list;

  std::queue<action> _actions; //queue of actions to go through
  con_list _connections; //list of connected clients

  //for every SubscribeTopic, there is also a seperate Client List
  con_list _source_subs;
  con_list _global_subs;
  con_list _reference_subs;
  con_list _masterlevel_subs;
  con_list _sourcelevel_subs;
  con_list _loudspeakerlevel_subs;

  //mutexes and condition variables for acess purposes
  mutex _action_lock;
  mutex _connection_lock;
  condition_variable _action_cond;
}

#endif /* SUBSCRIBER_H_ */