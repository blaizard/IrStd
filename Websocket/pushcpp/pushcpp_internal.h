#pragma once

#include "pushcpp.h"

#include <stdint.h>
#include <assert.h>

#include <sstream>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>

#include "../../Assert.hpp"
#include "../../Topic.hpp"
#include "../../Logger.hpp"
#include "../../Json.hpp"

IRSTD_TOPIC_USE(IrStd, Websocket);

#define DEBUG(stream) IRSTD_LOG_DEBUG(IRSTD_TOPIC(IrStd, Websocket), stream)

#include "easywsclient.hpp"

using easywsclient::WebSocket;

using namespace std;
