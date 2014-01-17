#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>

/*#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>*/
#include <websocketpp/common/thread.hpp>

#include <chrono>

#define NUMBEROFTOPICS 6 //defines the number of possible subscribe topics

#include "actions.h"
#include "jsonparser.h"




std::string scenefile;
server::message_ptr initial_scene;

class SubscribeServer
{
public:
  SubscribeServer()
  {
    // Initialize Asio Transport
  _server.init_asio();

    // Register handler callbacks
  _server.set_open_handler(bind(&SubscribeServer::on_open
    , this, ::_1));
  _server.set_close_handler(bind(&SubscribeServer::on_close
    , this, ::_1));
  _server.set_message_handler(bind(&SubscribeServer::on_message
    , this, ::_1, ::_2));
  _server.set_http_handler(bind(&SubscribeServer::on_http
    , this, ::_1));
  }

  /**
  *
  *
  **/
  void run(uint16_t port)
  {
    // listen on specified port
    _server.listen(port);

    // Start the server accept loop
    _server.start_accept();

    // Start the ASIO io_service run loop
    try
    {
     _server.run();
    }

    catch (const std::exception & e)
    {
      std::cout << e.what() << std::endl;
    }
    catch (websocketpp::lib::error_code e)
    {
      std::cout << e.message() << std::endl;
    }
    catch (...)
    {
      std::cout << "other exception" << std::endl;
    }
  }

  void on_http(connection_hdl hdl)
  {
    server::connection_ptr con = _server.get_con_from_hdl(hdl);

    std::stringstream out;
    out << "<!doctype html><html><body>Request: "
      << con->get_resource()
      << "</body></html>";

    con->set_status(websocketpp::http::status_code::ok);
    con->set_body(out.str());
    std::cout << "Sorry, Websocket Connections only." << std::endl;
  }

  void on_open(connection_hdl hdl)
  {
    std::cout << "on_open" <<std::endl;
    _connections.insert(hdl);


    // unique_lock<mutex> lock(_action_lock);
    // //std::cout << "on_open" << std::endl;
    // _actions.push(action(SIGNIN,hdl));
    // lock.unlock();
    // _action_cond.notify_one();
  }

  void on_close(connection_hdl hdl)
  {
    std::cout << "on_close" <<std::endl;
    _connections.erase(hdl);


    // unique_lock<mutex> lock(_action_lock);
    // //std::cout << "on_close" << std::endl;
    // _actions.push(action(SIGNOUT,hdl));
    // lock.unlock();
    // _action_cond.notify_one();
  }



  void on_message(connection_hdl hdl, server::message_ptr msg)
  {
    try
    {
      std::cout << "on_message" <<std::endl;
      std::string incoming = msg->get_payload();
      std::cout << incoming <<std::endl;
      struct action newAction = parseMessage(incoming, hdl);
      std::cout << "Message parsed" << std::endl;
      process_action(newAction);
    }
    catch (std::string e)
    {
      std::cout << e << std::endl;
    }
    catch (std::exception& e)
    {
      std::cout << e.what() << std::endl;
    }

    // // queue message up for sending by processing thread
    // unique_lock<mutex> lock(_action_lock);
    // //std::cout << "on_message" << std::endl;
    // _actions.push(action(MESSAGE,msg));
    // lock.unlock();
    // _action_cond.notify_one();
  }

