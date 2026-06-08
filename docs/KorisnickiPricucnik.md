# Korisnički priručnik - Dragon Breath

Dragon Breath je simulacija medicinskog spirometra sa zadatkom kontroliranja zmaja i njegovog vatrenog daha. 

## Pravila igre

1. Okrenuti potenciometar dok zmaj ne krene bacati vatreni dah
2. Uz pažljivo kalibriranje potenciometra držati zmajevu vatru na vrhuncu snage barem 5 sekundi, no biti pažljiv
3. Sakupiti što više bodova i zabaviti se

## Potrebne tehnologije
-  ESP32 Mikrokontroler- Centralna komponenta sustava
- Potenciometar- Emulator spirometra (12-bit ADC)
- Jednostavan gumb- Služi za ponovno pokretanje ESP32 (eng. reset)
- SSD1306 OLED zaslon- Glavni prikaz logike sustava
- TM1637 LED brojač- Prikaz ostvarenih bodova korisniku

## Postavljanje simulacije
1. Otvoriti Wokwi okruženje
2. Unijeti sve potrebne datoteke iz repozitorija
3. Pritisnuti **PLAY** i pričekati

#### Savjet
- U nezgodnim situacijama se uvijek moguće vratiti u stanje mirovanja
- Zmaj nije stvaran
