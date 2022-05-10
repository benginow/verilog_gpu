// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vvgademo.h for the primary calling header

#include "verilated.h"

#include "Vvgademo___024root.h"

VL_INLINE_OPT void Vvgademo___024root___sequent__TOP__0(Vvgademo___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vvgademo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgademo___024root___sequent__TOP__0\n"); );
    // Init
    SData/*9:0*/ __Vdly__vgademo__DOT__CounterX;
    SData/*9:0*/ __Vdly__vgademo__DOT__CounterY;
    // Body
    __Vdly__vgademo__DOT__CounterX = vlSelf->vgademo__DOT__CounterX;
    __Vdly__vgademo__DOT__CounterY = vlSelf->vgademo__DOT__CounterY;
    vlSelf->vgademo__DOT__hvsync__DOT__vga_VS = ((0x1eaU 
                                                  < (IData)(vlSelf->vgademo__DOT__CounterY)) 
                                                 & (0x1ecU 
                                                    > (IData)(vlSelf->vgademo__DOT__CounterY)));
    vlSelf->vgademo__DOT__hvsync__DOT__vga_HS = ((0x290U 
                                                  < (IData)(vlSelf->vgademo__DOT__CounterX)) 
                                                 & (0x2f0U 
                                                    > (IData)(vlSelf->vgademo__DOT__CounterX)));
    __Vdly__vgademo__DOT__CounterX = ((IData)(vlSelf->vgademo__DOT__hvsync__DOT__CounterXmaxed)
                                       ? 0U : (0x3ffU 
                                               & ((IData)(1U) 
                                                  + (IData)(vlSelf->vgademo__DOT__CounterX))));
    if ((0x320U == (IData)(vlSelf->vgademo__DOT__CounterX))) {
        __Vdly__vgademo__DOT__CounterY = ((IData)(vlSelf->vgademo__DOT__hvsync__DOT__CounterYmaxed)
                                           ? 0U : (0x3ffU 
                                                   & ((IData)(1U) 
                                                      + (IData)(vlSelf->vgademo__DOT__CounterY))));
    }
    vlSelf->pixel = ((IData)(vlSelf->vgademo__DOT__inDisplayArea)
                      ? (7U & ((IData)(vlSelf->vgademo__DOT__CounterX) 
                               >> 7U)) : 0U);
    vlSelf->vsync_out = (1U & (~ (IData)(vlSelf->vgademo__DOT__hvsync__DOT__vga_VS)));
    vlSelf->hsync_out = (1U & (~ (IData)(vlSelf->vgademo__DOT__hvsync__DOT__vga_HS)));
    vlSelf->vgademo__DOT__inDisplayArea = ((0x280U 
                                            > (IData)(vlSelf->vgademo__DOT__CounterX)) 
                                           & (0x1e0U 
                                              > (IData)(vlSelf->vgademo__DOT__CounterY)));
    vlSelf->vgademo__DOT__CounterY = __Vdly__vgademo__DOT__CounterY;
    vlSelf->vgademo__DOT__CounterX = __Vdly__vgademo__DOT__CounterX;
    vlSelf->vgademo__DOT__hvsync__DOT__CounterYmaxed 
        = (0x20dU == (IData)(vlSelf->vgademo__DOT__CounterY));
    vlSelf->vgademo__DOT__hvsync__DOT__CounterXmaxed 
        = (0x320U == (IData)(vlSelf->vgademo__DOT__CounterX));
}

void Vvgademo___024root___eval(Vvgademo___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vvgademo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgademo___024root___eval\n"); );
    // Body
    if (((IData)(vlSelf->clk) & (~ (IData)(vlSelf->__Vclklast__TOP__clk)))) {
        Vvgademo___024root___sequent__TOP__0(vlSelf);
    }
    // Final
    vlSelf->__Vclklast__TOP__clk = vlSelf->clk;
}

#ifdef VL_DEBUG
void Vvgademo___024root___eval_debug_assertions(Vvgademo___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vvgademo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgademo___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
}
#endif  // VL_DEBUG
