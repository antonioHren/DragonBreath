/**
* # DragonBreath - Kontrola zmajeve vatre spirometrom 
*
* # Opis projekta
* Ovaj projekt je reultat timskog rada u sklopu projeknog zadatka kolegija Razvoj ugradbenih sustava  na Tehničkom veleučilištu u Zagrebu. 
*
* DragonBreath koristi vizualni prikaz zmaja koji riga vatru kao duhovit način simuliranja rada medicinskog spirometra. Emuliranjem spirometra kroz potenciometar, korisnik udiše (okreće potenciometar), te pokušava protok zraka održati u optimalnoj zoni. Cilj projekta je gamifikacija rada spirometra, jednako koliko i njegovo funkcionalno ponašanje, te je korisnik nagrađen bodovima nakon uspješne odrade zadatka. 
*
*
*
* # Funkcijski zahtjevi
* - Čitanje vrijednosti potenciometra (12-bit ADC)
* - Zaglađivanje signala eksponencijalnim kliznim prosjekom
* -Izračun protoka i volumena simuliranog udaha
* - Prikaz tri praga aktivacije na OLED zaslonu (600, 900 i 1200 ml/s)
* - Logika optimalne zone protoka zraka u vremenskom okviru (900 < opt. < 1200 ml/s unutar 5s)
* - Implementacija kazne u slučaju pre slabog ili pre jakog protoka
* - Prikaz uspješnosti/neuspješnosti vježbe korisiniku
* - Brojanje i prikaz uspješno odrađenih ciklusa vježbe za redom (bodova)
* - Spremanje najboljeg rezultata u trajnu memoriju
* - Slanje ukupno ostvarenog volumena Wi-Fi vezom nakon svakog ciklusa
*
*
* # Tehnologije
*
* - Simulacijsko okruženje: Wokwi
* - Platforma: ESP32
* - Framework: Arduino
* - Jezik: C/C++
*
* # Članovi tima 
* | Ime | Osobni GitHub | Doprinos |
* |-----|--------|----------|
* | Antonio Hren | [GitHub-Hren](https://github.com/antonioHren/RUS--Hren) | Čitanje potenciometra, filtriranje signala i numerička integracija |
* | Mislav Maljković | [GitHub-Maljkovic](https://github.com/mislavmalj/RUS--Maljkovic) | IoT povezivost (Wi-Fi i NVS), bodovanje i dokumentacija |
* | Stefan Vedrina | [GitHub-Vedrina](https://github.com/StefanVedrina/RUS--Vedrina) | OLED prikaz i logika stanja igre |
*/

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "display.h"

// Definiranje pinova potenciometra, gumba i rezoluciju OLED-a
#define POT_PIN 34                 ///< Pin za analogni potenciometar
#define RESET_BUTTON_PIN 25        ///< Pin za ESP reset gumb
#define SCREEN_WIDTH 128           ///< Sirina OLED zaslona
#define SCREEN_HEIGHT 64           ///< Visina OLED zaslona
#define OLED_RESET -1

// Definiranje SDA i SCL pinova OLED-a
#define I2C_SDA 8
#define I2C_SCL 9

// Konstanta za zaglađivanje signala (Exponential Moving Average)
const float EMA_ALPHA = 0.15;

// Granice željnog protoka
const float FLOW_THRESHOLD_SUCCESS = 900.0;
const float FLOW_THRESHOLD_FAIL = 1200.0;

// Vremenske granice uspješnog udaha
const unsigned long SUCCESS_TIME = 5000; // 5 s
const unsigned long LOW_FLOW_TIME = 5000; 

// Vrijednosti protoka daha
float filteredADC = 2048.0;
float flowRate = 0.0;
float volume = 0.0;

// Varijable stabilnosti daha
float avgFlow = 0.0;
bool firstStableRun = true;

// Uspješnost vježbe
bool exerciseSuccess = false;
bool exerciseFailed = false;

// Varijable za mjerenje vremena uz millis()
unsigned long previousTime = 0;
unsigned long successStartTime = 0;
unsigned long lowFlowStartTime = 0;

// Rezultat
int score = 0;
int highScore = 0;
bool tooStrongFail = true;

// Trenutno stanje vježbe
GameState currentState = IDLE;

// Podaci o mreži i broker za testiranje MQTT
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
long currentTime = 0;
int value = 0;

// Varijable potrebne za trajnu memoriju
Preferences prefLib;



// Metoda za ponovni pokušaj uspostavljanja veze
void reconnect() 
{
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }

  }
}


// Meteda za slanje volumena ciklusa korištenjem MQTT protokola
void publishScore(int score)
{
  while (!client.connected()) {
      reconnect();
  }
  client.loop();
  client.publish("esp32/score", String(score).c_str());
  Serial.print("Ukupni volumen ciklusa poslan: ");
  Serial.print(volume);
  Serial.println(" ml");
}


// Metoda za spremanje rezultata u trajnu memoriju (NVS)
void saveHighScore(int score)
{
  prefLib.begin("dragonGame", false);
  prefLib.putInt("highScore", score);
  prefLib.end();
  Serial.print("Novi high score spremljen: ");
  Serial.println(score);
}


// Metoda za čitanje rezultata iz trajne memorije
int loadHighScore()
{
  prefLib.begin("dragonGame", true);
  int highScore = prefLib.getInt("highScore", 0);
  prefLib.end();

  Serial.print("High score ucitan: ");
  Serial.println(highScore);
  return highScore;
}


