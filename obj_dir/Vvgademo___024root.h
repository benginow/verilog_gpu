// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vvgademo.h for the primary calling header

#ifndef VERILATED_VVGADEMO___024ROOT_H_
#define VERILATED_VVGADEMO___024ROOT_H_  // guard

#include "verilated.h"

class Vvgademo__Syms;
VL_MODULE(Vvgademo___024root) {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_OUT8(pixel,2,0);
    VL_OUT8(hsync_out,0,0);
    VL_OUT8(vsync_out,0,0);
    CData/*0:0*/ vgademo__DOT__inDisplayArea;
    CData/*0:0*/ vgademo__DOT__hvsync__DOT__vga_HS;
    CData/*0:0*/ vgademo__DOT__hvsync__DOT__vga_VS;
    CData/*0:0*/ vgademo__DOT__hvsync__DOT__CounterXmaxed;
    CData/*0:0*/ vgademo__DOT__hvsync__DOT__CounterYmaxed;
    CData/*0:0*/ __Vclklast__TOP__clk;
    SData/*9:0*/ vgademo__DOT__CounterX;
    SData/*9:0*/ vgademo__DOT__CounterY;

    // INTERNAL VARIABLES
    Vvgademo__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vvgademo___024root(Vvgademo__Syms* symsp, const char* name);
    ~Vvgademo___024root();
    VL_UNCOPYABLE(Vvgademo___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);


#endif  // guard
