#ifndef actions_h_
#define actions_h_

#include <json/json.h>

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

/**
 * The value of a Json Key is one of map_value's elements
 */
enum map_value
{
  bool boolean;
  std::string string;
  int integer;
  std::vector<int> integer_v;
  double decimal;
  std::vector<double> decimal_v;
  Json::Value json;
}

//for the different kinds of messages
struct action
{
  //constructor for subscribe/unsubscribe messages
  action(action_type t, connection_hdl h, std::vector<subscribe_topic> top)
    : type(t)
    , hdl(h)
    , topics(top)
  {}
  //constructor for other messages
  action(action_type t, std::map<std::string, map_value> l)
    : type(t)
    , load(l)
  {}

  action_type type;

  websocketpp::connection_hdl hdl;
  std::vector<subscribe_topic> topics;

  std::map<std::string, map_value> load;
  //Json::Value msg_root;
};

#endif /* actions_h */