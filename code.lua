Lamp1Status = false;
Lamp1BlinkingStatus = false;

Lamp1PiN = 14

function Setup()
	pinMode(Lamp1PiN, OUTPUT)
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
function Lamp1()
    if Lamp1Status then
        Lamp1Status = false
        digitalWrite(Lamp1PiN, LOW);
    else
        Lamp1Status = true
        digitalWrite(Lamp1PiN, HIGH);
    end
	responseHtml(Lamp1Status)
end

--#api
function Lamp1blink()
    if Lamp1BlinkingStatus then
        Lamp1BlinkingStatus = false
    else
        Lamp1BlinkingStatus = true
    end
	responseHtml('status = ' + Lamp1BlinkingStatus)
end