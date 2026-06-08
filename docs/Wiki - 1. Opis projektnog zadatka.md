# Opis projektnog zadatka

## Opći opis sustava
DragonBreath koristi vizualni prikaz zmaja koji riga vatru kao duhovit način simuliranja rada medicinskog spirometra. Emuliranjem spirometra kroz potenciometar, korisnik udiše (okreće potenciometar), te pokušava protok zraka održati u optimalnoj zoni. Cilj projekta je gamifikacija rada spirometra, jednako koliko i njegovo funkcionalno ponašanje, te je korisnik nagrađen bodovima nakon uspješne odrade zadatka.

Projekt se bazira na ESP32 mikrokontroleru koji koristi Arduino framework, te je simuliran u simulacijskom okruženju Wokwi.

## Komponente sustava
- **ESP32 Mikrokontroler**- Centralna komponenta sustava
- **Potenciometar**- Emulator spirometra (12-bit ADC)
- **Jednostavan gumb**- Služi za ponovno pokretanje ESP32 (eng. reset)
- **SSD1306 OLED zaslon**- Glavni prikaz logike sustava
- **TM1637 LED brojač**- Prikaz ostvarenih bodova korisniku

## Korištene bibilioteke
- **Adafruit SSD1306**- OLED zaslon
- **TM1637 Driver**- LED brojač
- **PubSubClient**- Wi-Fi/MQTT protokol
- **Preferences**- Trajna(NVS) memorija

## Pragovi i logika sustava

| - | Protok | Opis stanja |
|-----|--------|----------|
| 1 | x < 50 ml/s | Sustav miruje (IDLE) |
| 2 | 50 ml/s < x < 900 ml/s | Sustav mjeri protok zraka izvan optimalne zone (INHALE) |
| 3 | 900 ml/s < x < 1200 ml/s | Protok zraka se nalazi u optimalnoj zoni (SUCCESS) |
| 4 | 1200 ml/s < x| Protok zraka prešao gornju granicu (FAILED) |

Uvjeti uspješne odrade vježbe:
- Protok zraka se nalazi u optimalnoj zoni (između 900 i 1200 ml/s)
- Stabilan protok u trajanju od 5 sekundi

Uvjeti neuspješne odrade vježbe:
- Protok zraka prešao gornju granicu od 1200 ml/s

ili

- Protok zraka se nalazi u neoptimalnoj zoni (između 50 i 900 ml/s)
- Stabilan protok u trajanju od 5 sekundi


## Potencijalna korist projekta
- **Edukacijska**: Logički prikaz rada spirometra, te primjer njegove integracije u ugradbeni sustav
- **Medicinska**: Implementacijom rada spirometra u igru povećava se motivacija pacijenata

## Ciljani korisnici
- Studenti medicine
- Pacijenti sa plućnim problemima
- Zainteresirani u ugradbene sustave

## Moguće nadogradnje sustava
- Implementacija pravog spirometra
- Nadogradnja aspekta igre sa različitim razinama i težinama
