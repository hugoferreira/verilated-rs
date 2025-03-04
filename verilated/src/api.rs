// Copyright (C) 2018 Dan Glastonbury <dan.glastonbury@gmail.com>

use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_int};

mod ffi {
    #![allow(dead_code)]

    use std::ffi::c_void;
    use std::os::raw::{c_char, c_int};

    extern "C" {
        pub fn verilated_set_rand_reset(val: c_int);
        pub fn verilated_rand_reset() -> c_int;
        pub fn verilated_set_debug(level: c_int);
        pub fn verilated_debug() -> c_int;
        pub fn verilated_set_calc_unused_sigs(flag: c_int);
        pub fn verilated_calc_unused_sigs() -> c_int;
        pub fn verilated_set_got_finish(flag: c_int);
        pub fn verilated_got_finish() -> c_int;
        pub fn verilated_trace_ever_on(flag: c_int);
        pub fn verilated_set_assert_on(flag: c_int);
        pub fn verilated_assert_on() -> c_int;
        pub fn verilated_set_fatal_on_vpi_error(flag: c_int);
        pub fn verilated_fatal_on_vpi_error() -> c_int;
        pub fn verilated_command_args(argc: c_int, argv: *const *const c_char);
        //    static CommandArgValues* getCommandArgs() {return &s_args;}
        pub fn verilated_command_args_plus_match(prefixp: *const c_char) -> *const c_char;
        pub fn verilated_product_name() -> *const c_char;
        pub fn verilated_product_version() -> *const c_char;
        pub fn verilated_internals_dump();
        pub fn verilated_scopes_dump();
    }

    // Common callback type for all versions
    pub type VoidPCb = extern "C" fn(*mut c_void);

    // Use the new callback API for both Verilator 4.38+ and 5.x
    extern "C" {
        // These functions are available in both Verilator 4.38+ and 5.x
        pub fn verilated_add_flush_cb(cb: VoidPCb, datap: *mut c_void);
        pub fn verilated_remove_flush_cb(cb: VoidPCb, datap: *mut c_void);
        pub fn verilator_run_flush_callbacks();
        pub fn verilated_add_exit_cb(cb: VoidPCb, datap: *mut c_void);
        pub fn verilated_remove_exit_cb(cb: VoidPCb, datap: *mut c_void);
        pub fn verilator_run_exit_callbacks();

        // Legacy callback functions
        pub fn verilated_flush_call();
        pub fn verilated_flush_cb(cb: VoidPCb);
    }
}

#[derive(Copy, Clone, Debug, PartialEq, PartialOrd, Eq, Ord)]
pub enum RandomMode {
    AllZeros,
    AllBits,
    Randomize,
}

/// Select initial value of otherwise uninitialized signals.
///
/// 0 = Set to zeros
/// 1 = Set all bits to one
/// 2 = Randomize all bits
pub fn set_rand_reset(val: RandomMode) {
    let val = match val {
        RandomMode::AllZeros => 0,
        RandomMode::AllBits => 1,
        RandomMode::Randomize => 2,
    };
    unsafe {
        ffi::verilated_set_rand_reset(val);
    }
}

/// Return randReset value
pub fn rand_reset() -> RandomMode {
    let val = unsafe { ffi::verilated_rand_reset() };
    match val {
        0 => RandomMode::AllZeros,
        1 => RandomMode::AllBits,
        _ => RandomMode::Randomize,
    }
}

/// Enable debug of internal verilated code
// pub fn verilated_set_debug(on: c_int);

/// Return debug value
// pub fn verilated_debug() -> c_int

/// Enable calculation of unused signals
//pub fn verilated_set_calc_unused_sigs(flag: c_int);

/// Return calcUnusedSigs value
//pub fn verilated_calc_unused_sigs() -> c_int;

/// Did the simulation $finish?
pub fn set_finish() {
    unsafe {
        ffi::verilated_set_got_finish(1);
    }
}

/// Return if got a $finish
pub fn got_finish() -> bool {
    unsafe { ffi::verilated_got_finish() != 0 }
}

