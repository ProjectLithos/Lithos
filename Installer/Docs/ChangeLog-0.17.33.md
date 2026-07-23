# OESDK 0.17.34

- Fixed Visual Studio launcher debugger detection.
- The dynamically compiled `Oesdk.NativeDebuggerProbe` type and `IsDebuggerPresent` method are now public, so PowerShell can resolve and invoke the generated type without the "generated type is not public" warning or type lookup failure.
- Added regression coverage for every launcher copy.
