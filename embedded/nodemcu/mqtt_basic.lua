-- this function helps us separate the mqtt message (think strtok(3) in C)
function tokenize(str, delim)
	local res = {}
	for tok in string.gmatch(str, ("[^%s]+"):format(delim)) do
		table.insert(res, tok)
	end
	return res
end

mqtt = mqtt.Client("nodemcu", 120)

mqtt:on("connect", function(con) print ("(*) connected") end)
mqtt:on("offline", function(con) print ("(*) offline") end)

-- just print the mqtt message nicely
function printTokenized(msg)
	print("(*) Event type:   ", msg[1])
	print("(*) Origin IP:    ", msg[2])
	print("(*) Hits so far   ", msg[3])
	print("(*) Epoch:        ", msg[4])
	print("(*) Domain:       ", msg[5])
	print("(*) TLD:          ", msg[6])
	print("(*) Domain group: ", msg[7])
end

-- domain groups
function groupHit(msg)
	local group = msg[7]
	if group ~= nil then
		if group == "facebook" then
			print("(!) facebook hit")
		elseif group == "google" then
			print("(!) google hit")
		end
	end
end

-- single domains in the same manner
function domainHit(msg)
	local domain = msg[5]
	if domain ~= nil then
		if string.find(domain, "google.com") then
			print("(!) google.com hit")
		end
	end
end


-- example usage of the table returned by tokenize()
mqtt:on("message", function(conn, topic, data)
	print(topic .. ":")
	if data ~= nil then
		local msg  = tokenize(data, ",")
		-- try different functions by uncommenting them
		printTokenized(msg)
		--groupHit(msg)
		--domainHit(msg)
	end
end)


-- connection settings
-- host, port
mqtt:connect("dowse.it", 1883, 0, function(conn)
	print("(*) connected")
	-- mqtt topic
	mqtt:subscribe("dns-query-channel", 0, function(conn)
		-- debug
		mqtt:publish("dns-query-channel", "nodemcu init", 0, 0, function(conn)
			print("(*) init sent")
		end)
	end)
end)
