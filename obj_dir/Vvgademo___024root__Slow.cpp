// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vvgademo.h for the primary calling header

#include "verilated.h"

#include "Vvgademo__Syms.h"
#include "Vvgademo___024root.h"

void Vvgademo___024root___ctor_var_reset(Vvgademo___024root* vlSelf);

Vvgademo___024root::Vvgademo___024root(Vvgademo__Syms* symsp, const char* name)
    : VerilatedModule{name}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vvgademo___024root___ctor_var_reset(this);
}

void Vvgademo___024root::__Vconfigure(bool first) {
    if (false && first) {}  // Prevent unused
}

Vvgademo___024root::~Vvgademo___024root() {
}
