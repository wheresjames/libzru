
#include <atomic>
#include <iostream>
#include <iomanip>

#include "libzru.h"

#define THREADS     16
#define ITERATIONS  5000
#define RXPOP       100

//static std::atomic<long> g_index(0);

class trackme
{
public:
    trackme() { std::cout << "trackme()\n"; }
    ~trackme() { std::cout << "~trackme()\n"; }
};

#define STARTTEST(s) ZruLog(s)
#define TEST(s) if (!(s)) { ZruError(#s); return -1; }


//-------------------------------------------------------------------
int Test_Any()
{
    STARTTEST("Test_Any");

    TEST(zru::any(11).toString() == "11");

    TEST(zru::any("12").toInt() == 12);

    TEST(zru::any("3").toFloat() == 3);

    TEST(zru::any('A').toString() == "A");

    TEST(zru::any(true).toString() == "true");
    TEST(zru::any("1") == true);
    TEST(zru::any("yes") == true);
    TEST(zru::any("on") == true);
    TEST(zru::any("true") == true);

    zru::vector v = zru::any(3.14).toVector();
    TEST(zru::string(v.begin(), v.end()) == "3.14");

    TEST(zru::any(&v).toString().substr(0, 3) == "[0x");

    return 0;
}

//-------------------------------------------------------------------
int Test_Str()
{
    STARTTEST("Test_Str");

    zru::t_anymap mVals({{"first", 12}, {"second", 13}});

    TEST(zru::str::map_values("first", mVals, zru::strpos(0)) == 12);
    TEST(zru::str::map_values("SeConD", mVals, zru::strpos(0)) == 13);
    //TEST(!zru::str::map_values("third", mVals, zru::strpos(0)).isSet());

    TEST(zru::str::UnescapeStr<zru::t_str>("\\r\\n", zru::strpos(0)) == "\r\n");

    return 0;
}

//-------------------------------------------------------------------
int Test_PropertyBag()
{
    STARTTEST("Test_PropertyBag");

    zru::property_bag pb;
    pb["a"]["b"]["c"] = 13;
    pb.at(".", "d.e.f") = 14;

    TEST(pb.at(".", "a.b.c").val() == 13);
    TEST(pb.at(".", "d.e.f").val() == 14);

    TEST(pb.isset(".", "a.b.c"));
    pb.erase(".", "a.b.c");
    TEST(!pb.isset(".", "a.b.c"));

    pb[15] = 16;
    pb["why"] = "me";
    pb[L"why_wide"] = L"me_wide";

    int n = 0;
    pb.each([&](auto k, auto v)->bool // > C++14
    {
        n++;
        return true;
    });
    TEST(5 == n);

    pb.clear();

    pb["a"] = 1;
    pb["b"] = 2;
    pb.map_keys({{"aaa", "a"}, {"bbb", "b"}});

    TEST(pb["aaa"].val() == 1);
    TEST(pb["bbb"].val() == 2);

    pb["bbb"] += 1;
    TEST(pb["bbb"].val() == 3);

    return 0;
}

