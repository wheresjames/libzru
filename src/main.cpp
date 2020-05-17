#include <atomic>
#include <iostream>

#include "libzru.h"

#define THREADS     16
#define ITERATIONS  5000
#define RXPOP       100


static std::atomic<long> g_index(0);

class trackme
{
public:
    trackme() { std::cout << "trackme()\n"; }
    ~trackme() { std::cout << "~trackme()\n"; }
};

int StdTest()
{
    std::cout << zru::any(11).toString().c_str() << std::endl;
    std::cout << zru::any("12").toInt() << std::endl;

    zru::property_bag pb;

    pb["a"]["b"]["c"] = 13;
    pb.at(".", "d.e.f") = 14;

    std::cout << pb.at(".", "a.b.c").val().toInt() << std::endl;
    std::cout << pb.at(".", "d.e.f").val().toInt() << std::endl;

    std::cout << "before erase : isset(a.b.c) : " << pb.isset(".", "a.b.c") << std::endl;

    pb.erase(".", "a.b.c");

    std::cout << "after  erase : isset(a.b.c) : " << pb.isset(".", "a.b.c") << std::endl;

    pb[15] = 16;
    pb["why"] = "me";
    pb[L"why_wide"] = L"me_wide";
    pb.each([=](zru::property_bag::t_str k, zru::property_bag &v)->bool
    {
        std::cout << k.c_str() << " = " << v.val().toString().c_str() << std::endl;

        fflush(stdout);

        return true;
    });

    zru::vector v = zru::any(3.14).toVector();
    std::cout << zru::string(v.begin(), v.end()).c_str() << std::endl;

    // Run a thread
    zru::worker_thread::sptr someThread(
                new zru::worker_thread([]()->int
                {
                    std::cout << "Hello from thread...\n";

                    trackme a;
//                    std::this_thread::sleep_for(std::chrono::seconds(10));

                    return 10000;
                }));

    std::this_thread::sleep_for(std::chrono::seconds(1));

//    someThread->abort();

    someThread->join();

    std::cout << "Thread has shutdown\n";


    zru::worker_thread::sptr rxThread(
                new zru::worker_thread([]()->int
                {
                    std::cout << "RX thread: started...\n";

                    int i = 0;
                    do
                    {
                        zru::property_bag pb;
                        if (!zru::pb.pop(pb, ".", "thread.q", 3000))
                        {
                            std::cout << "RX thread: Failed to receive data from tx thread\n";
                            return -1;
                        }

                        i = pb.val().toInt();

                        std::cout << "RX thread: " << i << std::endl;

                    } while (3 > i);

                    std::cout << "Rx thread: shutting down...\n";

                    return -1;
                }));


    zru::worker_thread::sptr txThread(
                new zru::worker_thread([]()->int
                {
                    std::cout << "TX thread: started...\n";

                    for (int i = 0; i <= 3; i++)
                    {
                        std::this_thread::sleep_for(std::chrono::seconds(1));

                        std::cout << "TX thread: " << i << std::endl;

                        zru::pb.push(".", "thread.q", i, true);
                    }

                    std::cout << "TX thread: shutting down...\n";

                    return -1;
                }));


    rxThread->join();
    txThread->join();

    std::cout << "RX and TX threads have shutdown\n";


    zru::worker_thread::sptr waitThread(
                new zru::worker_thread([]()->int
                {
                    std::cout << "Wait thread: started...\n";

                    if (zru::pb.wait_multiple(".", {"key1", "key2"}, 3000))
                        std::cout << "Wait thread: A key changed!\n";
                    else
                        std::cout << "Wait thread: Nothing changed!\n";

                    std::cout << "Wait thread: shutting down...\n";

                    return -1;
                }));

    std::this_thread::sleep_for(std::chrono::seconds(1));

//    zru::pb.set(".", "key1", 1);
    zru::pb.set(".", "key2", 1);

    waitThread->join();

    std::cout << "\n --- End of Tests ---\n\n";

    return 0;
}

int main(int argc, char *argv[])
{
    g_index = 0;

    StdTest();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return 0;
}

