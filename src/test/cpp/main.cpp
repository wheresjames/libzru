
#include <atomic>
#include <iostream>
#include <iomanip>
#include <cstring>

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

#define assertTrue(s) { if (!(s)) { ZruError(#s); return -1; } else ZruInfo("PASS: " #s); }
#define assertFalse(s) { if ((s)) { ZruError(#s); return -1; } else ZruInfo("PASS: " #s); }


//-------------------------------------------------------------------
int Test_Any()
{
    assertTrue(zru::any(11).toString() == "11");

    assertTrue(zru::any("12").toInt() == 12);

    assertTrue(zru::any("3").toFloat() == 3);

    assertTrue(zru::any('A').toString() == "A");

    assertTrue(zru::any(true).toString() == "true");
    assertTrue(zru::any("1") == true);
    assertTrue(zru::any("yes") == true);
    assertTrue(zru::any("on") == true);
    assertTrue(zru::any("true") == true);

    zru::vector v = zru::any(3.14).toVector();
    assertTrue(zru::string(v.begin(), v.end()) == "3.14");

    assertTrue(zru::any(&v).toString().substr(0, 3) == "[0x");

    return 0;
}

//-------------------------------------------------------------------
int Test_Str()
{
    zru::t_anymap mVals({{"first", 12}, {"second", 13}});

    assertTrue(zru::str::map_values("first", mVals, zru::strpos(0)) == 12);
    assertTrue(zru::str::map_values("SeConD", mVals, zru::strpos(0)) == 13);
    //assertFalse(zru::str::map_values("third", mVals, zru::strpos(0)).isSet());

    assertTrue(zru::str::UnescapeStr<zru::t_str>("\\r\\n", zru::strpos(0)) == "\r\n");

    // MD5
    assertTrue(zru::md5::MD5().digestString("abcdefghijklmnopqrstuvwxyz")
                    == "98ef94f1f01ac7b91918c6747fdebd96");

    return 0;
}

//-------------------------------------------------------------------
int Test_PropertyBag()
{
    zru::property_bag pb;
    pb["a"]["b"]["c"] = 13;
    pb.at(".", "d.e.f") = 14;

    assertTrue(pb.at(".", "a.b.c").val() == 13);
    assertTrue(pb.at(".", "d.e.f").val() == 14);

    assertTrue(pb.isset(".", "a.b.c"));
    pb.erase(".", "a.b.c");
    assertTrue(!pb.isset(".", "a.b.c"));

    pb[15] = 16;
    pb["why"] = "me";
    pb[L"why_wide"] = L"me_wide";

    int n = 0;
    pb.each([&](auto /*k*/, auto /*v*/)->bool // > C++14
    {
        n++;
        return true;
    });
    assertTrue(5 == n);

    pb.clear();

    pb["a"] = 1;
    pb["b"] = 2;
    pb.map_keys({{"aaa", "a"}, {"bbb", "b"}});

    assertTrue(pb["aaa"].val() == 1);

    assertTrue(pb["bbb"].val() == 2);

    pb["bbb"] += 1;
    assertTrue(pb["bbb"].val() == 3);

    return 0;
}