//-------------------------------------------------------------------
int Test_Parsers()
{
    STARTTEST("Test_Parsers");

    const typename zru::string cmdline
        = "myapp.exe ok -v -xyz 1234 -i \"C:\\\\Program Files\" --out 'C:/temp' --p1:hi --p2:\"hi 1\" --p3:hi\\ 2 --p4 hi\\ 3 --p5:##1";

    auto pb = zru::parsers::parse_command_line(cmdline);

    TEST(pb["v"].val() == "1234");
    TEST(pb["x"].val() == "1234");
    TEST(pb["y"].val() == "1234");
    TEST(pb["z"].val() == "1234");
    TEST(pb["p1"].val() == "hi");
    TEST(pb["p2"].val() == "hi 1");
    TEST(pb["p3"].val() == "hi 2");
    TEST(pb["p4"].val() == "hi 3");
    TEST(pb["p5"].val() == "ok");
    TEST(pb["out"].val() == "C:/temp");
    TEST(pb["i"].val() == "C:\\Program Files");

    const typename zru::string jValid[] =
        {
            "{\"a\":\"b\", \"c\":{\"d\":3.14}, \"e\":[11,22,33,44,\"ok\"]}",
            "{\"a\":\"b\", \"c\":{\"d\":3.14,\"e\":{\"int\":1234,\"true\":true,\"false\":false}}}"
        };

    pb = zru::parsers::json_parse(jValid[0]);
//    std::cout << zru::parsers::dumpPb(pb) << std::endl;
    std::cout << zru::parsers::json_encode(pb, false) << std::endl;
    TEST(pb["a"].val() == "b");
    TEST(pb["c"]["d"].val() == 3.14);

    pb = zru::parsers::json_parse(jValid[1]);
    std::cout << zru::parsers::json_encode(pb, false) << std::endl;
    TEST(pb["a"].val() == "b");
    TEST(pb["c"]["d"].val() == 3.14);
    TEST(pb["c"]["e"]["int"].val() == 1234);
    TEST(pb["c"]["e"]["true"].val() == true);
    TEST(pb["c"]["e"]["false"].val() == false);

    const typename zru::string jBestEffort[] =
        {
            "{\"a\"=\"b\", \"c\":{\"d\":3.14}}",
            "a=b,c=d",
            "a=b\nc=d\ne={f:g,h=i}",
            "a\nb=c",
            "a=b;c=d\ne=f\n#g=h\ni=/*no*/j\nk=l//hello",
            "a = Hello World \n This Has = Spaces // Ha"
        };

    pb = zru::parsers::json_parse(jBestEffort[0]);
    std::cout << zru::parsers::json_encode(pb, false) << std::endl;

    return 0;
}

//-------------------------------------------------------------------
int Test_Threads()
{
    STARTTEST("Test_Threads");

    //---------------------------------------------------------------
    int value = 0;
    zru::worker_thread::sptr someThread(
                new zru::worker_thread([&value]()->int
                {
                    value = 11;
                    return 10000;
                }));
    someThread->join();
    TEST(11 == value);

    //---------------------------------------------------------------
    value = 0;
    zru::worker_thread::sptr rxThread(
                new zru::worker_thread([&value]()->int
                {
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
                        value += i;

                    } while (3 > i);

                    return -1;
                }));

    zru::worker_thread::sptr txThread(
                new zru::worker_thread([]()->int
                {
                    for (int i = 0; i <= 3; i++)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        zru::pb.push(".", "thread.q", i, true);
                    }
                    return -1;
                }));

    rxThread->join();
    txThread->join();

    TEST(6 == value); // 1 + 2 + 3 = 6


    //---------------------------------------------------------------
    value = 0;
    zru::pb.add_named_wait_multiple("thread-rx", ".", {"key1", "key2"});
    zru::worker_thread::sptr waitThread1(
                new zru::worker_thread([&value]()->int
                {
                    if (zru::pb.named_wait("thread-rx", 3000))
                        value++;
                    if (zru::pb.get(".", "key3").val() == -1)
                        return -1;
                    return 0;
                }));

    zru::pb.set(".", "key1", 1);
    zru::pb.set(".", "key2", 2);
    zru::pb.set(".", "key3", 3);
    zru::pb.set(".", "key2", -1);
    waitThread1->join();

    TEST(0 < value);
    TEST(3 == zru::pb.get_named_update_count("thread-rx"));

    zru::pb.remove_named_wait("thread-rx");


    // //---------------------------------------------------------------
    // value = 0;
    // zru::worker_thread::sptr waitThread2(
    //             new zru::worker_thread([&bInitialized, &value]()->int
    //             {
    //                 if (zru::pb.wait_multiple(".", {"key1", "key2"}, 3000))
    //                     value++;
    //                 return 0;
    //             }));

    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // zru::pb.set(".", "key1", 1);
    // zru::pb.set(".", "key2", 2);
    // zru::pb.set(".", "key3", 3);
    // waitThread2->join();

    // TEST(0 < value);

    return 0;
}

int main(int argc, char *argv[])
{
    int result = 0;

    std::cout << "\n --- Starting tests ---\n\n";

    result = Test_Any();
    if (result)
        return result;

    result = Test_Str();
    if (result)
        return result;

    result = Test_PropertyBag();
    if (result)
        return result;

    result = Test_Parsers();
    if (result)
        return result;

    result = Test_Threads();
    if (result)
        return result;

    std::cout << "\n --- Success ---\n\n";

    return 0;
}

