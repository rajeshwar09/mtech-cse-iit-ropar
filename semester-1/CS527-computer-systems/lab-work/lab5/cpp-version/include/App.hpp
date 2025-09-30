#pragma once
#include <cstdint>
#include "Bus.hpp"

class App {
public:
    static App& instance();

    void init();
    // Lab-5 API expected by the PDF
    void read (unsigned int address, char* message, unsigned int size);
    void write(unsigned int address, char* message, unsigned int size);

private:
    Bus bus_;
    App() = default;
};
