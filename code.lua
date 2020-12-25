Lamp1Status = false;
Lamp2Status = false;
Lamp3Status = false;
Lamp4Status = false;
Lamp1BlinkingStatus = false;

Lamp1PiN = 14
Lamp2PiN = 12
Lamp3PiN = 16
Lamp4PiN = 13

TOUCH1PIN = 2
TOUCH2PIN = 0
TOUCH3PIN = 5
TOUCH4PIN = 4

function Setup()
    pinMode(Lamp1PiN, OUTPUT)
    pinMode(Lamp2PiN, OUTPUT)
    pinMode(Lamp3PiN, OUTPUT)
    pinMode(Lamp4PiN, OUTPUT)
	
	pinMode(TOUCH1PIN, INPUT)
	pinMode(TOUCH2PIN, INPUT)
	pinMode(TOUCH3PIN, INPUT)
	pinMode(TOUCH4PIN, INPUT)
	
end

function Loop()
	
    if Lamp1BlinkingStatus then
        digitalWrite(Lamp1PiN, HIGH);
        delay(500);
        digitalWrite(Lamp1PiN, LOW);
        delay(500);
    end
	
	
end

--#api
function get_devices()
    responseHtml("{\"swith\":[{\"id\":1,\"title\": \"Bedroom\",\"control_url\":\"http://192.168.1.61/api/lamp1\",\"check\":\"http://192.168.1.61/api/getstatus1\"},{\"id\":2,\"title\": \"Kitchen\",\"control_url\":\"http://192.168.1.61/api/lamp2\",\"check\":\"http://192.168.1.61/api/getstatus2\"},{\"id\":3,\"title\": \"Hall1\",\"control_url\":\"http://192.168.1.61/api/lamp3\",\"check\":\"http://192.168.1.61/api/getstatus3\"},{\"id\":4,\"title\": \"Hall2\",\"control_url\":\"http://192.168.1.61/api/lamp4\",\"check\":\"http://192.168.1.61/api/getstatus4\"}]}")
end

--#api
function getJson()
    -- json = require "json"
    -- local t = { x = 10, y = 20, z = 30 }
    -- json.encode(t)
    local jsonraw = '{"test":[1,2,3,4]}'
local jsonparse = json.decode( jsonraw )

   responseHtml("\"json\":\"".. jsonparse)
end

--#api
function getstatus1()
    lamp1StatusString = "true"
    if Lamp1Status then
        lamp1StatusString = "true"
    else
        lamp1StatusString = "false"
    end
   responseHtml("{\"checked\":" .. lamp1StatusString .. "}")
end

--#api
function getstatus2()
    lamp2StatusString = "false"
    if Lamp2Status then
        lamp2StatusString = "true"
    else
        lamp2StatusString = "false"
    end
    responseHtml("{\"checked\":" .. lamp2StatusString .. "}")
end

--#api
function getstatus3()
    lamp3StatusString = "false"
    if Lamp3Status then
        lamp3StatusString = "true"
    else
        lamp3StatusString = "false"
    end
    responseHtml("{\"checked\":" .. lamp3StatusString .. "}")
end

--#api
function getstatus4()
    lamp4StatusString = "false"
    if Lamp4Status then
        lamp4StatusString = "true"
    else
        lamp4StatusString = "false"
    end
    responseHtml("{\"checked\":" .. lamp4StatusString .. "}")
end

--#api
function lamp1()
    if Lamp1Status then
     Lamp1Status = false
     digitalWrite(Lamp1PiN, LOW);
     responseHtml("{\"checked\":false}") 
    else
     Lamp1Status = true
        digitalWrite(Lamp1PiN, HIGH);
        responseHtml("{\"checked\":true}") 
    end
 end

 --#api
function lamp2()
    if Lamp2Status then
     Lamp2Status = false
     digitalWrite(Lamp2PiN, LOW);
     responseHtml("{\"checked\":false}") 
    else
     Lamp2Status = true
     digitalWrite(Lamp2PiN, HIGH);
     responseHtml("{\"checked\":true}") 
    end
 end

 --#api
function lamp3()
    if Lamp3Status then
     Lamp3Status = false
     digitalWrite(Lamp3PiN, LOW);
     responseHtml("{\"checked\":false}") 
    else
     Lamp3Status = true
     digitalWrite(Lamp3PiN, HIGH);
     responseHtml("{\"checked\":true}") 
    end
 end

 --#api
function lamp4()
    if Lamp4Status then
     Lamp4Status = false
     digitalWrite(Lamp4PiN, LOW);
     responseHtml("{\"checked\":false}") 
    else
     Lamp4Status = true
     digitalWrite(Lamp4PiN, HIGH);
     responseHtml("{\"checked\":true}") 
    end
 end