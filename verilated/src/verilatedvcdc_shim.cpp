// -*- mode: C++; c-file-style: "cc-mode" -*-
/// \file
/// \brief VerilatedVcdC Shim: Expose C++ interface as C functions.

#include <verilated_vcd_c.h>
#if VERILATOR_VERSION_MAJOR >= 5
#include <verilated_trace.h>
#endif

// METHODS - User called
extern "C" {

VerilatedVcdC*
verilatedvcdc_new() {
  return new VerilatedVcdC();
}

void
verilatedvcdc_delete(VerilatedVcdC* vcd) {
  delete vcd;
}

// ACCESSORS
/// Is file open?
int
verilatedvcdc_is_open(VerilatedVcdC* vcd) {
  return vcd->isOpen() ? 1 : 0;
}

// METHODS
/// Open a new VCD file
/// This includes a complete header dump each time it is called,
/// just as if this object was deleted and reconstructed.
void
verilatedvcdc_open(VerilatedVcdC* vcd, const char* filename) {
  vcd->open(filename);
}

/// Continue a VCD dump by rotating to a new file name
/// The header is only in the first file created, this allows
/// "cat" to be used to combine the header plus any number of data files.
void
verilatedvcdc_open_next(VerilatedVcdC* vcd, int inc_filename) {
  vcd->openNext(inc_filename ? true : false);
}

/// Set size in megabytes after which new file should be created
void
verilatedvcdc_rollover_mb(VerilatedVcdC* vcd, size_t rolloverMB) {
#if VERILATOR_VERSION_MAJOR >= 5
  // In Verilator 5.x, rolloverMB is not available directly on VerilatedVcdC
  // This is a no-op for Verilator 5.x
  (void)vcd;
  (void)rolloverMB;
#else
  // In Verilator 4.x, the rolloverMB method is available
  vcd->rolloverMB(rolloverMB);
#endif
}

/// Close dump
void
verilatedvcdc_close(VerilatedVcdC* vcd) {
  vcd->close();
}

/// Flush dump
void
verilatedvcdc_flush(VerilatedVcdC* vcd) {
  vcd->flush();
}

/// Write one cycle of dump data
void verilatedvcdc_dump(VerilatedVcdC* vcd, vluint64_t timeui) {
  vcd->dump(timeui);
}

/// Set time units (s/ms, defaults to ns)
/// See also VL_TIME_PRECISION, and VL_TIME_MULTIPLIER in verilated.h
void
verilatedvcdc_set_time_unit(VerilatedVcdC* vcd, const char* unit) {
  vcd->set_time_unit(unit);
}

/// Set time resolution (s/ms, defaults to ns)
/// See also VL_TIME_PRECISION, and VL_TIME_MULTIPLIER in verilated.h
void
verilatedvcdc_set_time_resolution(VerilatedVcdC* vcd, const char* unit) {
  vcd->set_time_resolution(unit);
}

// Helper function for Verilator 5.x compatibility
#if VERILATOR_VERSION_MAJOR >= 5
VerilatedTraceBaseC* verilatedvcdc_as_trace_base(VerilatedVcdC* vcd) {
  return dynamic_cast<VerilatedTraceBaseC*>(vcd);
}
#endif

}
