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
struct map_value_data
{
  int integer;
  bool b;
  std::string string;
  std::vector<int> integer_v;
  double decimal;
  std::vector<double> decimal_v;
  Json::Value json;
};

enum map_value_type
{
  BOOL,
  STRING,
  INTEGER,
  DOUBLE,
  INTARRAY
};

template <typename T>
struct getValueImp;

typedef struct map_value
{
  map_value_type type;
  map_value_data data;

  template <typename T>
  T getValue() {return getValueImp<T>::getValue(this);}
} map_value;


/* This needs to be specialised for every possible type of map_value*/
template <typename T>
struct getValueImp
{
  static T getValue(map_value* const value);
};

/*Specialisation for bool type*/
template<>
struct getValueImp<bool>
{
  static bool getValue(map_value* const value) {return value->data.b;}
};

/*Specialisation for bool type*/
template<>
struct getValueImp<std::string>
{
  static std::string getValue(map_value* const value) {return value->data.string;}
};

/*Specialisation for bool type*/
template<>
struct getValueImp<int>
{
  static int getValue(map_value* const value) {return value->data.integer;}
};

/*Specialisation for bool type*/
template<>
struct getValueImp<double>
{
  static double getValue(map_value* const value) {return value->data.decimal;}
};
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