-- red-green-yellow button

-- gpio pins - https://iotbytes.wordpress.com/nodemcu-pinout/
pin1   = 1 -- color1
pin2   = 2 -- color2
btnPin = 3 -- keypress trigger

-- pin ops
function pinLow(...)
	for i = 1, #arg do
		--print("gpio# i = ", arg[i])
		if i ~= nil then
			gpio.write(arg[i], gpio.LOW)
		end
	end
end
function pinHigh(...)
	for i = 1, #arg do
		--print("gpio# i = ", arg[i])
		if i ~= nil then
			gpio.write(arg[i], gpio.HIGH)
		end
	end
end

-- colors
function led(clr)
	if clr ~= nil then
		if clr == "off" or clr == 0 then
			pinHigh(pin1, pin2)
		elseif clr == "yellow" or clr == 1 then
			pinLow(pin1, pin2)
		elseif clr == "green" or clr == 2 then
			pinLow(pin1)
			pinHigh(pin2)
		elseif clr == "red" or clr == 3 then
			pinLow(pin2)
			pinHigh(pin1)
		else
			print("[E] no such color")
		end
	end
end


-- this function is called on gpio trigger
-- delay, and the timer is for safety, because a normal interrupt calls
-- the function too many times
function pressAct()
	delay = 0
	x = tmr.now()
	if x > delay then
		delay = tmr.now() + 100000
		-- let's say it's doing something
		-- like waiting for the http request to finish
		led("yellow")
		print("(*) BOOM!")
	end

	print("Current state: ", state)
	if state == "on" then
		state = "off"
		print("Setting state: ", state)
		led("red")
	elseif state == "off" then
		state = "on"
		print("Setting state: ", state)
		led("green")
	else
		print("borked, Setting state: ", state)
		led("yellow")
		state = "off"
	end
end

state = "on"
led("green")

-- initialize our gpio trigger
gpio.mode(btnPin, gpio.INT, gpio.PULLUP)
--gpio.trig(btnPin, "up", pressAct)
gpio.trig(btnPin, "down", pressAct)
