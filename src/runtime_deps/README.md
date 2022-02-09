## `runtime_deps` = Runtime Dependencies

This directory contains code that is inserted into the runtime template to provide OS-dependent features transparently to the end-user, regardless of their true OS.
This includes multithreading and time for now.

The HVM compiler replaces certain comment strings/tokens in this template to generate an output C file for the user.
By replacing certain comments with whole source files in a platform-dependent way, we can help the compiler target different platforms.
-   replaced tokens are always block comments of the form `/* GENERATED_DEPENDENCY_<DEP_NAME> */`

    NOTE: this may change.
-   example `<DEP_NAME>` are `thread`, `types`, and other subdirectories.

For now, the compiler just targets the same platform it is running on: this can be easily changed via cross-compilation options.

