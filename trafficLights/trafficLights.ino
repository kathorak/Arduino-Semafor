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

class TrafficLights3{
enum States {STOP, READY, GO, HALT, OFF};
private:
  States _state;

  Led _r, _y, _g;
  Timer _t;

  void _stop(){
    _r.turn(true);
    _y.turn(false);
    _g.turn(false);
  }
  void _ready(){
    _r.turn(false);
    _y.turn(true);
    _g.turn(false);
  }
  void _go(){
    _r.turn(false);
    _y.turn(false);
    _g.turn(true);
  }
  void _halt(){
    _r.turn(true);
    _y.turn(true);
    _g.turn(false);
  }
  void _off(){
    _r.turn(false);
    if(_t.timeout()){
      _y.switchState();
      _t.reset();
    }
    _g.turn(false);
  }

public:
  TrafficLights3(byte pin_r, byte pin_y, byte pin_g, States state):
  _r(pin_r), _y(pin_y), _g(pin_g), _t(1000){
    _state = state;
  }

  void init(){
    _r.init();
    _y.init();
    _g.init();
    _t.init();
  }

  void changeState(States newState){
    _state = newState;

  }

  void update(){
    switch (_state){
      case STOP:
        _stop();
        break;
      case READY:
        _ready();
        break;
      case GO:
        _go();
        break;
      case HALT:
        _halt();
        break;
      case OFF:
        _off();
        break;
    }
  }
};

//* ===== Variables ======

int i = 1;

TrafficLights3 semafor(3, 2, 1, 0);
Button button(9);

//* ===== Arduino code ======

void setup(){
  button.init();
  semafor.init();
}	

void loop(){
  if(button.isPressed()){
    semafor.changeState(i);
    i++;
    i %= 5;
  }

  semafor.update();
}
