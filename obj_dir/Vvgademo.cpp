// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vvgademo.h"
#include "Vvgademo__Syms.h"

//============================================================
// Constructors

Vvgademo::Vvgademo(VerilatedContext* _vcontextp__, const char* _vcname__)
    : vlSymsp{new Vvgademo__Syms(_vcontextp__, _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , pixel{vlSymsp->TOP.pixel}
    , hsync_out{vlSymsp->TOP.hsync_out}
    , vsync_out{vlSymsp->TOP.vsync_out}
    , rootp{&(vlSymsp->TOP)}
{
}

Vvgademo::Vvgademo(const char* _vcname__)
    : Vvgademo(nullptr, _vcname__)
{
}

//============================================================
// Destructor

Vvgademo::~Vvgademo() {
    delete vlSymsp;
}

//============================================================
// Evaluation loop

void Vvgademo___024root___eval_initial(Vvgademo___024root* vlSelf);
void Vvgademo___024root___eval_settle(Vvgademo___024root* vlSelf);
void Vvgademo___024root___eval(Vvgademo___024root* vlSelf);
#ifdef VL_DEBUG
void Vvgademo___024root___eval_debug_assertions(Vvgademo___024root* vlSelf);
#endif  // VL_DEBUG
void Vvgademo___024root___final(Vvgademo___024root* vlSelf);

static void _eval_initial_loop(Vvgademo__Syms* __restrict vlSymsp) {
    vlSymsp->__Vm_didInit = true;
    Vvgademo___024root___eval_initial(&(vlSymsp->TOP));
    // Evaluate till stable
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial loop\n"););
        Vvgademo___024root___eval_settle(&(vlSymsp->TOP));
        Vvgademo___024root___eval(&(vlSymsp->TOP));
    } while (0);
}

void Vvgademo::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vvgademo::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vvgademo___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    // Initialize
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) _eval_initial_loop(vlSymsp);
    // Evaluate till stable
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Clock loop\n"););
        Vvgademo___024root___eval(&(vlSymsp->TOP));
    } while (0);
    // Evaluate cleanup
}

//============================================================
// Utilities

VerilatedContext* Vvgademo::contextp() const {
    return vlSymsp->_vm_contextp__;
}

const char* Vvgademo::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

VL_ATTR_COLD void Vvgademo::final() {
    Vvgademo___024root___final(&(vlSymsp->TOP));
}
