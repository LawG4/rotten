// Exposing the listener struct has multiple members it's not an opaque pointer or incomplete struct class so
// we can't pass the xdg listner struct between libraries. Instead we have a static listner which will do all
// you need plus a function to attach this default listener to the window