// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VVGADEMO__SYMS_H_
#define VERILATED_VVGADEMO__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vvgademo.h"

// INCLUDE MODULE CLASSES
#include "Vvgademo___024root.h"

// SYMS CLASS (contains all model state)
class Vvgademo__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vvgademo* const __Vm_modelp;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vvgademo___024root             TOP;

    // CONSTRUCTORS
    Vvgademo__Syms(VerilatedContext* contextp, const char* namep, Vvgademo* modelp);
    ~Vvgademo__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

#endif  // guard