// Metoda za očitavanje i filtriranje izlaza potenciometra
void readSensor()
{
    int raw = analogRead(POT_PIN);

    filteredADC =
        EMA_ALPHA * raw +
        (1.0 - EMA_ALPHA) * filteredADC;
}


// Metoda za prevođenje vrijednosti potenciometra u protok zraka
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


// Metoda za računanje ukupnog volumena ciklusa
void calculateVolume()
{
    unsigned long now = millis();

    float dt =
        (now - previousTime) / 1000.0;

    previousTime = now;

    volume += flowRate * dt;
}


// Metoda za provjeru stabilnosti udaha
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


// Metoda za pretvorbu stanja sustava u String varijablu
String getStateAsString() 
{
    switch(currentState) {
      case IDLE: return "IDLE";
        break;
      case INHALE: return "INHALE";
        break;
      case SUCCESS: return "SUCCESS";
        break;
      default: return "FAILED";
        break;
    }
}


// Metoda za promjenu stanja sustava
void changeState(GameState newState) 
{
  if (currentState == newState) return;

  //Provjera kraja ciklusa za slanje rezultata
  if(newState == SUCCESS || newState == FAILED)
  {
    publishScore(volume);
  }

  if(newState == FAILED){
    Serial.print("Broj bodova: ");
    Serial.println(score);
    Serial.println("------------ Kraj Vježbe -------------\n");
  }

  // Provjera početka nove vježbe za reset i spremanje rezultata
  if(currentState == FAILED)
  {
    if(score > highScore){
      highScore = score;
      saveHighScore(highScore);
    }

    Serial.println("\n------------ Početak vježbe -------------");

    score = 0;
    counter_reset();
  }

  String debugInfo = "Changed state: " + getStateAsString() + " => ";
  
  currentState = newState;

  debugInfo = debugInfo + getStateAsString();
  Serial.println(debugInfo);
}


// Metoda za provjeru uspjeha vježbe
void updateExercise()
{

    if(exerciseSuccess || exerciseFailed)
    {
      return;
    }

    unsigned long now = millis();
    changeState(INHALE);

    // Fail uvjet za prejak protok zraka
    if(flowRate > FLOW_THRESHOLD_FAIL)
    {
        exerciseFailed = true;
        tooStrongFail = true;
        changeState(FAILED);
        return;
    }

    // Fail uvjet za pre slab protok zraka
    if(flowRate < FLOW_THRESHOLD_SUCCESS)
    {
        if(lowFlowStartTime == 0)
        {
            lowFlowStartTime = now;
        }

        if(now - lowFlowStartTime >= LOW_FLOW_TIME)
        {
            exerciseFailed = true;
            tooStrongFail = false;
            changeState(FAILED);
        }

        successStartTime = 0;

        return;
    }

    lowFlowStartTime = 0;

    
    // Uvjet za uspješno odrađen ciklus
    bool stable = isStable();
    if(stable)
    {
        if(successStartTime == 0)
        {
            successStartTime = now;
        }

        if(now - successStartTime >= SUCCESS_TIME)
        {
            changeState(SUCCESS);
            exerciseSuccess = true;

            // Povećanje rezultata
            score++;
        }
    }
    else
    {
        successStartTime = 0;
    }
}


// Metoda za reset podataka ciklusa
void resetExercise()
{
    exerciseSuccess = false;
    exerciseFailed = false;

    successStartTime = 0;
    lowFlowStartTime = 0;

    volume = 0;

    firstStableRun = true;

}


// Metoda zvana prekidom za resetiranje ESP32
void resetESP(){
    Serial.println("\n Reset mikrokontrolera!s");
    WiFi.disconnect();
    ESP.restart();
}




void setup()
{
    Serial.begin(115200);

    // Kod za povezivanje na WiFi (Nakon Serial.begin)
    WiFi.disconnect(true);
    WiFi.persistent(false);

    Serial.println("Conncting to WiFi");
    WiFi.begin(ssid, password, 6);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println("Connection successful");


    //Uspostavljanje servera za MTQQ pozive
    client.setServer(mqtt_server, 1883);


    // Postavljanje prekida na gumb
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(RESET_BUTTON_PIN, resetESP, RISING);

    analogReadResolution(12);
    previousTime = millis();
    highScore = loadHighScore();

    Serial.println("Spirometer logic started\n");
    display_init(); 

    Serial.println("------------ Početak vježbe -------------");
}

void loop()
{
  readSensor();
  calculateFlow();

  // Reset ciklusa ako uđe u IDLE rang vrijednosti potenciometra
  if(flowRate < 50)
  {
    if (currentState != IDLE) 
      {
          changeState(IDLE);
          resetExercise();
      }
  }
  else 
  {
    //Ažuriranje numeričkih vrijednosti ovisno o trenutnom stanju
    if (!exerciseSuccess && !exerciseFailed) 
    {
        calculateVolume();
        updateExercise();
    }
  }

  //Izračun vremena zaslona
  unsigned long displayTime = (successStartTime > 0) ? (millis() - successStartTime) : 0;

  //Ažuriranje OLED zaslona
  display_update(currentState, flowRate, displayTime, score, score > highScore ? true : false, tooStrongFail);
  
  //Ispis vrijednosti dobivenih numeričkom integracijom
  /*
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
  */
}
