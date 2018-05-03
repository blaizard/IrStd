# IrStd-Assert
Set of assertion tests that can be used to test invariants. 
# Member(s)
- [_IRSTD_UNREACHABLE0()](#group__IrStd-Assert_1gaf5f07a1a6f2e832a885c5de43be69bdd)
- [IRSTD_ASSERT_NOLOG(...)](#group__IrStd-Assert_1ga0f3f07c5417b94f621262bba40ca468e)
- [_IRSTD_CRASH0()](#group__IrStd-Assert_1ga0d252341d9191eba379c62fb321bbe95)
- [IRSTD_ASSERT_VAR_TYPEOF(variable, ...)](#group__IrStd-Assert_1ga95410fb83456d3088c42284d9c1af623)
- [_IRSTD_CRASH_NOLOG2(topic, expr)](#group__IrStd-Assert_1ga395b4a98d661bc2b1fa0051ffece5efa)
- [IRSTD_CRASH_NOLOG(...)](#group__IrStd-Assert_1ga180eccd0ac07d026c2be3a0ac9225f07)
- [_IRSTD_ASSERT_NOLOG3(topic, condition, expr)](#group__IrStd-Assert_1ga07baff96ff2157f1f3855e17485f6661)
- [IRSTD_THROW_ASSERT(...)](#group__IrStd-Assert_1ga1b1e0f83ed5e073c2de0ea94789ca7aa)
- [_IRSTD_CRASH1(expr)](#group__IrStd-Assert_1ga57723feba716a33d54aa69bbe138ff4d)
- [_IRSTD_ASSERT2(condition, expr)](#group__IrStd-Assert_1ga31501466da77e68a17b1415a033334ce)
- [IRSTD_ASSERT_NO_CONCURRENCY()](#group__IrStd-Assert_1ga74bf657c7777e290c5be483ae22138ca)
- [IRSTD_ASSERT_VAR_CHILDOF(variable, typeBase)](#group__IrStd-Assert_1ga2525b587700aa4b14b51a4e32f6b72b3)
- [_IRSTD_CRASH2(topic, expr)](#group__IrStd-Assert_1gab9a00406ebe7d24d1784bdfe89cf9db8)
- [_IRSTD_ASSERT_NOLOG1(condition)](#group__IrStd-Assert_1ga2d83eeab2c3133252645fc370f276d0d)
- [_IRSTD_ASSERT3(topic, condition, expr)](#group__IrStd-Assert_1ga7b0a6e482569895a8c7d8546f2f78dfe)
- [_IRSTD_UNREACHABLE1(topic)](#group__IrStd-Assert_1gac086e123edcc4d1ce7f438e1d2872a2e)
- [IRSTD_UNREACHABLE(...)](#group__IrStd-Assert_1gaa6c8dd6680808c793be03fad6c8bcdc3)
- [IRSTD_ASSERT(...)](#group__IrStd-Assert_1ga270fc001003cb74293eac358d0c2cc65)
- [_IRSTD_THROW_ASSERT1(condition)](#group__IrStd-Assert_1ga81afbdb693288e8701aab2e64051b31d)
- [IRSTD_ASSERT_TYPEOF(type, ...)](#group__IrStd-Assert_1gaf2c862fde9ace19f1057e2f50c25a17f)
- [_IRSTD_UNREACHABLE2(topic, expr)](#group__IrStd-Assert_1ga7efbfbb1799dd210ee74510b2976c988)
- [_IRSTD_CRASH_NOLOG1(expr)](#group__IrStd-Assert_1ga612a199428ae7ea6038b5328a5b0bb6d)
- [_IRSTD_CRASH_NOLOG0()](#group__IrStd-Assert_1ga78aa3b858926bd6f49d279127d1ad411)
- [_IRSTD_THROW_ASSERT2(condition, expr)](#group__IrStd-Assert_1ga9891170bedb72e7938f79a8249699743)
- [_IRSTD_ASSERT_NOLOG2(condition, expr)](#group__IrStd-Assert_1ga9983b9ebd4168fc5b4d4f9fb0ba06794)
- [IRSTD_ASSERT_CHILDOF(typeChild, typeBase)](#group__IrStd-Assert_1ga496aa5d736d944c3cd935b5c6568b65a)
- [IRSTD_CRASH(...)](#group__IrStd-Assert_1ga8cbd4487fa1f5b2f6499150928ff02de)
- [_IRSTD_THROW_ASSERT3(topic, condition, expr)](#group__IrStd-Assert_1ga74119d1f45f24015c1ac41a05872a364)
- [_IRSTD_ASSERT1(condition)](#group__IrStd-Assert_1ga0d8b80aa2c51544b82f5016c2424d6ea)
## Assertions


<a name='group__IrStd-Assert_1ga270fc001003cb74293eac358d0c2cc65'></a> `public` **IRSTD_ASSERT(...)**

Ensures value evaluates to true and log the failure (if any) 
If the value evaluates to false, a crash is immediatly triggered by calling [IRSTD_CRASH](group__IrStd-Assert.md#group__IrStd-Assert_1ga8cbd4487fa1f5b2f6499150928ff02de) .
<br/>**Parameters**
<br/>`topic` (optional) The topic to which this assertion applies. This must be a topic of type IrStd::TopicImpl . 
<br/>`value` The value to be tested 
<br/>`message` (optional) A descriptive message that will be displayed in case the assertion fails. 
<br/>




<a name='group__IrStd-Assert_1ga0f3f07c5417b94f621262bba40ca468e'></a> `public` **IRSTD_ASSERT_NOLOG(...)**

Ensures value evaluates to true and print to std::cerr the failure (if any) 
If the value evaluates to false, a crash is immediatly triggered by calling [IRSTD_CRASH_NOLOG](group__IrStd-Assert.md#group__IrStd-Assert_1ga180eccd0ac07d026c2be3a0ac9225f07) .
<br/>**Parameters**
<br/>`topic` (optional) The topic to which this assertion applies. This must be a topic of type IrStd::TopicImpl . 
<br/>`value` The value to be tested 
<br/>`message` (optional) A descriptive message that will be displayed in case the assertion fails. 
<br/>




<a name='group__IrStd-Assert_1ga1b1e0f83ed5e073c2de0ea94789ca7aa'></a> `public` **IRSTD_THROW_ASSERT(...)**

Ensures value evaluates to true and throw an exception in case of failure. 
<br/>**Parameters**
<br/>`topic` (optional) The topic to which this assertion applies. This must be a topic of type IrStd::TopicImpl . 
<br/>`value` The value to be tested 
<br/>`message` (optional) A descriptive message that will be displayed in case the assertion fails. 
<br/>




<a name='group__IrStd-Assert_1ga496aa5d736d944c3cd935b5c6568b65a'></a> `public` **IRSTD_ASSERT_CHILDOF(typeChild, typeBase)**

Assert that a type is a child of **typeBase**. 
This assertion is done at compile time, hence the error message will be displayed on std::cerr.
<br/>**Parameters**
<br/>`typeChild` The child or descendant type. 
<br/>`typeBase` The parent type. 
<br/>




<a name='group__IrStd-Assert_1ga2525b587700aa4b14b51a4e32f6b72b3'></a> `public` **IRSTD_ASSERT_VAR_CHILDOF(variable, typeBase)**

Assert that a variable is a child of **typeBase**. 
This assertion is done at compile time, hence the error message will be displayed on std::cerr.
<br/>**Parameters**
<br/>`variable` The child or descendant type. 
<br/>`typeBase` The parent type. 
<br/>




<a name='group__IrStd-Assert_1gaf2c862fde9ace19f1057e2f50c25a17f'></a> `public` **IRSTD_ASSERT_TYPEOF(type, ...)**

Assert that a type is of a specific type. 
This assertion is done at compile time, hence the error message will be displayed on std::cerr.
<br/>**Parameters**
<br/>`type` The type to be checked. 
<br/>`...` The different types. **type** must comply to at least one of them. 
<br/>




<a name='group__IrStd-Assert_1ga95410fb83456d3088c42284d9c1af623'></a> `public` **IRSTD_ASSERT_VAR_TYPEOF(variable, ...)**

Assert that a variable is of a specific type. 
This assertion is done at compile time, hence the error message will be displayed on std::cerr.
<br/>**Parameters**
<br/>`variable` The variable to be checked. 
<br/>`...` The different types. **variable** must comply to at least one of them. 
<br/>




<a name='group__IrStd-Assert_1ga74bf657c7777e290c5be483ae22138ca'></a> `public` **IRSTD_ASSERT_NO_CONCURRENCY()**

Ensures that a code path does not run into thread concurrency. 


## Termination


<a name='group__IrStd-Assert_1ga8cbd4487fa1f5b2f6499150928ff02de'></a> `public` **IRSTD_CRASH(...)**

Immediatly terminates the program and logs the crash. 
The crash will be logged using the default logger. See IrStd::Logger for more information.
<br/>**Parameters**
<br/>`topic` (optional) The topic to which this crash applies. This must be a topic of type IrStd::TopicImpl . 
<br/>`message` (optional) A descriptive message. 
<br/>




<a name='group__IrStd-Assert_1ga180eccd0ac07d026c2be3a0ac9225f07'></a> `public` **IRSTD_CRASH_NOLOG(...)**

Immediatly terminates the program and prints the crash over std::cerr. 
<br/>**Parameters**
<br/>`topic` (optional) The topic to which this crash applies. This must be a topic of type IrStd::TopicImpl . 
<br/>`message` (optional) A descriptive message. 
<br/>




<a name='group__IrStd-Assert_1gaa6c8dd6680808c793be03fad6c8bcdc3'></a> `public` **IRSTD_UNREACHABLE(...)**

Similar to [IRSTD_CRASH](group__IrStd-Assert.md#group__IrStd-Assert_1ga8cbd4487fa1f5b2f6499150928ff02de), this will immediatly terminates the program and logs the termination. 
This should be used when a line of code should never be reached.
<br/>**Parameters**
<br/>`topic` (optional) The topic to which this applies. This must be a topic of type IrStd::TopicImpl . 
<br/>


## Define(s)


<a name='group__IrStd-Assert_1ga0d8b80aa2c51544b82f5016c2424d6ea'></a> `public` **_IRSTD_ASSERT1(condition)**





<a name='group__IrStd-Assert_1ga31501466da77e68a17b1415a033334ce'></a> `public` **_IRSTD_ASSERT2(condition, expr)**





<a name='group__IrStd-Assert_1ga7b0a6e482569895a8c7d8546f2f78dfe'></a> `public` **_IRSTD_ASSERT3(topic, condition, expr)**





<a name='group__IrStd-Assert_1ga2d83eeab2c3133252645fc370f276d0d'></a> `public` **_IRSTD_ASSERT_NOLOG1(condition)**





<a name='group__IrStd-Assert_1ga9983b9ebd4168fc5b4d4f9fb0ba06794'></a> `public` **_IRSTD_ASSERT_NOLOG2(condition, expr)**





<a name='group__IrStd-Assert_1ga07baff96ff2157f1f3855e17485f6661'></a> `public` **_IRSTD_ASSERT_NOLOG3(topic, condition, expr)**





<a name='group__IrStd-Assert_1ga81afbdb693288e8701aab2e64051b31d'></a> `public` **_IRSTD_THROW_ASSERT1(condition)**





<a name='group__IrStd-Assert_1ga9891170bedb72e7938f79a8249699743'></a> `public` **_IRSTD_THROW_ASSERT2(condition, expr)**





<a name='group__IrStd-Assert_1ga74119d1f45f24015c1ac41a05872a364'></a> `public` **_IRSTD_THROW_ASSERT3(topic, condition, expr)**





<a name='group__IrStd-Assert_1gaf5f07a1a6f2e832a885c5de43be69bdd'></a> `public` **_IRSTD_UNREACHABLE0()**





<a name='group__IrStd-Assert_1gac086e123edcc4d1ce7f438e1d2872a2e'></a> `public` **_IRSTD_UNREACHABLE1(topic)**





<a name='group__IrStd-Assert_1ga7efbfbb1799dd210ee74510b2976c988'></a> `public` **_IRSTD_UNREACHABLE2(topic, expr)**





<a name='group__IrStd-Assert_1ga0d252341d9191eba379c62fb321bbe95'></a> `public` **_IRSTD_CRASH0()**





<a name='group__IrStd-Assert_1ga57723feba716a33d54aa69bbe138ff4d'></a> `public` **_IRSTD_CRASH1(expr)**





<a name='group__IrStd-Assert_1gab9a00406ebe7d24d1784bdfe89cf9db8'></a> `public` **_IRSTD_CRASH2(topic, expr)**





<a name='group__IrStd-Assert_1ga78aa3b858926bd6f49d279127d1ad411'></a> `public` **_IRSTD_CRASH_NOLOG0()**





<a name='group__IrStd-Assert_1ga612a199428ae7ea6038b5328a5b0bb6d'></a> `public` **_IRSTD_CRASH_NOLOG1(expr)**





<a name='group__IrStd-Assert_1ga395b4a98d661bc2b1fa0051ffece5efa'></a> `public` **_IRSTD_CRASH_NOLOG2(topic, expr)**



