#!/usr/bin/lua

local network = require"luci.model.network".init()
local accumulator = {}
local net = network:get_wifinet("wlan0")
  if net then
      local dev = net:get_device()
      if dev then  
        print(net::signal())
      end
  end