//-------------------------------------------------------------------
int Test_Parsers()
{
    const typename zru::string cmdline
        = "myapp.exe ok -v -xyz 1234 -i \"C:\\\\Program Files\" --out 'C:/temp' --p1:hi --p2:\"hi 1\" --p3:hi\\ 2 --p4 hi\\ 3 --p5:##1";

    auto pb = zru::parsers::parse_command_line(cmdline);

    assertTrue(pb["v"].val() == "1234");
    assertTrue(pb["x"].val() == "1234");
    assertTrue(pb["y"].val() == "1234");
    assertTrue(pb["z"].val() == "1234");
    assertTrue(pb["p1"].val() == "hi");
    assertTrue(pb["p2"].val() == "hi 1");
    assertTrue(pb["p3"].val() == "hi 2");
    assertTrue(pb["p4"].val() == "hi 3");
    assertTrue(pb["p5"].val() == "ok");
    assertTrue(pb["out"].val() == "C:/temp");
    assertTrue(pb["i"].val() == "C:\\Program Files");

    pb.map_keys({{"xxx", "x"}, {"yyy", "y"}, {"zzz", "z"}});
    assertTrue(pb.isset({"xxx", "yyy"}));

    const typename zru::string jsonExamples[] =
        {
            "{\"a\":\"b\", \"c\":{\"d\":3.14}, \"e\":[11,22,33,44,\"ok\"]}",
            "{\"a\":\"b\", \"c\":{\"d\":3.14,\"e\":{\"int\":1234,\"true\":true,\"false\":false}}}"
        };

    pb = zru::parsers::json_parse(jsonExamples[0]);
    // std::cout << zru::parsers::json_encode(pb, false) << std::endl;
    assertTrue(pb["a"].val() == "b");
    assertTrue(pb["c"]["d"].val() == 3.14);
    assertTrue(pb["e"][1].val() == 22);

    pb = zru::parsers::json_parse(jsonExamples[1]);
    // std::cout << zru::parsers::json_encode(pb, false) << std::endl;
    assertTrue(pb["a"].val() == "b");
    assertTrue(pb["c"]["d"].val() == 3.14);
    assertTrue(pb["c"]["e"]["int"].val() == 1234);
    assertTrue(pb["c"]["e"]["true"].val() == true);
    assertTrue(pb["c"]["e"]["false"].val() == false);


    const typename zru::string cfgExamples[] =
        {
            "{\"a\"=\"b\", \"c\":{\"d\":3.14}}",
            "a=b,c=d",
            "a=b\nc=d\ne={f:g,h=i}",
            "a\nb=c/d=e",
            "a=b;c=d\ne=f\n#g=h\ni=/*no*/j\nk=l//hello",
            "a = Hello World \n This Has = Spaces // Ha"
        };

    pb = zru::parsers::config_parse(cfgExamples[0]);
//    std::cout << zru::parsers::dumpPb(pb) << std::endl;
    assertTrue(pb["a"].val() == "b");
    assertTrue(pb["c"]["d"].val() == 3.14);

    pb = zru::parsers::config_parse(cfgExamples[1]);
//    std::cout << zru::parsers::dumpPb(pb) << std::endl;
    assertTrue(pb["a"].val() == "b");
    assertTrue(pb["c"].val() == "d");

    pb = zru::parsers::config_parse(cfgExamples[2]);
//    std::cout << zru::parsers::dumpPb(pb) << std::endl;
    assertTrue(pb["a"].val() == "b");
    assertTrue(pb["c"].val() == "d");
    assertTrue(pb["e"]["f"].val() == "g");
    assertTrue(pb["e"]["h"].val() == "i");

    pb = zru::parsers::config_parse(cfgExamples[3]);
//    std::cout << zru::parsers::dumpPb(pb) << std::endl;
    assertTrue(pb.isset("a"));
    assertTrue(pb["b"].val() == "c");
    assertTrue(pb["d"].val() == "e");

    pb = zru::parsers::config_parse(cfgExamples[4]);
//    std::cout << zru::parsers::dumpPb(pb) << std::endl;
    assertTrue(pb["a"].val() == "b");
    assertTrue(pb["c"].val() == "d");
    assertTrue(pb["e"].val() == "f");
    assertTrue(!pb.isset("g"));
    assertTrue(pb["i"].val() == "j");
    assertTrue(pb["k"].val() == "l");

    pb = zru::parsers::config_parse(cfgExamples[5]);
//    std::cout << zru::parsers::dumpPb(pb) << std::endl;
    assertTrue(pb["a"].val() == "Hello World");
    assertTrue(pb["This Has"].val() == "Spaces");
    assertTrue(!pb.isset("Ha"));

    return 0;
}

//-------------------------------------------------------------------
int Test_Threads()
{
    //---------------------------------------------------------------
    int value = 0;
    zru::worker_thread::sptr someThread(
                new zru::worker_thread([&value]()->int
                {
                    value = 11;
                    return 10000;
                }));
    someThread->join();
    assertTrue(11 == value);

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

    assertTrue(6 == value); // 1 + 2 + 3 = 6


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

    assertTrue(0 < value);
    assertTrue(3 == zru::pb.get_named_update_count("thread-rx"));

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

//-------------------------------------------------------------------
int Test_Shrmem()
{
    zru::shrmem m1, m2;

    assertTrue(m1.open("/myshare", 1024));
    assertTrue(0 != m1.ptr());
    // ZruInfo("Shared memory exists: ", m1.isExisting());

    assertTrue(m2.open("/myshare", 1024));
    assertTrue(0 != m2.ptr());
    // ZruInfo("Shared memory exists: ", m2.isExisting());

    strcpy(m1.str(), "Hello");
    assertTrue(zru::t_str(m2.str()) == "Hello");

    return 0;
}


int main(int /*argc*/, char */*argv*/[])
{
    int result = 0;

    std::cout << " --- VERSION " APPVER " [" APPBUILD "] ---\n";
    std::cout << " --- Starting tests ---\n";

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

    result = Test_Shrmem();
    if (result)
        return result;

    std::cout << " --- Success ---\n";

    return 0;
}

