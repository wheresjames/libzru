
#include <atomic>
#include <iostream>
#include <iomanip>
#include <cstring>

#include "libzru.h"

// Incremented when user clicks ctrl-c
volatile int fCtrlC = 0;

int main(int argc, char *argv[])
{
    zru::install_ctrl_c_handler(&fCtrlC);

    const char *pUsage = "USAGE : memmon <--share|-s share-name>"
                         "\n               <--size|z share-size>"
                         "\n               [--write|w data-to-write]"
                         ;

    // Read command line
    auto pbCl = zru::parsers::parse_command_line<zru::t_str>(argc, argv);
    pbCl.map_keys({ {"version", "v"},
                    {"share", "s"},
                    {"size", "z"},
                    {"write", "w"},
                    {"create", "c"}
                  });

    // Version string?
    if (pbCl.isset("version"))
    {   std::cout << APPVER << " [" << APPBUILD << "]" << std::endl;
        return 0;
    }

    // Show command line
    ZruShow("\n--- PARAMETERS ---\n", zru::parsers::dumpPb(pbCl));

    // Make sure we got what we need
    if (!pbCl.isset({"share", "size"}))
    {
        ZruError(pUsage);
        return -1;
    }

    // Open shared memory
    zru::shrmem sm;
    if (!sm.open(pbCl["share"].val().toString(), pbCl["size"].val().toInt(), pbCl.isset("create")) || !sm.ptr())
    {
        ZruError("Failed to open shared memory : ", pbCl["share"].val());
        return -1;
    }

    // Let the user know what's up
    ZruShow("Opened ", sm.isExisting() ? "existing" : "new", " share : ", pbCl["share"].val());

    int sz = sm.size();
    if (128 < sz)
        sz = 128;
    char buf[128 + 1] = {0};

    if (pbCl["write"].isset())
    {
        zru::t_str ws = pbCl["write"].val().toString();
        ZruShow("Writing : ", ws);
        if (ws.length() > sz)
            ws = ws.substr(0, sz);
        memcpy(sm.ptr(), ws.c_str(), ws.length());
    }

    else
    {
        while(!fCtrlC)
        {
            // Check for changes
            if (memcmp(buf, sm.ptr(), sz))
            {
                memcpy(buf, sm.ptr(), sz);
                buf[sz] = 0;
                std::cout   << "\n--- Shared memory changed ---\n"
                            << zru::parsers::dumpStr(zru::t_str(buf, sz))
                            << "\n";
            }
            // else
            //     std::cout << ".";

            fflush(stdout);

            // Once per second
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    return 0;
}

