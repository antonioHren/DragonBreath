 # Specifikacija zahtjeva sustava

## Obrasci uporabe

### UC1: Kontrola protoka zraka

**Glavni sudionik**: Korisnik

**Cilj**: Utjecati na uspješnost vježbe

**Preduvjet**: Sustav je uključen i u INHALE stanju

Opis osnovnog tijeka:
1. Korisnik fizički okreće potenciometar
2. Sustav očitava vrijednost potenciometra (F-1.1)
3. Sustav zaglađuje signal EKP-om (F-1.2)
4. Sustav računa protok zraka (F-1.3)
5. Sustav prikazuje trenutni protok zraka korisniku (F-1.4)


***

### UC2: Uspješna odrada vježbe

**Glavni sudionik**: Korisnik

**Cilj**: Uspješna odrada jednog ciklusa vježbe

**Preduvjet**: Sustav je uključen i u INHALE stanju

Opis osnovnog tijeka:
1. Korisnik održava protok zraka u optimalnoj zoni protoka zraka (900 - 1200 ml/s)
2. Sustav u stvarnome vremenu očitava, zaglađuje i računa protok iz signala (F-1.1, F-1.3, F-1.2, F-1.4)
3. Nakon 5 sekundi sustav detektira uspješnu odradu vježbe (F-2.1)
4. Sustav prikazuje korisniku uspješnost vježbe (F-2.3)
5. Sustav boduje korisnika i ažurira LED brojač (F-2.4)
6. Sustav šalje ukupni postignuti volumen ciklusa Wi-Fi vezom (F-2.5)
7. Sustav ulazi u SUCCESS stanje


***

### UC3: Nespješna odrada vježbe

**Glavni sudionik**: Korisnik

**Cilj**: Neuspješna odrada jednog ciklusa vježbe

**Preduvjet**: Sustav je uključen i u INHALE stanju

Opis osnovnog tijeka:
1. Korisnik održava protok zraka izvan optimalne zone protoka zraka (900 - 1200 ml/s)
2. Sustav u stvarnome vremenu očitava, zaglađuje i računa protok iz signala (F-1.1, F-1.3, F-1.2, F-1.4)
3. Nakon 5 sekundi ili prelaskom gornje granice protoka sustav detektira neuspješnu odradu vježbe (F-2.2)
4. Sustav prikazuje korisniku uspješnost vježbe (F-2.3)
5. Sustav šalje ukupni postignuti volumen ciklusa Wi-Fi vezom (F-2.6)
6. Sustav ulazi u FAILED stanje

Opis mogućih odstupanja:
- Finalni rezultat je veći od high score-a:
1. Sustav sprema novi najbolji rezultat u trajnu memoriju (F-2.5)


***

### UC4: Ponovno pokretanje sustava

**Glavni sudionik**: Korisnik

**Cilj**: Ponovno pokrenuti sustav i učitati podatke iz trajne memorije

**Preduvjet**: Sustav je uključen

Opis osnovnog tijeka:
1. Korisnik pritisne gumb za ponovno pokretanje sustava
2. Sustav se ponovno pokreće (F-3.1)
3. Sustav učitava podatke o najboljem rezultatu iz trajne memorije (F-3.2)

Opis mogućih odstupanja:
- Ne postoji zapis o najboljem rezultatu:
1. Sustav odabire početnu vrijdnost 0 za najbolji rezultat


## Dijagram obrazaca upotrebe

<img width="664" height="206" alt="image" src="https://github.com/user-attachments/assets/0b0d7ee3-61d5-45ab-8f9b-2c6f60173d26" />

## Provjera uključenosti ključnih funkcionalnosti u obrasce uporabe

| Funkcionalni zahtjev | Obuhvaćeni obrasci uporabe |
|-----|----------|
| F-1.1: Očitavanje potenciometra| UC1, UC2, UC3 |
| F-1.2: Zaglađivanje signala | UC1, UC2, UC3 |
| F-1.3: Računanje protoka | UC1, UC2, UC3 |
| F-1.4: Prikaz protoka | UC1, UC2, UC3 |
| F-2.1: Detekcija uspješne vježbe| UC2 |
| F-2.2: Detekcija neuspjele vježbe | UC3 | 
| F-2.3: Prikaz uspješnosti vježbe | UC2, UC3 |
| F-2.4: Prikaz trenutnog rezultata | UC2 | 
| F-2.5: Spremanje najboljeg rezultata | UC3* | 
| F-2.6: Slanje volumena Wi-Fi-om | UC2, UC3 | 
| F-3.1: Ponovno pokretanje| UC4 | 
| F-3.2: Učitavanje najboljeg rezultata| UC4 | 
