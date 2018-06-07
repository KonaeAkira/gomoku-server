#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <fstream>
#include <iostream>
#include <set>
#include <streambuf>
#include <string>
#include <thread>
#include <mutex>

namespace http_server
{

	class telemetry_server {
	public:
		typedef websocketpp::connection_hdl connection_hdl;
		typedef websocketpp::server<websocketpp::config::asio> server;
		
		std::vector<std::string> init_list;

		telemetry_server() {
		    // set up access channels to only log interesting things
		    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
		    m_endpoint.set_access_channels(websocketpp::log::alevel::access_core);
		    m_endpoint.set_access_channels(websocketpp::log::alevel::app);

		    // Initialize the Asio transport policy
		    m_endpoint.init_asio();

		    // Bind the handlers we are using
		    using websocketpp::lib::placeholders::_1;
		    using websocketpp::lib::bind;
		    m_endpoint.set_open_handler(bind(&telemetry_server::on_open,this,_1));
		    m_endpoint.set_close_handler(bind(&telemetry_server::on_close,this,_1));
		    m_endpoint.set_http_handler(bind(&telemetry_server::on_http,this,_1));
		}
		
		~telemetry_server() {
			m_endpoint.stop_listening();
		}

		void run(std::string docroot, uint16_t port) {
		    std::stringstream ss;
		    ss << "Running telemetry server on port "<< port <<" using docroot=" << docroot;
		    m_endpoint.get_alog().write(websocketpp::log::alevel::app,ss.str());
		    
		    m_docroot = docroot;
		    
		    // listen on specified port
		    m_endpoint.listen(port);

		    // Start the server accept loop
		    m_endpoint.start_accept();

		    // Start the ASIO io_service run loop
		    try {
		        m_endpoint.run();
		    } catch (websocketpp::exception const & e) {
		        std::cerr << e.what() << std::endl;
		    }
		}
		
		void broadcast_message(std::string &message) {
			con_list::iterator it;
			std::lock_guard<std::mutex> lock(m_connections_mtx);
		    for (it = m_connections.begin(); it != m_connections.end(); ++it) {
		        m_endpoint.send(*it,message,websocketpp::frame::opcode::text);
		    }
		    init_list.push_back(message);
		}

		void on_http(connection_hdl hdl) {
		    // Upgrade our connection handle to a full connection_ptr
		    server::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);
		
		    std::ifstream file;
		    std::string filename = con->get_resource();
		    std::string response;
		
		    m_endpoint.get_alog().write(websocketpp::log::alevel::app,
		        "http request1: "+filename);
		
		    if (filename == "/") {
		        filename = m_docroot+"index.html";
		    } else {
		        filename = m_docroot+filename.substr(1);
		    }
		    
		    m_endpoint.get_alog().write(websocketpp::log::alevel::app,
		        "http request2: "+filename);
		
		    file.open(filename.c_str(), std::ios::in);
		    if (!file) {
		        // 404 error
		        std::stringstream ss;
		    
		        ss << "<!doctype html><html><head>"
		           << "<title>Error 404 (Resource not found)</title><body>"
		           << "<h1>Error 404</h1>"
		           << "<p>The requested URL " << filename << " was not found on this server.</p>"
		           << "</body></head></html>";
		    
		        con->set_body(ss.str());
		        con->set_status(websocketpp::http::status_code::not_found);
		        return;
		    }
		
		    file.seekg(0, std::ios::end);
		    response.reserve(file.tellg());
		    file.seekg(0, std::ios::beg);
		
		    response.assign((std::istreambuf_iterator<char>(file)),
		                    std::istreambuf_iterator<char>());
		
		    con->set_body(response);
		    con->set_status(websocketpp::http::status_code::ok);
		}

		void on_open(connection_hdl hdl) {
			std::lock_guard<std::mutex> lock(m_connections_mtx);
		    m_connections.insert(hdl);
		    for (std::string &str : init_list)
		    	m_endpoint.send(hdl,str,websocketpp::frame::opcode::text);
		}

		void on_close(connection_hdl hdl) {
			std::lock_guard<std::mutex> lock(m_connections_mtx);
		    m_connections.erase(hdl);
		}
	private:
		typedef std::set<connection_hdl,std::owner_less<connection_hdl>> con_list;		
		server m_endpoint;
		con_list m_connections;
		std::mutex m_connections_mtx;
		std::string m_docroot;
	};
	
	telemetry_server server_instance;
	
	void start(uint16_t port) {
		server_instance.run("src/web/", port);
	}
	
	void broadcast(std::string str) {
		server_instance.broadcast_message(str);
	}
	
} // namespace server

