#ifndef jsonparser_h_
#define jsonparser_h_

#include "actions.h"

action parseMessage(std::string load, connection_hdl con)
{
  Json::Value root;
  Json::Reader reader;

  try
  {
    bool parsingSuccessful = reader.parse(load, root);
    if (!parsingSuccessful)
      {
      // report to the user the failure and their locations in the JSON.
      std::cout  << "Failed to parse incoming Message\n"
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
        std::string current = topics[i].asString();
        if (current == "Sources") topic_v[i] = SOURCES;
        else if (current == "Global") topic_v[i] = GLOBAL;
        else if (current == "Reference") topic_v[i] = REFERENCE;
        else if (current == "Masterlevel") topic_v[i] = MASTERLEVEL;
        else if (current == "Sourcelevel") topic_v[i] = SOURCELEVEL;
        else if (current == "Loudspeakerlevel") topic_v[i] = LOUDSPEAKERLEVEL;
        else
        {
          throw "Invalid Topic Name\n";
        }
      }
      if (sub) return action(SUBSCRIBE, con, topic_v);
      return action(UNSUBSCRIBE, con, topic_v);
    }

    else if (type == "Message")
    {
      Json::Value msg_load = root.get(1, NULL);
      if (msg_load == NULL)
      {
        throw "Message Type specified, but no parameters given.\n";
      }
      Json::Value::Members param_names = msg_load.getMemberNames();

      std::map<std::string, map_value> params;

      for (int Json::Value::Members::iterator it = param_names.begin()
        ; it < param_names.end(); ++it)
      {
        switch *it
        {
          case "level":
          case "type":
          case "port":
          case "change":
          case "name":
          case "filename":
          case "id":
            if(!msg_load[*it].isString())
            {
              throw "Wrong format in Json Message\n";
            }
          //TODO
        }
      }
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
