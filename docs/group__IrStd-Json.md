# IrStd-Json
Json decoder and encoder, based on RapidJson. 
# Member(s)
- [void IrStd::Json::toStream(std::ostream &out) const](#group__IrStd-Json_1ga229815547ccee142b77b01d6abdc745d)
- [const IrStd::Json:: Object& IrStd::Json::getObject(A &&... a) const](#group__IrStd-Json_1ga62a2f4f8ad5f2b0722064978f91583f4)
- [IrStd::Json:: Null& IrStd::Json::getNull(A &&... a)](#group__IrStd-Json_1ga3f12f4c0e3b702ce0b63f81c511d0fed)
- [bool IrStd::Json::isBool(A &&... a) const noexcept](#group__IrStd-Json_1ga91aaa8cbfb3e52851719ddba5701c5f4)
- [const IrStd::Json:: Number& IrStd::Json::getNumber(A &&... a) const](#group__IrStd-Json_1ga28cfc980fd53a4101afaf8bba6cf363a)
- [IrStd::Json:: Object& IrStd::Json::getObject(A &&... a)](#group__IrStd-Json_1ga25ce90ebf060e38eea7bf14e0da2bb20)
- [std::string IrStd::Json::serialize() const](#group__IrStd-Json_1ga475f834b35e64f43f05cdd7ac365a9a0)
- [IrStd::Json::Json(const char *const pStr)](#group__IrStd-Json_1ga28fd7d91af8009ffb6dd8d824cc9955c)
- [bool IrStd::Json::isObject(A &&... a) const noexcept](#group__IrStd-Json_1ga90d8d58ef9c650fa3e0e7ba925c0812a)
- [bool IrStd::Json::isNumber(A &&... a) const noexcept](#group__IrStd-Json_1ga6c78e7d7a352fd6ba1f1bef19afa6276)
- [bool IrStd::Json::is(A &&... a) const noexcept](#group__IrStd-Json_1ga49a4d102d001cff62fa7554fee09ad52)
- [const IrStd::Json:: Null& IrStd::Json::getNull(A &&... a) const](#group__IrStd-Json_1ga1af587bc16351a66be1e0f74aa964664)
- [bool IrStd::Json::isArray(A &&... a) const noexcept](#group__IrStd-Json_1ga515e07e00845a46a46e6092d4577c5ce)
- [IrStd::Json:: Number& IrStd::Json::getNumber(A &&... a)](#group__IrStd-Json_1ga484f41822c138a705a10dbd87b0a5721)
- [IrStd::Json:: Bool& IrStd::Json::getBool(A &&... a)](#group__IrStd-Json_1ga94f8bd6b6b346ecb0d8ebcb64f0caa1c)
- [const IrStd::Json:: Bool& IrStd::Json::getBool(A &&... a) const](#group__IrStd-Json_1ga17ff60b8eca9557e80989178fb5395c2)
- [const IrStd::Json:: Array& IrStd::Json::getArray(A &&... a) const](#group__IrStd-Json_1ga82b9b90a122fb1024a5067be4fd61a6c)
- [IrStd::Json:: String& IrStd::Json::getString(A &&... a)](#group__IrStd-Json_1gab6f525ad84968f992d5f2593c566feb4)
- [bool IrStd::Json::isNull(A &&... a) const noexcept](#group__IrStd-Json_1gafd45556462b6e957cb89ab60a70b4182)
- [IrStd::Json::Json(const Json &json)=delete](#group__IrStd-Json_1ga39b5bc456eebd982382ac8cdcd3f7fc0)
- [void IrStd::Json::merge(const Json &json)](#group__IrStd-Json_1gace5e463e5ded8399fc0da0c347716889)
- [bool IrStd::Json::isString(A &&... a) const noexcept](#group__IrStd-Json_1gab00bd32416773a2ed3a66890f80de505)
- [IrStd::Json::Json(Json &&json)](#group__IrStd-Json_1ga4a4332cc11660cc1fff546e10836b6bb)
- [IrStd::Json::Json(const Type::Gson::Map &gson)](#group__IrStd-Json_1ga7b31b93b7dc5703eceaf4610b35ffc7d)
- [IrStd::Json:: Array& IrStd::Json::getArray(A &&... a)](#group__IrStd-Json_1ga57aafdac35d6f1fc66c934986ff8d29c)
- [void IrStd::Json::clear()](#group__IrStd-Json_1ga24b378ae4a99c15b9af9babf7f0bdd66)
- [IrStd::Json::Json()](#group__IrStd-Json_1gae15f2d43b27546531e9f30b91d05be03)
- [const IrStd::Json:: String& IrStd::Json::getString(A &&... a) const](#group__IrStd-Json_1ga6bbc8a14c8a0dee1114f956e98387268)
## Constructors


<a name='group__IrStd-Json_1gae15f2d43b27546531e9f30b91d05be03'></a> `public` **IrStd::Json::Json()**

Construct an empty Json object. 




<a name='group__IrStd-Json_1ga28fd7d91af8009ffb6dd8d824cc9955c'></a> `public` `explicit` **IrStd::Json::Json(const char \*const pStr)**

Construct a Json object from a string. 
<br/>**Parameters**
<br/>`pStr` Json serialized string 
<br/>




<a name='group__IrStd-Json_1ga7b31b93b7dc5703eceaf4610b35ffc7d'></a> `public` `explicit` **IrStd::Json::Json(const Type::Gson::Map &gson)**

Construct a Json object from a IrStd::Gson object, This gives the possiblity to create an Json object in a human readable way. 
<br/>**Parameters**
<br/>`gson` Gson map object 
<br/>




<a name='group__IrStd-Json_1ga39b5bc456eebd982382ac8cdcd3f7fc0'></a> `public` **IrStd::Json::Json(const Json &json)=delete**

Delete the copy constructor for performance concerns. 




<a name='group__IrStd-Json_1ga4a4332cc11660cc1fff546e10836b6bb'></a> `public` **IrStd::Json::Json(Json &&json)**

Move constructor. 


## Accessors


<a name='group__IrStd-Json_1ga90d8d58ef9c650fa3e0e7ba925c0812a'></a> `public` `template <class... A>` `inline` **bool IrStd::Json::isObject(A &&... a) const noexcept**

Check that the element of type Object exists. 




<a name='group__IrStd-Json_1ga25ce90ebf060e38eea7bf14e0da2bb20'></a> `public` `template <class... A>` `inline` **IrStd::Json:: Object& IrStd::Json::getObject(A &&... a)**

Retrieve the element of type Object. 




<a name='group__IrStd-Json_1ga62a2f4f8ad5f2b0722064978f91583f4'></a> `public` `template <class... A>` `inline` **const IrStd::Json:: Object& IrStd::Json::getObject(A &&... a) const**

Retrieve the element of type Object. 




<a name='group__IrStd-Json_1ga515e07e00845a46a46e6092d4577c5ce'></a> `public` `template <class... A>` `inline` **bool IrStd::Json::isArray(A &&... a) const noexcept**

Check that the element of type Array exists. 




<a name='group__IrStd-Json_1ga57aafdac35d6f1fc66c934986ff8d29c'></a> `public` `template <class... A>` `inline` **IrStd::Json:: Array& IrStd::Json::getArray(A &&... a)**

Retrieve the element of type Array. 




<a name='group__IrStd-Json_1ga82b9b90a122fb1024a5067be4fd61a6c'></a> `public` `template <class... A>` `inline` **const IrStd::Json:: Array& IrStd::Json::getArray(A &&... a) const**

Retrieve the element of type Array. 




<a name='group__IrStd-Json_1gab00bd32416773a2ed3a66890f80de505'></a> `public` `template <class... A>` `inline` **bool IrStd::Json::isString(A &&... a) const noexcept**

Check that the element of type String exists. 




<a name='group__IrStd-Json_1gab6f525ad84968f992d5f2593c566feb4'></a> `public` `template <class... A>` `inline` **IrStd::Json:: String& IrStd::Json::getString(A &&... a)**

Retrieve the element of type String. 




<a name='group__IrStd-Json_1ga6bbc8a14c8a0dee1114f956e98387268'></a> `public` `template <class... A>` `inline` **const IrStd::Json:: String& IrStd::Json::getString(A &&... a) const**

Retrieve the element of type String. 




<a name='group__IrStd-Json_1ga6c78e7d7a352fd6ba1f1bef19afa6276'></a> `public` `template <class... A>` `inline` **bool IrStd::Json::isNumber(A &&... a) const noexcept**

Check that the element of type Number exists. 




<a name='group__IrStd-Json_1ga484f41822c138a705a10dbd87b0a5721'></a> `public` `template <class... A>` `inline` **IrStd::Json:: Number& IrStd::Json::getNumber(A &&... a)**

Retrieve the element of type Number. 




<a name='group__IrStd-Json_1ga28cfc980fd53a4101afaf8bba6cf363a'></a> `public` `template <class... A>` `inline` **const IrStd::Json:: Number& IrStd::Json::getNumber(A &&... a) const**

Retrieve the element of type Number. 




<a name='group__IrStd-Json_1ga91aaa8cbfb3e52851719ddba5701c5f4'></a> `public` `template <class... A>` `inline` **bool IrStd::Json::isBool(A &&... a) const noexcept**

Check that the element of type Bool exists. 




<a name='group__IrStd-Json_1ga94f8bd6b6b346ecb0d8ebcb64f0caa1c'></a> `public` `template <class... A>` `inline` **IrStd::Json:: Bool& IrStd::Json::getBool(A &&... a)**

Retrieve the element of type Bool. 




<a name='group__IrStd-Json_1ga17ff60b8eca9557e80989178fb5395c2'></a> `public` `template <class... A>` `inline` **const IrStd::Json:: Bool& IrStd::Json::getBool(A &&... a) const**

Retrieve the element of type Bool. 




<a name='group__IrStd-Json_1gafd45556462b6e957cb89ab60a70b4182'></a> `public` `template <class... A>` `inline` **bool IrStd::Json::isNull(A &&... a) const noexcept**

Check that the element of type Null exists. 




<a name='group__IrStd-Json_1ga3f12f4c0e3b702ce0b63f81c511d0fed'></a> `public` `template <class... A>` `inline` **IrStd::Json:: Null& IrStd::Json::getNull(A &&... a)**

Retrieve the element of type Null. 




<a name='group__IrStd-Json_1ga1af587bc16351a66be1e0f74aa964664'></a> `public` `template <class... A>` `inline` **const IrStd::Json:: Null& IrStd::Json::getNull(A &&... a) const**

Retrieve the element of type Null. 


## Function(s)


<a name='group__IrStd-Json_1ga229815547ccee142b77b01d6abdc745d'></a> `public` **void IrStd::Json::toStream(std::ostream &out) const**

Print a human readable representation of the json object (for debugging purpose) 
<br/>**Parameters**
<br/>`out` The stream where to print the object 
<br/>




<a name='group__IrStd-Json_1ga475f834b35e64f43f05cdd7ac365a9a0'></a> `public` **std::string IrStd::Json::serialize() const**

Serialize the Json object into a string. 
<br/>**Return**
<br/>A string containing the serialized Json 




<a name='group__IrStd-Json_1ga24b378ae4a99c15b9af9babf7f0bdd66'></a> `public` **void IrStd::Json::clear()**

Clear the content of the Json object. 




<a name='group__IrStd-Json_1gace5e463e5ded8399fc0da0c347716889'></a> `public` **void IrStd::Json::merge(const Json &json)**

Merge a json object into this one. 
<br/>**Parameters**
<br/>`json` The Json object to be merged. 
<br/>




<a name='group__IrStd-Json_1ga49a4d102d001cff62fa7554fee09ad52'></a> `public` `template <class... A>` `inline` **bool IrStd::Json::is(A &&... a) const noexcept**

Assess if a member exists. 
<br/>**Parameters**
<br/>`...` List of keys to reach the element
<br/>
<br/>**Return**
<br/>true if the element exists, false otherwise 


