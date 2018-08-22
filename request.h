#pragma once

#include "stdafx.h"
#include "Response.h"

void initRequest();

std::shared_ptr<Response> startRequest(const char *uriStr);
