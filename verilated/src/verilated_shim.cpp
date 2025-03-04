// -*- mode: C++; c-file-style: "cc-mode" -*-
/// \file
/// \brief Verilated Shim: Expose C++ static interface as C functions.

#include <verilated.h>
#include <verilated_vcd_c.h>

// Add stub implementation for VlThreadPool for Verilator 5.x
#if (VERILATOR_VERSION_MAJOR >= 5)
#include <verilated_threads.h>

// Only define VlThreadPool if VL_THREADED is not defined
// This avoids redefinition with Verilator 5.x's own VlThreadPool
#if !defined(VL_THREADED) && !defined(VL_ENABLE_MT)
// Define a stub implementation that won't conflict with the real one
class VlThreadPoolStub {
public:
    VlThreadPoolStub(VerilatedContext* contextp, unsigned int nThreads) {}
    ~VlThreadPoolStub() {}
};
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Common callback type for all versions
typedef void (*voidp_cb)(void*);

// Verilator 5.x compatibility
#if (VERILATOR_VERSION_MAJOR >= 5)
#define VERILATOR_5X
#endif

// METHODS - User called

/// Select initial value of otherwise uninitialized signals.
////
/// 0 = Set to zeros
/// 1 = Set all bits to one
/// 2 = Randomize all bits
void
verilated_set_rand_reset(int val) {
  Verilated::randReset(val);
}

/// Return randReset value
int
verilated_rand_reset() {
  return Verilated::randReset();
}

/// Enable debug of internal verilated code
void
verilated_set_debug(int level) {
  return Verilated::debug(level);
}

/// Return debug value
int
verilated_debug() {
  return Verilated::debug();
}

/// Enable calculation of unused signals
void
verilated_set_calc_unused_sigs(int flag) {
  Verilated::calcUnusedSigs(flag != 0);
}

/// Return calcUnusedSigs value
int
verilated_calc_unused_sigs() {
  return Verilated::calcUnusedSigs() ? 1 : 0;
}

/// Did the simulation $finish?
void
verilated_set_got_finish(int flag) {
  Verilated::gotFinish(flag != 0);
}

/// Return if got a $finish
int
verilated_got_finish() {
  return Verilated::gotFinish() ? 1 : 0;
}

/// Allow traces to at some point be enabled (disables some optimizations)
void
verilated_trace_ever_on(int flag) {
  Verilated::traceEverOn(flag != 0);
}

/// Enable/disable assertions
void
verilated_set_assert_on(int flag) {
  Verilated::assertOn(flag != 0);
}

int
verilated_assert_on() {
  return Verilated::assertOn() ? 1 : 0;
}

/// Enable/disable vpi fatal
void
verilated_set_fatal_on_vpi_error(int flag) {
  Verilated::fatalOnVpiError(flag != 0);
}

int
verilated_fatal_on_vpi_error() {
  return Verilated::fatalOnVpiError() ? 1 : 0;
}

// In Verilator 5.x, callbacks are handled differently
// Use a common callback type for all versions

#if VERILATOR_VERSION_MAJOR >= 5
// Verilator 5.x uses void* callback signature directly

/// Callbacks to run on global flush
void
verilated_add_flush_cb(voidp_cb cb, void* datap) {
  Verilated::addFlushCb(cb, datap);
}

void
verilated_remove_flush_cb(voidp_cb cb, void* datap) {
  Verilated::removeFlushCb(cb, datap);
}

void
verilator_run_flush_callbacks() {
  Verilated::runFlushCallbacks();
}

/// Callbacks to run prior to termination
void
verilated_add_exit_cb(voidp_cb cb, void* datap) {
  Verilated::addExitCb(cb, datap);
}

void
verilated_remove_exit_cb(voidp_cb cb, void* datap) {
  Verilated::removeExitCb(cb, datap);
}

void
verilator_run_exit_callbacks() {
  Verilated::runExitCallbacks();
}

