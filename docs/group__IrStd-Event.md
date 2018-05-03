# IrStd-Event
Event mutex. 
# Member(s)
- [const char* IrStd::Event::getName() const noexcept](#group__IrStd-Event_1ga68ea1274870b84147246e6e6d72b2548)
- [static Event* IrStd::Event::waitForNexts(const uint64_t timeoutMs, Events &... events)](#group__IrStd-Event_1ga03792340819b253685f7a7360c795ef6)
- [size_t IrStd::Event::getCounter() const noexcept](#group__IrStd-Event_1gab13cf11e86033dcac8ccbca7b3ef8496)
- [void IrStd::Event::trigger() noexcept](#group__IrStd-Event_1ga8b67ef3abf51a158dd4a7d6515799b9b)
- [void IrStd::Event::reset() noexcept](#group__IrStd-Event_1ga37bc60949a6e9664671c581e861ee503)
- [IrStd::Event::Event(const char *const pName=nullptr)](#group__IrStd-Event_1ga8e2d3c2e8704545450b051435c3c312a)
- [size_t IrStd::Event::waitForAtLeast(const size_t nbEvents, const uint64_t timeoutMs) noexcept](#group__IrStd-Event_1gabaa483e1bc64559c9f90bbd02c326664)
- [size_t IrStd::Event::waitForNext(const uint64_t timeoutMs=0) const noexcept](#group__IrStd-Event_1gaa30ea69d94166742e6ad3c4e6e04d553)
## Function(s)


<a name='group__IrStd-Event_1ga8e2d3c2e8704545450b051435c3c312a'></a> `public` **IrStd::Event::Event(const char \*const pName=nullptr)**

Create an event of a specified name. 
<br/>**Parameters**
<br/>`pName` (optional) The name of the event 
<br/>




<a name='group__IrStd-Event_1gab13cf11e86033dcac8ccbca7b3ef8496'></a> `public` **size_t IrStd::Event::getCounter() const noexcept**

Get the current count of the event. 
<br/>**Return**
<br/>The number of times this event has been triggered 




<a name='group__IrStd-Event_1ga37bc60949a6e9664671c581e861ee503'></a> `public` **void IrStd::Event::reset() noexcept**

Will reset the state of the event and reset its counter. `this will trigger the event. `




<a name='group__IrStd-Event_1ga8b67ef3abf51a158dd4a7d6515799b9b'></a> `public` **void IrStd::Event::trigger() noexcept**

Trigger an event and increment the counter. 




<a name='group__IrStd-Event_1gaa30ea69d94166742e6ad3c4e6e04d553'></a> `public` **size_t IrStd::Event::waitForNext(const uint64_t timeoutMs=0) const noexcept**

Wait until the trigger or the timeout if specified. 
<br/>**Parameters**
<br/>`timeoutMs` Timeout in ms before this function returns
<br/>
<br/>**Return**
<br/>The counter after wakeup or 0 if the timeout has been reached. 




<a name='group__IrStd-Event_1gabaa483e1bc64559c9f90bbd02c326664'></a> `public` **size_t IrStd::Event::waitForAtLeast(const size_t nbEvents, const uint64_t timeoutMs) noexcept**

Wait for at least a minimum number of event. 
<br/>**Parameters**
<br/>`nbEvents` The number of events to wait for 
<br/>`timeoutMs` (Optional) Timeout in ms before this function returns
<br/>
<br/>**Return**
<br/>The counter after wakeup or 0 if the timeout has been reached. 




<a name='group__IrStd-Event_1ga03792340819b253685f7a7360c795ef6'></a> `public` `template <class ... Events>` `static` `inline` **static Event\* IrStd::Event::waitForNexts(const uint64_t timeoutMs, Events &... events)**

Wait until all the events passed into argument are triggered. 
<br/>**Parameters**
<br/>`timeoutMs` The maximum timeout in ms 
<br/>`events` The events
<br/>
<br/>**Return**
<br/>nullptr if all the events are triggered on time. Otherwise a pointer on the event that triggered the timeout, hence a non-null value means it has reached the timeout. 




<a name='group__IrStd-Event_1ga68ea1274870b84147246e6e6d72b2548'></a> `public` **const char\* IrStd::Event::getName() const noexcept**

Return the name associated with this event. 
<br/>**Return**
<br/>The name of the event. 


