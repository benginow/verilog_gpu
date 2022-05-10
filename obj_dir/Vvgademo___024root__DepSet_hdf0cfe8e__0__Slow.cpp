// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vvgademo.h for the primary calling header

#include "verilated.h"

#include "Vvgademo___024root.h"

VL_ATTR_COLD void Vvgademo___024root___settle__TOP__0(Vvgademo___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vvgademo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgademo___024root___settle__TOP__0\n"); );
    // Body
    vlSelf->hsync_out = (1U & (~ (IData)(vlSelf->vgademo__DOT__hvsync__DOT__vga_HS)));
    vlSelf->vsync_out = (1U & (~ (IData)(vlSelf->vgademo__DOT__hvsync__DOT__vga_VS)));
    vlSelf->vgademo__DOT__hvsync__DOT__CounterXmaxed 
        = (0x320U == (IData)(vlSelf->vgademo__DOT__CounterX));
    vlSelf->vgademo__DOT__hvsync__DOT__CounterYmaxed 
        = (0x20dU == (IData)(vlSelf->vgademo__DOT__CounterY));
}

VL_ATTR_COLD void Vvgademo___024root___eval_initial(Vvgademo___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vvgademo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgademo___024root___eval_initial\n"); );
    // Body
    vlSelf->__Vclklast__TOP__clk = vlSelf->clk;
}

VL_ATTR_COLD void Vvgademo___024root___eval_settle(Vvgademo___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vvgademo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgademo___024root___eval_settle\n"); );
    // Body
    Vvgademo___024root___settle__TOP__0(vlSelf);
}

VL_ATTR_COLD void Vvgademo___024root___final(Vvgademo___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vvgademo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgademo___024root___final\n"); );
}

VL_ATTR_COLD void Vvgademo___024root___ctor_var_reset(Vvgademo___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vvgademo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgademo___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->clk = VL_RAND_RESET_I(1);
    vlSelf->pixel = VL_RAND_RESET_I(3);
    vlSelf->hsync_out = VL_RAND_RESET_I(1);
    vlSelf->vsync_out = VL_RAND_RESET_I(1);
    vlSelf->vgademo__DOT__inDisplayArea = VL_RAND_RESET_I(1);
    vlSelf->vgademo__DOT__CounterX = VL_RAND_RESET_I(10);
    vlSelf->vgademo__DOT__CounterY = VL_RAND_RESET_I(10);
    vlSelf->vgademo__DOT__hvsync__DOT__vga_HS = VL_RAND_RESET_I(1);
    vlSelf->vgademo__DOT__hvsync__DOT__vga_VS = VL_RAND_RESET_I(1);
    vlSelf->vgademo__DOT__hvsync__DOT__CounterXmaxed = VL_RAND_RESET_I(1);
    vlSelf->vgademo__DOT__hvsync__DOT__CounterYmaxed = VL_RAND_RESET_I(1);
}