// Compatibility layer for legacy functions
void
verilated_flush_cb(voidp_cb cb) {
  // For compatibility with older code, add a callback that ignores the data pointer
  // This is a simple wrapper that calls the callback with NULL data
  if (cb) {
    Verilated::addFlushCb(cb, NULL);
  }
}

void
verilated_flush_call() {
  // Call the flush callbacks for compatibility
  Verilated::runFlushCallbacks();
}

#elif VERILATOR_VERSION_MAJOR == 4 && VERILATOR_VERSION_MINOR >= 38
// Define the legacy callback type for Verilator 4.x
typedef void (*VerilatedVoidCb)(void);

// Verilator 4.38+ already has the void* callback API

/// Callbacks to run on global flush
void
verilated_add_flush_cb(voidp_cb cb, void* datap) {
  Verilated::addFlushCb(cb, datap);
}

void
verilated_remove_flush_cb(voidp_cb cb, void* datap) {
  Verilated::removeFlushCb(cb, datap);
}

void
verilator_run_flush_callbacks() {
  Verilated::runFlushCallbacks();
}

/// Callbacks to run prior to termination
void
verilated_add_exit_cb(voidp_cb cb, void* datap) {
  Verilated::addExitCb(cb, datap);
}

void
verilated_remove_exit_cb(voidp_cb cb, void* datap) {
  Verilated::removeExitCb(cb, datap);
}

void
verilator_run_exit_callbacks() {
  Verilated::runExitCallbacks();
}

void
verilated_flush_cb(voidp_cb cb) {
  // Cast to the legacy type for older Verilator versions
  Verilated::flushCb((VerilatedVoidCb)cb);
}

void
verilated_flush_call() {
  Verilated::flushCall();
}

#else // Older Verilator versions
// Define the legacy callback type for older Verilator versions
typedef void (*VerilatedVoidCb)(void);

// For older Verilator versions, provide both APIs

// Implement the new-style callback API using the old API
void
verilated_add_flush_cb(voidp_cb cb, void* datap) {
  // Not directly supported in older Verilator versions
  // This is an approximation that ignores the data pointer
  if (cb) {
    // Cast to the legacy type for older Verilator versions
    Verilated::flushCb((VerilatedVoidCb)cb);
  }
}

void
verilated_remove_flush_cb(voidp_cb cb, void* datap) {
  // Not directly supported in older Verilator versions
  // No easy way to remove callbacks in the old API
}

void
verilator_run_flush_callbacks() {
  Verilated::flushCall();
}

// Not supported in older versions
void
verilated_add_exit_cb(voidp_cb cb, void* datap) {
  // Not directly supported in older Verilator versions
}

void
verilated_remove_exit_cb(voidp_cb cb, void* datap) {
  // Not directly supported in older Verilator versions
}

void
verilator_run_exit_callbacks() {
  // Not directly supported in older Verilator versions
}

// Legacy API
void
verilated_flush_cb(voidp_cb cb) {
  // Cast to the legacy type for older Verilator versions
  Verilated::flushCb((VerilatedVoidCb)cb);
}

void
verilated_flush_call() {
  Verilated::flushCall();
}
#endif

/// Record command line arguments, for retrieval by $test$plusargs/$value$plusargs
void
verilated_command_args(int argc, const char** argv) {
  Verilated::commandArgs(argc, argv);
}

/// Match plusargs with a given prefix. Returns static char* valid only for a single call
const char*
verilated_command_args_plus_match(const char* prefixp) {
  return Verilated::commandArgsPlusMatch(prefixp);
}

/// Produce name & version for (at least) VPI
const char*
verilated_product_name() {
  return Verilated::productName();
}

const char*
verilated_product_version() {
  return Verilated::productVersion();
}

/// For debugging, print much of the Verilator internal state.
/// The output of this function may change in future
/// releases - contact the authors before production use.
void
verilated_internals_dump() {
  Verilated::internalsDump();
}

/// For debugging, print text list of all scope names with
/// dpiImport/Export context.  This function may change in future
/// releases - contact the authors before production use.
void
verilated_scopes_dump() {
  Verilated::scopesDump();
}

#ifdef __cplusplus
}
#endif 