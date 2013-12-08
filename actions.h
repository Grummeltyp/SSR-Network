#ifndef actions_h_
#define actions_h_

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

enum action_type 
{
  SUBSCRIBE,
  UNSUBSCRIBE,
  MESSAGE
};

enum subscribe_topic
{
  SOURCES,
  GLOBAL,
  REFERENCE,
  MASTERLEVEL,
  SOURCELEVEL,
  LOUDSPEAKERLEVEL
};

//for the different kinds of messages
struct action 
{
  //constructor for subscribe/unsubscribe messages
  action(action_type t, connection_hdl h, std::vector<subscribe_topic> topics)
    : type(t)
    , hdl(h)
    , topics(topics)
  {}
  //constructor for other messages
  action(action_type t, server::message_ptr m)
    : type(t)
   // , msg_root(m)
  {}

  action_type type;

  websocketpp::connection_hdl hdl;
  std::vector<subscribe_topic> topics;
  //Json::Value msg_root;
};

#endif /* actions_h */