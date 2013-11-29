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

class WebsocketServer : public SubscriberServer
{
public:
  WebsocketServer() 
    {
        // Initialize Asio Transport
        _m_server.init_asio();

        // Register handler callbacks
        _m_server.set_open_handler(bind(&SubscribeServer::on_open,
            this,::_1));
        _m_server.set_close_handler(bind(&SubscribeServer::on_close,
            this,::_1));
        _m_server.set_message_handler(bind(&SubscribeServer::on_message,
            this,::_1,::_2));
    }
}