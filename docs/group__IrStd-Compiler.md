# IrStd-Compiler
Set of macro to test the compiler, compilation options and platform. 
# Member(s)
- [IRSTD_STATIC_INFO(expr)](#group__IrStd-Compiler_1ga0a9629b7ef521d5612a9a5aa5cbca9e1)
- [IRSTD_IS_PLATFORM_WINDOWS()](#group__IrStd-Compiler_1ga9a9a4d43401e3fef37349e0cfd69259e)
- [IRSTD_IS_PLATFORM_LINUX()](#group__IrStd-Compiler_1ga928cf012434ed45f82113b8046b35fda)
- [IRSTD_COMPILER_STRING()](#group__IrStd-Compiler_1ga45668bf0af2725cfdd2477a0841d6e30)
- [IRSTD_IS_PLATFORM_APPLE()](#group__IrStd-Compiler_1ga794d05dfc9993e9b8ea4cc7d3d24f08b)
- [_IRSTD_IS_COMPILER_GCC1(x)](#group__IrStd-Compiler_1ga1a8012173769615ad283a6e2638ab05b)
- [_IRSTD_IS_COMPILER_GCC0()](#group__IrStd-Compiler_1gaf6bc532f9255b4a32fa09ebad497be79)
- [IRSTD_IS_COMPILER_GCC(...)](#group__IrStd-Compiler_1ga489c4032bf55077a2800aaf40bebfb7d)
- [IRSTD_IS_RELEASE()](#group__IrStd-Compiler_1ga6e00bad4b006508517a3995c55d1db06)
- [IRSTD_FILENAME()](#group__IrStd-Compiler_1ga37af8a837356ac986e8bf1ce1619078f)
- [IRSTD_COMPILER_VERSION()](#group__IrStd-Compiler_1ga20d966f4f5f0dcb84790aa9f9134285c)
- [IRSTD_IS_COMPILER(name, ...)](#group__IrStd-Compiler_1gacf067849c6d6ced0445df4e6924fb2a1)
- [IRSTD_STATIC_ERROR(expr)](#group__IrStd-Compiler_1ga3943a5b899adf751cc5eb2b52cd39dc6)
- [IRSTD_IS_PLATFORM(name, ...)](#group__IrStd-Compiler_1ga8c5f5034187b224649b3d806940c20e2)
- [IRSTD_IS_DEBUG()](#group__IrStd-Compiler_1gaed52dea974715bf051a8d37185203d24)
- [_IRSTD_IS_COMPILER_GCC2(x, y)](#group__IrStd-Compiler_1ga55352a0c35564bdacb87286d1604ee9b)
- [IRSTD_PLATFORM_STRING()](#group__IrStd-Compiler_1gae5fa8610556a46edac493807337374d9)
- [IRSTD_PLATFORM_BIT()](#group__IrStd-Compiler_1gab96a133afd6b19597c40ed4f04127516)
- [IRSTD_IS_COMPILER_DOXYGEN(...)](#group__IrStd-Compiler_1ga80ab00eafa64c32619ea34b6d3acd165)
- [IRSTD_PLATFORM_NAME()](#group__IrStd-Compiler_1ga20fe904500ab32ced7353d1a6f0dd699)
- [_IRSTD_IS_COMPILER_GCC3(x, y, z)](#group__IrStd-Compiler_1ga1bb2f73763d45b1005e3ed1cfa5ddab0)
- [IRSTD_COMPILER_NAME()](#group__IrStd-Compiler_1gafb132b85c824dd1395e8d5c217fcc12b)
## Compiler


<a name='group__IrStd-Compiler_1gacf067849c6d6ced0445df4e6924fb2a1'></a> `public` **IRSTD_IS_COMPILER(name, ...)**

Check if the name and version of the compiler used. 
<br/>**Parameters**
<br/>`name` The name of the compiler, "GCC" for example. 
<br/>`...` (optional) Extra parameters to be passed to the IRSTD_IS_COMPILER_##name macro 
<br/>




<a name='group__IrStd-Compiler_1ga489c4032bf55077a2800aaf40bebfb7d'></a> `public` **IRSTD_IS_COMPILER_GCC(...)**

Check if the compiler used is GCC and, optionaly, if its version is greater or equal to the one supplied into arguments. 
<br/>**Parameters**
<br/>`major` (optional) Major version 
<br/>`minor` (optional) Minor version 
<br/>`patch` (optional) Patch level 
<br/>




<a name='group__IrStd-Compiler_1ga80ab00eafa64c32619ea34b6d3acd165'></a> `public` **IRSTD_IS_COMPILER_DOXYGEN(...)**

Check if the compiler (preprocessor in this case) used is DOXYGEN. 




<a name='group__IrStd-Compiler_1gafb132b85c824dd1395e8d5c217fcc12b'></a> `public` **IRSTD_COMPILER_NAME()**

The name of the compiler used, example "GCC". 




<a name='group__IrStd-Compiler_1ga20d966f4f5f0dcb84790aa9f9134285c'></a> `public` **IRSTD_COMPILER_VERSION()**

The version of the compiler, example "5.3.1". 




<a name='group__IrStd-Compiler_1ga45668bf0af2725cfdd2477a0841d6e30'></a> `public` **IRSTD_COMPILER_STRING()**

Descriptive string representing the compiler used. It includes the compiler name and its version. 


## Platform


<a name='group__IrStd-Compiler_1ga8c5f5034187b224649b3d806940c20e2'></a> `public` **IRSTD_IS_PLATFORM(name, ...)**

Check if the compilation is performed on a specific platform. 
<br/>**Parameters**
<br/>`name` The name of the platform, LINUX for example. 
<br/>




<a name='group__IrStd-Compiler_1ga928cf012434ed45f82113b8046b35fda'></a> `public` **IRSTD_IS_PLATFORM_LINUX()**

Check if the compilation is performed on a Linux platform. 




<a name='group__IrStd-Compiler_1ga9a9a4d43401e3fef37349e0cfd69259e'></a> `public` **IRSTD_IS_PLATFORM_WINDOWS()**

Check if the compilation is performed on a Windows platform. 




<a name='group__IrStd-Compiler_1ga794d05dfc9993e9b8ea4cc7d3d24f08b'></a> `public` **IRSTD_IS_PLATFORM_APPLE()**

Check if the compilation is performed on an Apple platform. 




<a name='group__IrStd-Compiler_1ga20fe904500ab32ced7353d1a6f0dd699'></a> `public` **IRSTD_PLATFORM_NAME()**

The name of the platform used for the compilation. 




<a name='group__IrStd-Compiler_1gab96a133afd6b19597c40ed4f04127516'></a> `public` **IRSTD_PLATFORM_BIT()**

The architecture bit width of the processor used for the compilation. 




<a name='group__IrStd-Compiler_1gae5fa8610556a46edac493807337374d9'></a> `public` **IRSTD_PLATFORM_STRING()**

Descriptive string representing the platform used. It includes the platform name and the bit width. 


## Build Type


<a name='group__IrStd-Compiler_1gaed52dea974715bf051a8d37185203d24'></a> `public` **IRSTD_IS_DEBUG()**

Defined when the build is for debug. 




<a name='group__IrStd-Compiler_1ga6e00bad4b006508517a3995c55d1db06'></a> `public` **IRSTD_IS_RELEASE()**

Defined when the build is for release. 


## Messaging


<a name='group__IrStd-Compiler_1ga0a9629b7ef521d5612a9a5aa5cbca9e1'></a> `public` **IRSTD_STATIC_INFO(expr)**

Display an informal message at compilation time and continue compilation. 




<a name='group__IrStd-Compiler_1ga3943a5b899adf751cc5eb2b52cd39dc6'></a> `public` **IRSTD_STATIC_ERROR(expr)**

Display an error at compilation time and breaks compilation. 




<a name='group__IrStd-Compiler_1ga37af8a837356ac986e8bf1ce1619078f'></a> `public` **IRSTD_FILENAME()**

Get the current file's name. 


## Define(s)


<a name='group__IrStd-Compiler_1gaf6bc532f9255b4a32fa09ebad497be79'></a> `public` **_IRSTD_IS_COMPILER_GCC0()**





<a name='group__IrStd-Compiler_1ga1a8012173769615ad283a6e2638ab05b'></a> `public` **_IRSTD_IS_COMPILER_GCC1(x)**





<a name='group__IrStd-Compiler_1ga55352a0c35564bdacb87286d1604ee9b'></a> `public` **_IRSTD_IS_COMPILER_GCC2(x, y)**





<a name='group__IrStd-Compiler_1ga1bb2f73763d45b1005e3ed1cfa5ddab0'></a> `public` **_IRSTD_IS_COMPILER_GCC3(x, y, z)**



