# Coding Style

Thank you and welcome to coding and rotten, here I'll layout the general layout for the project and coding style.

## Directory Structure

Every library gets it's own folder in the `rotten/src` folder, the public include for those folders go inside `rotten/inc/rotten`. Inside each of the libraries folders, the only files not sorted into further subdirectory should be the `CMakeLists.txt` file and the internal header used to share resources just within that library.