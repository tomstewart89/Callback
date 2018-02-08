#include <Callback.h>

// The foo class will keep an eye out for a particular char
class Foo
{
  char watch;
public:
  Foo(char watchChar) { watch = watchChar; }
  
  void OnCharReceivedCheckForWatch(char c)
  { 
    if(c == watch)
      Serial.println("watchChar detected!"); 
  }
} foo('a');

// This function will print off any characters emitted to it
void OnCharReceivedPrint(char c) 
{ 
  Serial.println(c); 
}

// This signal will be emitted when we process characters
Signal<char> characterReceived;
  
void setup()
{
  Serial.begin(9600);

  // Define two types of slot to keep an eye on the characterRecieved signal
  MethodSlot<Foo,char> memFunSlot(&foo, &Foo::OnCharReceivedCheckForWatch);
  FunctionSlot<char> ptrSlot(OnCharReceivedPrint);

  // Attach them up
  characterReceived.attach(memFunSlot);
  characterReceived.attach(ptrSlot);
}

void loop()
{
  // Fire the characterReceived signal every time a character arrives on the serial port.
  while(Serial.available())
    characterReceived.fire(Serial.read());
}
