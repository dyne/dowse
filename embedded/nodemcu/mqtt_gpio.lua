-- combine this with the functions in mqtt_basic.lua

-- gpio pins - https://iotbytes.wordpress.com/nodemcu-pinout/
red  = 1
grn  = 2
blu  = 3

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
		if clr == "white" or clr == 1 then
			pinLow(red, grn, blu)
		elseif clr == "yellow" or clr == 2 then
			pinLow(red, grn)
			pinHigh(blu)
		elseif clr == "purple" or clr == 3 then
			pinLow(red, blu)
			pinHigh(grn)
		elseif clr == "red" or clr == 4 then
			pinLow(red)
			pinHigh(grn, blu)
		elseif clr == "cyan" or clr == 5 then
			pinLow(grn, blu)
			pinHigh(red)
		elseif clr == "green" or clr == 6 then
			pinLow(grn)
			pinHigh(red, blu)
		elseif clr == "blue" or clr == 7 then
			pinLow(blu)
			pinHigh(red, grn)
		elseif clr == "off" or clr == 8 then
			pinHigh(red, grn, blu)
		else
			print("[E] no such color")
		end
	end
end

function secFn()
	num = math.random(1,8)
	led(num)
end

-- some fun
function disco()
	print("(*) discoing...")
	-- https://nodemcu.readthedocs.io/en/master/en/modules/tmr/#tmralarm
	-- tmr.alarm params: timerID, miliseconds, mode, fn
	tmr.alarm(0, 200, 1, secFn)
	--tmr.stop(0)
end

--disco()
