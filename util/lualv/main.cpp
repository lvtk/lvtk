
#include "console.hpp"
#include <iostream>
#include <sol/sol.hpp>

int main (int argc, char** argv) {
    sol::state lua;

    // this is not a real startup routine. but you can see
    // where I want to go with this.
    lua.open_libraries();

    auto console = lua.create_table();
    console["log"] = [&] (const sol::object& obj) {
        lua["print"](obj);
    };
    lua["console"] = console;

    lua["help"] = [&]() {
        std::cout << "help!!\n";
    };
    lua["quit"] = lua["os"]["exit"];

    // register some callbacks and/or intrusive
    // functions into the interpreter to be used
    // for smooth GUI operation.

    return lualv_main (lua, argc, argv);
}
