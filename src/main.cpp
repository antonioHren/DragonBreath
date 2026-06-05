#include <Arduino.h>

#define POT_PIN 34
#define RESET_BUTTON_PIN 25

//konstante
const float EMA_ALPHA = 0.15;

const float FLOW_THRESHOLD_SUCCESS = 900.0;
const float FLOW_THRESHOLD_FAIL = 1200.0;

const unsigned long SUCCESS_TIME = 5000; // 5 s
const unsigned long LOW_FLOW_TIME = 5000; 


float filteredADC = 2048.0;
float flowRate = 0.0;
float volume = 0.0;


float avgFlow = 0.0;
bool firstStableRun = true;


bool exerciseSuccess = false;
bool exerciseFailed = false;

unsigned long previousTime = 0;

unsigned long successStartTime = 0;
unsigned long lowFlowStartTime = 0;

//ocitavanje senzora

void readSensor()
{
    int raw = analogRead(POT_PIN);

    filteredADC =
        EMA_ALPHA * raw +
        (1.0 - EMA_ALPHA) * filteredADC;
}

//pretvorba ocitanja potenciometra u protok zraka
void calculateFlow()
{
    if (filteredADC <= 2048)
    {
        flowRate = 0;
        return;
    }

    flowRate =
        ((filteredADC - 2048.0) /
         (4095.0 - 2048.0))
        * 1500.0;
}

//racunanje volumena

void calculateVolume()
{
    unsigned long now = millis();

    float dt =
        (now - previousTime) / 1000.0;

    previousTime = now;

    volume += flowRate * dt;
}

//stabilnost
bool isStable()
{
    if(firstStableRun)
    {
        avgFlow = flowRate;
        firstStableRun = false;
    }

    avgFlow =
        0.05 * flowRate +
        0.95 * avgFlow;

    return abs(flowRate - avgFlow) < 70;
}

//provjera uspjesnosti

void updateExercise()
{
    if(exerciseSuccess || exerciseFailed)
        return;

    unsigned long now = millis();

//fail uvjeti
    if(flowRate > FLOW_THRESHOLD_FAIL)
    {
        exerciseFailed = true;
        return;
    }



    if(flowRate < FLOW_THRESHOLD_SUCCESS)
    {
        if(lowFlowStartTime == 0)
        {
            lowFlowStartTime = now;
        }

        if(now - lowFlowStartTime >= LOW_FLOW_TIME)
        {
            exerciseFailed = true;
        }

        successStartTime = 0;

        return;
    }

    lowFlowStartTime = 0;

//uspjesna zona

    bool stable = isStable();

    if(stable)
    {
        if(successStartTime == 0)
        {
            successStartTime = now;
        }

        if(now - successStartTime >= SUCCESS_TIME)
        {
            exerciseSuccess = true;
        }
    }
    else
    {
        successStartTime = 0;
    }
}

//reset

void resetExercise()
{
    exerciseSuccess = false;
    exerciseFailed = false;

    successStartTime = 0;
    lowFlowStartTime = 0;

    volume = 0;

    firstStableRun = true;
}

//api

float getFlowRate()
{
    return flowRate;
}

float getVolume()
{
    return volume;
}

bool getStable()
{
    return isStable();
}

bool isExerciseSuccessful()
{
    return exerciseSuccess;
}

bool isExerciseFailed()
{
    return exerciseFailed;
}


void setup()
{
    Serial.begin(115200);

    analogReadResolution(12);

    previousTime = millis();

    Serial.println("Spirometer logic started");

    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
  if (digitalRead(RESET_BUTTON_PIN) == LOW)
  {
      Serial.print("Reseting!");
      ESP.restart();
      
  }
    readSensor();

    calculateFlow();

    calculateVolume();

    updateExercise();

    Serial.print("Flow=");
    Serial.print(flowRate, 1);

    Serial.print(" ml/s");

    Serial.print(" | Volume=");
    Serial.print(volume, 0);

    Serial.print(" ml");

    Serial.print(" | Stable=");
    Serial.print(isStable());

    Serial.print(" | Success=");
    Serial.print(exerciseSuccess);

    Serial.print(" | Failed=");
    Serial.println(exerciseFailed);

    //reset ako se vrati potenciometar na sredinu

    if(flowRate < 50 &&
       (exerciseSuccess || exerciseFailed))
    {
        resetExercise();
    }
}

