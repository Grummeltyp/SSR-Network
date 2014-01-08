#ifndef jsonparser_h_
#define jsonparser_h_

#include "actions.h"


/* create a map for making the parameter names switchable*/
std::map<std::string, int> paramNumbers
{
  {"level", 0},
  {"type", 1},
  {"port", 2},
  {"change", 3},
  {"name", 4},
  {"filname", 5},
  {"id", 6},
  {"play", 7},
  {"mute", 8},
  {"fixed", 9},
  {"subwoofer", 10},
  {"referenceOrientation", 11},
  {"orientation", 12},
  {"duration", 13},
  {"volume", 14},
  {"channel", 15},
  {"referencePosition", 16},
  {"position", 17}
};

struct action parseMessage(std::string load, connection_hdl con)
{
  Json::Value root;
  Json::Reader reader;

  try
  {
    bool parsingSuccessful = reader.parse(load, root);
    if (!parsingSuccessful)
      {
      // report to the user the failure and their locations in the JSON.
      std::cout << "Failed to parse incoming Message\n"
        << reader.getFormatedErrorMessages();
      }


    std::string type = root.get("Type", "INVALID").asString();

    if (type == "Subscribe" || type == "Unsubscribe")
    {
      bool sub = true;
      if(type == "Unsubscribe") sub = false;

      Json::Value topics = root["Topics"];

      std::vector<subscribe_topic> topic_v;
      for (unsigned int i = 0; i < topics.size(); ++i)
      {
        std::cout << "Topic Number" << i+1<< std::endl;
        std::string current = topics[i].asString();
        if (current == "Sources") topic_v.push_back(SOURCES);
        else if (current == "Global") topic_v.push_back(GLOBAL);
        else if (current == "Reference") topic_v.push_back(REFERENCE);
        else if (current == "Masterlevel") topic_v.push_back(MASTERLEVEL);
        else if (current == "Sourcelevel") topic_v.push_back(SOURCELEVEL);
        else if (current == "Loudspeakerlevel")
          topic_v.push_back(LOUDSPEAKERLEVEL);
        else
        {
          throw "Invalid Topic Name\n";
        }
      }
      std::cout << "Returning from Parser" << std::endl;
      if (sub) return action(SUBSCRIBE, con, topic_v);
      return action(UNSUBSCRIBE, con, topic_v);
    }

    else if (type == "Message")
    {
      Json::Value msg_load = root.get(1, new Json::Value(0));
      if (msg_load.isNull())
      {
        throw "Message Type specified, but no parameters given.\n";
      }
      Json::Value::Members param_names = msg_load.getMemberNames();

      std::map<std::string, map_value> params;

      for (Json::Value::Members::iterator it = param_names.begin()
        ; it != param_names.end(); ++it)
      {
        std::pair<std::string, map_value> newVal;

        switch (paramNumbers[*it])
        {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
            if(msg_load[*it].isString())
            {
              newVal.first = *it;
              newVal.second.type = STRING;
              newVal.second.data.string = msg_load[*it].asString();
              break;
            }
            throw "Wrong format for string value\n";

          case 7:
          case 8:
          case 9:
          case 10:
            if(msg_load[*it].isBool())
            {
              newVal.first = *it;
              newVal.second.type = BOOL;
              newVal.second.data.b = msg_load[*it].asBool();
              break;
            }
            throw "Wrong format for bool value\n";

          case 11:
          case 12:
          case 13:
          case 14:
            if(msg_load[*it].isDouble())
            {
              newVal.first = *it;
              newVal.second.type = DOUBLE;
              newVal.second.data.decimal = msg_load[*it].asDouble();
              break;
            }
          case 15:
            if(msg_load[*it].isInt())
            {
              newVal.first = *it;
              newVal.second.type = INTEGER;
              newVal.second.data.integer = msg_load[*it].asInt();
              break;
            }
            throw "Wrong format for numeric value\n";

          case 16:
          case 17:
            if(msg_load[*it].isArray())
            {
              newVal.first = *it;
              newVal.second.type = INTARRAY;
              for (unsigned int i = 0; i < sizeof(msg_load[*it]); ++i)
              {
                newVal.second.data.integer_v[i] = msg_load[*it][i].asInt();
              }
              break;
            }
            throw "Wrong format for array value\n";
          default:
            throw "Wrong Key.\n";
        }
        /* Hint: If a key is inserted twice, the second insertion is ignored */
        params.insert(newVal);
      }
      return action(MESSAGE, params);
    }
    else
      throw "Mismatched Actiontype\n";
  }
  catch (std::string e)
  {
    std::cout << e << std::endl;
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
}

#endif /* jsonparser_h_ */
