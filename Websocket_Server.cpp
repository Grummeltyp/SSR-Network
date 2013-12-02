#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>

#include "Subscriber.h"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

class WebsocketServer : public SubscribeServer<websocketpp::connection_hdl,server::message_ptr>
{
public:
  WebsocketServer() 
  {
  // Initialize Asio Transport
  _server.init_asio();

  // Register handler callbacks
  _server.set_open_handler(bind(&WebsocketServer::on_newConnection,
    this,::_1));
  _server.set_close_handler(bind(&WebsocketServer::on_closingConnection,
    this,::_1));
  _server.set_message_handler(bind(&WebsocketServer::on_recieve,
    this,::_1,::_2));
  }

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
private:
  server _server;
};

int main()
{
  try
  { 
    WebsocketServer server;

    // Start a thread to run the processing loop
    thread processing_t(bind(&WebsocketServer::process_messages,&server));
        
    // // Start a thread to run the update loop
    // thread update_t(bind(&SubscribeServer::update_scene,&server));

    // Run the asio loop with the main thread
    std::cout << "TestServer running..." << std::endl;
    server.run(9002);
  

    processing_t.join();
    // update_t.join();

  } 
  catch  (std::exception & e) 
  {
    std::cout << e.what() << std::endl;
  }

}