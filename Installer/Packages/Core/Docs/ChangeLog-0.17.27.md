# OESDK 0.17.28

- Fixed Visual Studio parsing of the base-template heap declarations.
- Removed `_Alignas` from the generated template because the heap initializers align their usable regions internally.
- Kept the arrays as ordinary static storage, which is accepted by Visual Studio IntelliSense, clang-cl, and the freestanding compiler.
- Added regression coverage preventing GNU or C11 alignment specifiers from returning to the Visual Studio base template.
- Synchronized all version headers and manifests to 0.17.28.
