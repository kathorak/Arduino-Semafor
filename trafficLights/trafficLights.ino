#include <Arduino.h>

//* ===== API ======

class Led{
  private:
  byte _pin;
  bool _state = false;
  
  public:
  Led(byte pin){
  	_pin = pin;
  }
  void init(){
  	pinMode(_pin, OUTPUT);
  }
  void turn(bool state){
  	 _state = state;
    digitalWrite(_pin, _state);
  }
  bool switchState(){
    turn(!_state);
    return _state;
  }
};

class Timer{
  private:
  unsigned long _duration; // Dlouhé bezznaménkové celé číslo
  unsigned long _startTime;
  
  public:
  Timer(unsigned long duration){
  	timeSet(duration);
  }
  void init(){
  	reset();
  }
  void reset(){
  	_startTime = millis();
  }
  void timeSet(unsigned long newDuration){
  	_duration = newDuration;
  }
  bool timeout(){
  	return _startTime + _duration < millis();
  }  
};

class Button{
  private:
  byte _pin;
  Timer _timer;
  enum States {UNTOUCH, REBOUNCE, PRESS, HOLD};
  States _state = UNTOUCH;
  
  public:
  Button(byte pin, int rebounceTime = 50): _timer(rebounceTime){
  	_pin = pin;
  }
  void init(){
  	pinMode(_pin, INPUT);
    _timer.init();
  }
  void update(){
    switch(_state){
      case UNTOUCH:{
        if(digitalRead(_pin)){
          _state = REBOUNCE;
          _timer.reset();
        }
        break;
      }
      case REBOUNCE:{
        if(digitalRead(_pin)){
          if(_timer.timeout()){
            _state = PRESS;
          }
        }
        else{
          _state = UNTOUCH;
        }
        break;
      }
      case PRESS:{
        if(digitalRead(_pin)){
          _state = HOLD;
        }
        else{
          _state = UNTOUCH;
        }
        break;
      }
      case HOLD:{
        if(!digitalRead(_pin)){
          _state = UNTOUCH;
        }
        break;
      }
    }
  }
  bool isPressed(){
  	update();
    return _state == PRESS;
  }
  bool isHeld(){
    update();
    return _state == HOLD;
  }
};

//* ===== Variables ======

int i = 1000;
int d = 200;

Timer timer(i);
Led led(1);
Button button(9);

//* ===== Arduino code ======

void setup(){
  led.init();
  led.turn(false);
  timer.init();
  button.init();
}	

void loop(){
  if(button.isPressed()){
    led.switchState();
  }
}

