#include "CLIO_STEERING_WHEEL_CMD.h"

#include "Arduino.h"

uint8_t buttons_indexes[NB_BUTTONS] = {0, 1, 2, 6, 7, 8};
uint8_t wheel_indexes[NB_WHEEL] = {3, 4, 5};

WHEEL_CMD::WHEEL_CMD(uint8_t *inputs, uint8_t *outputs) {
    _inputs = inputs;
    _outputs = outputs;
};

void WHEEL_CMD::update() {

    bool keys[9] = {
        false, false, false,
        false, false, false,
        false, false, false,
    };

    for (uint8_t c=0; c < NB_OUTPUTS; c++) { //for each output
        //make the output HIGH
        pinMode(_outputs[c], OUTPUT);
        digitalWrite(_outputs[c], LOW);

        for (uint8_t k=0; k < NB_INPUTS; k++) { //listen each input
            keys[k*3 + c] = analogRead(_inputs[k]) == 0;
        }

        //reset the output to LOW
        digitalWrite(_outputs[c], HIGH);
        pinMode(_outputs[c], INPUT);
    }

    for (uint8_t i=0; i < NB_BUTTONS; i++) {
        //do not overflow uint16_t number
        keys[i] ? (_buttons[i] < 0xFFFF && _buttons[i]++) : _buttons[i] = 0;
    }

    //Serial.println();
    //for(uint8_t i =0; i < NB_BUTTONS; i++) {
        //Serial.print(keys[i]);
        //if ((i+1)%3 == 0) {
            //Serial.println();
        //}
    //}
    //Serial.println();

};

uint8_t WHEEL_CMD::getButton(uint8_t button) {

    uint8_t result;

    if (_buttons[button] == INTERVAL_HOLD) {
        return HOLD;
    } else if (_buttons[button] == INTERVAL_PRESS) {
        return PRESSED;
    } else {
        return NO_NEWS;
    }


};

uint8_t WHEEL_CMD::getWheel() {

    //if all true wheel buttons are down, we're on a transition
    if (_buttons[3] == 0 and _buttons[4] == 0 and _buttons[5] == 0) {
        return NO_NEWS;
    }

    //which wheel button is up ?
    uint8_t tmp;
    if (_buttons[3] > 0) {
        tmp = 0;
    } else if (_buttons[4] > 0) {
        tmp = 1;
    } else {
        tmp = 2;
    }

    //if it has change since last time
    if (tmp != _currentWheel and _currentWheel != 3) {
        uint8_t result;

        //update wheel status
        _lastWheel = _currentWheel;
        _currentWheel = tmp;

        if ((_lastWheel == 0 && _currentWheel == 1)
            || (_lastWheel == 1 && _currentWheel == 2)
            || (_lastWheel == 2 && _currentWheel == 0)) {
            return WHEEL_DOWN;
        } else{
            return WHEEL_UP;
        }

    } else {
        //if _currentWheel == 3, it's the first time we're here
        //so we return NO_NEWS and update _currentWheel
        //in order to prevent rising event at arduino start
        if (_currentWheel == 3) {
            _currentWheel = tmp;
        }
        return NO_NEWS;
    }

};