  void process_action(struct action a)
  {
    if (a.type == SUBSCRIBE)
    {

      std::cout << "Adding Client to Subscriberlists." <<std::endl;

      for (std::vector<subscribe_topic>::iterator it = a.topics.begin()
        ; it != a.topics.end(); ++it)
      {
        if(*it == SOURCES) _source_subs.insert(a.hdl);
        else if(*it == GLOBAL) _global_subs.insert(a.hdl);
        else if(*it == REFERENCE) _reference_subs.insert(a.hdl);
        else if(*it == MASTERLEVEL) _masterlevel_subs.insert(a.hdl);
        else if(*it == SOURCELEVEL) _sourcelevel_subs.insert(a.hdl);
        else if(*it == LOUDSPEAKERLEVEL) _loudspeakerlevel_subs.insert(a.hdl);
      }
    }

    if (a.type == UNSUBSCRIBE)
    {

      std::cout << "Removing Client from Subscriberlists." <<std::endl;


      for (std::vector<subscribe_topic>::iterator it = a.topics.begin()
        ; it != a.topics.end(); ++it)
      {
        if(*it == SOURCES) _source_subs.erase(a.hdl);
        else if(*it == GLOBAL) _global_subs.erase(a.hdl);
        else if(*it == REFERENCE) _reference_subs.erase(a.hdl);
        else if(*it == MASTERLEVEL) _masterlevel_subs.erase(a.hdl);
        else if(*it == SOURCELEVEL) _sourcelevel_subs.erase(a.hdl);
        else if(*it == LOUDSPEAKERLEVEL) _loudspeakerlevel_subs.erase(a.hdl);
      }
    }

    if (a.type == MESSAGE)
    {
      for (std::map<std::string, map_value>::iterator it = a.load.begin()
        ; it != a.load.end(); ++it)
      {
        std::cout << it->first << " : ";
        if (it->second.type == BOOL)
          std::cout << it->second.getValue<bool>() << std::endl;
        if (it->second.type == STRING)
          std::cout << it->second.getValue<std::string>() << std::endl;
        if (it->second.type == INTEGER)
          std::cout << it->second.getValue<int>() << std::endl;
        if (it->second.type == DOUBLE)
          std::cout << it->second.getValue<double>() << std::endl;
      }
    }
  }

  // void process_messages()
  // {
  //   while(1)
  //   {
  //     unique_lock<mutex> lock(_action_lock);

  //     while(_actions.empty()) _action_cond.wait(lock);

  //     action a = _actions.front();
  //     _actions.pop();

  //     lock.unlock();

  //     if (a.type == SIGNIN)  //add new Client and send it the current scene
  //     {
  //       unique_lock<mutex> lock(_connection_lock);
  //       _connections.insert(a.hdl);

  //       //TODO: wait until client sends

  //       //add the client to the corresponding subscriber lists
  //       for (std::vector<subscribe_topic>::iterator it = a.topics.begin()
  //         ; it != a.topics.end(); ++it)
  //       {

  //       }

		//     //send current scene to new Subscriber
		//    _server.send(a.hdl,initial_scene);

		//     lock.unlock();

  //     }
  //     else if (a.type == SIGNOUT) //remove client from list
  //     {
  //       unique_lock<mutex> lock(_connection_lock);
  //       _connections.erase(a.hdl);
		//     lock.unlock();
  //     }
  //     else if (a.type == MESSAGE) //store changes to scene in queue
  //     {
		//     unique_lock<mutex> lock(_msg_lock);
		//     _msgs.push(a.msg);
		//     lock.unlock();
  //     }
  //     else
  //     {
  //       // undefined.
  //     }
  //   }
  // }

// void update_scene()
// {
//   while (1)
//   {
//     server::message_ptr messageToSend;
//     //std::string teststring = "This is a Test.";
//     //messageToSend->set_payload(teststring);
//     //wait for 100 ms
//     std::this_thread::sleep_for(std::chrono::milliseconds(100));

//     unique_lock<mutex> lock(_connection_lock);


//     con_list::iterator it;
//     for (it = _connections.begin(); it != _connections.end(); ++it)
//     {
//       _server.send(*it,messageToSend);
//     }
//   }
// }
private:
    typedef std::set<connection_hdl,std::owner_less<connection_hdl>> con_list;

    server _server;
    con_list _connections;
    std::queue<action> _actions;
    std::queue<server::message_ptr> _msgs;

    mutex _action_lock;
    mutex _connection_lock;
    mutex _msg_lock;
    condition_variable _action_cond;

    //For every Subscribe topic, theres a seperate list containing all clients
    //who subscribed to that topic
    con_list _source_subs;
    con_list _global_subs;
    con_list _reference_subs;
    con_list _masterlevel_subs;
    con_list _sourcelevel_subs;
    con_list _loudspeakerlevel_subs;
};

int main()
{
  try
    {
  ::scenefile = "enviroment.json";

  SubscribeServer server;

  // Start a thread to run the processing loop
  // thread processing_t(bind(&SubscribeServer::process_messages,&server));

  // Start a thread to run the update loop
  // thread update_t(bind(&SubscribeServer::update_scene,&server));

  // Run the asio loop with the main thread
  std::cout << "TestServer running..." << std::endl;
  server.run(9002);


  // processing_t.join();
  // update_t.join();

  }
  catch (std::exception & e)
  {
    std::cout << e.what() << std::endl;
  }

}

// Settings for Vim (http://www.vim.org/), please do not remove:
// vim:softtabstop=2:shiftwidth=2:expandtab:textwidth=80:cindent