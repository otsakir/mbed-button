#ifndef _BUTTON_H_
#define _BUTTON_H_

enum ButtonType {  // not really used
	BUTTON_PRESS, 	
	BUTTON_RELEASE, 
	BUTTON_HOLD, 
	BUTTON_ANALOG_PRESS 
};
enum ButtonEvent { 
	BUTTON_EVENT_PRESSED, // for digital pins, when pin voltage changes from HIGH to LOW
	BUTTON_EVENT_RELEASED, // for digital pins, when pin voltage changes from LOW to HIGH
	BUTTON_EVENT_DOUBLE_PRESS // when there are two subsequent presses within DOUBLE_CLICK_MILLIS
};

/*
 * High level class for handling button presses, releases, holds etc.
 * 
 * Normally, a button is open (port LOW) and when pressed the circuit is closed (port HIGH)
 */
struct Button {

  // define the 'Handler' callback type
  typedef void (*Handler)(ButtonEvent event, Button& button);

  Handler handler;
  unsigned char port;
  ButtonType type;
  bool status; // last value returned by the port 
  static const int LOWHIGH_THRESHOLD = 200; // when the button is bound to analog ports this is the threshold value between LOW and HIGH. The value read from the analog port ranges from 1-1024.
  static const unsigned long DOUBLE_CLICK_MILLIS = 350; // if there are two subsequent button presses within DOUBLE_CLICK_MILLIS, raise a BUTTON_EVENT_DOUBLE_PRESS
  unsigned long lastPressMillis = 0; // whatever millis() returned the last time a BUTTON_EVENT_PRESSED was raised


private:

  bool readPort() {
    //Serial.print("reading from port"); Serial.println(port);
    if (type == BUTTON_ANALOG_PRESS) {
        int value = analogRead(port);
        //Serial.print("analog value: "); Serial.println(value);
        if (value < LOWHIGH_THRESHOLD)
          return LOW;
        return HIGH;
    } else { // digital input
      return digitalRead(port);    
    }
  }

public:

  Button(unsigned char pport, ButtonType ptype, Handler phandler) {
    port = pport;
    handler = phandler;
    type = ptype;
  }

  void init() { 
    pinMode(port, INPUT);
    status = readPort();       
  }

  void update() {
    bool newStatus = readPort(); 
    //if ( type == BUTTON_PRESS) { 
      if (status == HIGH && newStatus == LOW) { 
        handler(BUTTON_EVENT_PRESSED, *this);
        if (lastPressMillis == 0)
          lastPressMillis = millis();
        else {
          unsigned long newMillis = millis();
          unsigned long delta = newMillis - lastPressMillis;
          lastPressMillis = newMillis;
          if (delta < DOUBLE_CLICK_MILLIS)
            handler(BUTTON_EVENT_DOUBLE_PRESS, *this);
        }
          
      }
    //} else
    //if ( type == BUTTON_RELEASE) { 
      if (status == LOW && newStatus == HIGH) { 
        handler(BUTTON_EVENT_RELEASED, *this);
      }
    //}    
    // TODO BUTTON_RELEASE and BUTTON_HOLD
    status = newStatus;
  }

};


#endif
