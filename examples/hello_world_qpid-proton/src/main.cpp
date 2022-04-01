

#include <proton/container.hpp>
#include <proton/listen_handler.hpp>
#include <proton/listener.hpp>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/sender.hpp>
#include <proton/sender_options.hpp>
#include <proton/source_options.hpp>
#include <proton/tracker.hpp>
#include <proton/delivery.hpp>
#include <proton/receiver_options.hpp>

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <cctype>
#include <thread>

static const int fixed_channel_id = 1;
static const char test_queue_name[] = "test_queue";

class server : public proton::messaging_handler {
  private:
    class listener_ready_handler : public proton::listen_handler {
        void on_open(proton::listener& l) override {
            std::cout << "listening on " << l.port() << std::endl;
        }
    };

    typedef std::map<std::string, proton::sender> sender_map;
    listener_ready_handler listen_handler;
    std::string url;
    sender_map senders;
    int address_counter;

  public:
    server(const std::string &u) : url(u), address_counter(0) {}

    void on_container_start(proton::container &c) override {
        c.listen(url, listen_handler);
    }

    std::string to_upper(const std::string &s) {
        std::string uc(s);
        size_t l = uc.size();

        for (size_t i=0; i<l; i++)
            uc[i] = static_cast<char>(std::toupper(uc[i]));

        return uc;
    }

    std::string generate_address() {
        std::ostringstream addr;
        addr << "server" << address_counter++;

        return addr.str();
    }

    void on_sender_open(proton::sender &sender) override {
        if (sender.source().dynamic()) {
            std::string addr = generate_address();
            sender.open(proton::sender_options().source(proton::source_options().address(addr)));
            senders[addr] = sender;
        }
    }

    void on_message(proton::delivery &, proton::message &m) override {
        std::cout << "Received " << m.body() << std::endl;

        std::string reply_to = m.reply_to();
        sender_map::iterator it = senders.find(reply_to);

        if (it == senders.end()) {
            std::cout << "No link for reply_to: " << reply_to << std::endl;
        } else {
            proton::sender sender = it->second;
            proton::message reply;

            reply.to(reply_to);
            reply.body(to_upper(proton::get<std::string>(m.body())));
            reply.correlation_id(m.correlation_id());

            sender.send(reply);
        }


    }
};

using proton::receiver_options;
using proton::source_options;

class client : public proton::messaging_handler {
  private:
    std::string url;
    std::vector<std::string> requests;
    proton::sender sender;
    proton::receiver receiver;

  public:
    client(const std::string &u, const std::vector<std::string>& r) : url(u), requests(r) {}

    void on_container_start(proton::container &c) override {
        sender = c.open_sender(url);
        // Create a receiver requesting a dynamically created queue
        // for the message source.
        receiver_options opts = receiver_options().source(source_options().dynamic(true));
        receiver = sender.connection().open_receiver("", opts);
    }

    void send_request() {
        proton::message req;
        req.body(requests.front());
        req.reply_to(receiver.source().address());
        sender.send(req);
    }

    void on_receiver_open(proton::receiver &) override {
        send_request();
    }

    void on_message(proton::delivery &d, proton::message &response) override {
        if (requests.empty()) return; // Spurious extra message!

        std::cout << requests.front() << " => " << response.body() << std::endl;
        requests.erase(requests.begin());

        if (!requests.empty()) {
            send_request();
        } else {
            d.connection().close();
        }
    }
};
void thread_function()
{
    try
    {
        server hw("127.0.0.1:5672");
        proton::container(hw).run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void thread_function2()
{
    try {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        std::vector<std::string> requests;
        requests.push_back("Twas brillig, and the slithy toves");
        requests.push_back("Did gire and gymble in the wabe.");
        requests.push_back("All mimsy were the borogroves,");
        requests.push_back("And the mome raths outgrabe.");

        client c("127.0.0.1:5672", requests);
        proton::container(c).run();

        return 0;
//    } catch (const example::bad_option& e)
//    {
//        std::cout << opts << std::endl << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
int main()
{
    try
    {
    
    std::thread t(&thread_function);   // t starts running
    
    std::thread t2(&thread_function2);   // t starts running
    
    t.join();   // main thread waits for the thread t to finish
    
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}