/// Allow traces to at some point be enabled (disables some optimizations)
pub fn trace_ever_on(on: bool) {
    unsafe { ffi::verilated_trace_ever_on(on as _) }
}

/// Enable/disable assertions
//pub fn verilated_set_assert_on(flag: c_int);
//pub fn verilated_assert_on()  -> c_int;

/// Enable/disable vpi fatal
//pub fn verilated_set_fatal_on_vpi_error(flag: c_int);
//pub fn verilated_fatal_on_vpi_error() -> c_int;

// Trampoline function for callbacks
extern "C" fn callback_trampoline<F>(user_data: *mut std::ffi::c_void)
where
    F: Fn(*mut std::ffi::c_void),
{
    let callback_ptr = user_data as *mut F;
    unsafe {
        let callback = &*callback_ptr;
        callback(user_data);
    }
}

/// Register a flush callback with data pointer
/// This is the preferred method for Verilator 4.38+ and 5.x
pub fn add_flush_cb<F>(callback: F, data: *mut std::ffi::c_void) 
where 
    F: Fn(*mut std::ffi::c_void)
{
    unsafe {
        // Use a static function as the callback
        ffi::verilated_add_flush_cb(callback_trampoline::<F>, data);
    }
}

/// Remove a flush callback
pub fn remove_flush_cb<F>(callback: F, data: *mut std::ffi::c_void)
where 
    F: Fn(*mut std::ffi::c_void)
{
    unsafe {
        // Use a static function as the callback
        ffi::verilated_remove_flush_cb(callback_trampoline::<F>, data);
    }
}

/// Run all registered flush callbacks
pub fn run_flush_callbacks() {
    unsafe {
        ffi::verilator_run_flush_callbacks();
    }
}

// Simple callback for the legacy interface
extern "C" fn simple_callback_trampoline(_: *mut std::ffi::c_void) {
    // This is a placeholder - the actual callback would be stored in a global or static variable
    // For now, we'll just provide a no-op implementation
}

/// Legacy flush callback interface (for older code)
/// This sets a single flush callback without a data pointer
/// For Verilator 5.x, this is a no-op
pub fn flush_cb<F>(_callback: F)
where
    F: Fn()
{
    unsafe {
        // Use a static function as the callback
        ffi::verilated_flush_cb(simple_callback_trampoline);
    }
}

/// Call the flush callback (legacy interface)
/// For Verilator 5.x, this calls run_flush_callbacks()
pub fn flush_call() {
    unsafe {
        ffi::verilated_flush_call();
    }
}

/// Record command line arguments, for retrieval by $test$plusargs/$value$plusargs
pub fn command_args(args: Vec<CString>) {
    let args: Vec<*const c_char> = args.into_iter().map(|a| a.as_ptr()).collect();
    unsafe {
        ffi::verilated_command_args(args.len() as c_int, args.as_ptr() as *const *const c_char);
    }
}

//pub fn verilated_command_args_add(argc: c_int, argv: *mut *const c_char);

//    static CommandArgValues* getCommandArgs() {return &s_args;}

/// Match plusargs with a given prefix. Returns static char* valid only for a single call
// pub fn verilated_command_args_plus_match(prefixp: *const c_char) -> *const c_char;

/// Produce name & version for (at least) VPI
pub fn product_name() -> &'static CStr {
    unsafe {
        let name = ffi::verilated_product_name();
        CStr::from_ptr(name)
    }
}

pub fn product_version() -> &'static CStr {
    unsafe {
        let version = ffi::verilated_product_version();
        CStr::from_ptr(version)
    }
}

/// For debugging, print much of the Verilator internal state.
/// The output of this function may change in future
/// releases - contact the authors before production use.
pub fn internals_dump() {
    unsafe {
        ffi::verilated_internals_dump();
    }
}

/// For debugging, print text list of all scope names with
/// dpiImport/Export context.  This function may change in future
/// releases - contact the authors before production use.
pub fn scopes_dump() {
    unsafe {
        ffi::verilated_scopes_dump();
    }
}
