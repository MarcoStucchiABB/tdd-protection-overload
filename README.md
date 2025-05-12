# Documentation
:dart: Target of this Chapter Day Project is to experiment about **Test Driven Design** and **Test Automation** in embedded world. 
An exemlplary TDD flow is applied to a Circuit Breaker Overload Protection as per standard IEC 60947-2.

## Main features
Step 1:
- **Windows** and **ARM** GCC toolchains used for tests (regardless of production toolchain)
- Test suite: **Unity** 🚀

Step 2:
- ARM emulation with **QEMU** 

## Testing process 
### Development process
Basic development process uses specific IDEs and compiler to generate an executable file for the target platform (e.g. Keil MDK and ARM LLVM C compiler).

![image](https://github.com/user-attachments/assets/61693346-283c-4f31-ab6a-b950c5782a5c)

### Test Process
On the other hand, testing process is typically based on a sepataed toolchain, and uses dedicated test files able to test a dedicated set of source files in an isolated environment, that is not necessarily the target environment. Typically the environment is a Host PC - your laptop or a cloud machine - and the toolchain includes GCC.

![image](https://github.com/user-attachments/assets/150ecabb-f542-4af1-9e36-275778fe1ce0)

### Multiple Test Environments
Multiple test environments can be put in place. This project will include testing on Windows and testing on a emulated ARM environment. 

# Links
WinLibs standalone build of GCC and MinGW-w64 for Windows - https://winlibs.com/

Arm GNU Toolchain Downloads - [https://developer.arm.com/](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

Unity Assertion Reference - [docs/UnityAssertionsReference.md](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md)

QEMU -  https://www.qemu.org/
