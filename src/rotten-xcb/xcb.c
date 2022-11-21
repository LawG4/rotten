#include "rotten-core.h"
#include "rotten-xcb.h"

// Define two macro for loading in the xcb functions, one checks for null pointers which is more about
// checking if I have spelled the symbols correctly!
#ifndef __XCB_CHECK_FOR_NULL
#define LOAD_XCB_FN(X) lib->X = rotten_dynamic_library_fetch(lib->xcb_lib, "xcb_" #X)
#else
#define LOAD_XCB_FN(X)                                              \
    lib->X = rotten_dynamic_library_fetch(lib->xcb_lib, "xcb_" #X); \
    if (lib->X == NULL) return e_rotten_unclassified_error;
#endif

rotten_success_code rotten_library_xcb_load_min(rotten_library_xcb* lib)
{
    // This is just going to overwrite the values set, so it won't null pointer check, this is the callers
    // responsibility to check
    rotten_log_debug("Attempting to open libxcb.so", e_rotten_log_verbose);

    // Attempt to load the shared library libxcb.so
    lib->xcb_lib = rotten_dynamic_library_open("libxcb.so");
    if (lib->xcb_lib == NULL) {
        rotten_log("Failed to open libxcb.so", e_rotten_log_warning);
        return e_rotten_library_not_present;
    }

    // Load in the function pointers needed to connect to xcb. Which is literally just connect and disconnect
    LOAD_XCB_FN(connect);
    LOAD_XCB_FN(disconnect);

    return e_rotten_success;
}

rotten_success_code rotten_library_xcb_valid_session(rotten_library_xcb* lib)
{
    // Assumes the library has already had minimal loading
    xcb_connection_t* test_connection = lib->connect(NULL, NULL);

    // TODO: What value means that the x connection actually failed? Is it null? I assume so!
    if (test_connection == NULL) {
        rotten_log("Failed to form a connection to an xcb server", e_rotten_log_warning);
        e_rotten_feature_not_present;
    } else {
        rotten_log_debug("Connected to xcb server successfully!", e_rotten_log_info);
        lib->disconnect(test_connection);
        return e_rotten_success;
    }
}

rotten_success_code rotten_library_xcb_load_full(rotten_library_xcb* lib)
{
    // Assumes the library handle is loaded and the connect and disconnect functions are already loaded. Just
    // fetch the rest of the function pointers
    LOAD_XCB_FN(get_setup);
    LOAD_XCB_FN(setup_roots_iterator);
    LOAD_XCB_FN(generate_id);
    LOAD_XCB_FN(create_window);
    LOAD_XCB_FN(map_window);
    LOAD_XCB_FN(flush);
    LOAD_XCB_FN(intern_atom);
    LOAD_XCB_FN(intern_atom_reply);
    LOAD_XCB_FN(change_property);
    LOAD_XCB_FN(poll_for_event);
    return e_rotten_success;
}

rotten_success_code rotten_library_xcb_close(rotten_library_xcb* lib)
{
    rotten_dynamic_library_close(lib->xcb_lib);
    lib->xcb_lib == NULL;
}