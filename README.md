# Callback: 
### A simple Signals and Slots implementation for Arduino

Signals and slots is a handy pattern for situations where one or more objects need to be informed of changes occuring elsewhere in the program much like a callback. Unlike callbacks, with this library it's possible to have multiple functions called on the occurence of an event. Additionally, those functions can be methods of an object as well as standard functions.

To make use of this, define a Signal for an event that might need monitoring like:
```Arduino
Signal<float> highTempMeasured;
```

Signal is a template class that expects the type of signals it'll later pass to its Slots. In this case it's a temperature so a float is appropriate. It has a second optional template parameter which is the number of slots it can support, the default is 8.

Now connect to the signal with Slots, recepticles and handlers for the events emitted by the signal. Slots can be made from free standing functions like so:
```Arduino
void RaiseHighTempAlert(float temperature)
{
	Serial.println("High temp measured!");
}
```

Or methods of objects like so:
```Arduino
class AlertsManager
{
	float highestTempRecorded;
public:
	OnHighTempMeasured(float temperature)
	{
		if(temperature > highestTempRecorded)
			highestTempRecorded = temperature;
	}
} alerter;
```

For free standing functions we define a slot like so:
```Arduino
FunctionSlot<float> funcSlot(RaiseHighTempAlert);
```

And for methods, like this:
```Arduino
MethodSlot<Foo, float> methSlot(&alerter, &AlertsManager::OnHighTempMeasured);
```

Both can be connected to the highTempMeasured signal if their function parameters match that of the signal (and the type supplied to the tempate during declaration). In this case they do so:
```Arduino
highTempMeasured.attach(funcSlot);
highTempMeasured.attach(methSlot);
```

Now calling fire and passing it say, 54.7 will call void RaiseHighTempAlert(54.7) and alerter.OnHighTempMeasured(54.7).

Slots can also be disconnected from signals using their detach method:
```Arduino
highTempMeasured.detach(funcSlot);
highTempMeasured.detach(methSlot);
```

Unlike some Signals and Slot libraries, this one does not support multiple parameters which simplifies the implementation significantly. If multiple parameters are necessary just define a struct containing everything that's needed and pass that through instead.
