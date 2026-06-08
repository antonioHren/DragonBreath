# Arhitektura sustava

## Stil arhitekture
DragonBreath koristi arhitekturu temeljenu na stanju. Svaka komponenta ima definiran način rada ovisno o jednom od predefiniranih stanja sustava (IDLE, SUCCESS, INHALE i FAILED), te njihovim podstanjima (Opimalna zona i neoptimalna zona vrijednosti protoka zraka u INHALE stanju).

- Visoka kohezija: Svako stanje ima unaprijed definiranu ulogu
- Brzo vrijeme odziva i niska povezanost
- Neblokirajuće: U kodu koristimo millis() umjesto delay()

## Podsustavi
| Podsustav| Komponente| Uloga |
|-----|----------| ------- |
| Senzorski| Analogni potenciometar (PIN34) | Konstantno prikupljanje podataka iz okoline |
| Aktuatorski | SSD1306 OLED zaslon i TM1637 LED brojač | Prikaz rezultata, protoka i stanja uspjeha | 
| Komunikacijski | Ugrađeni Wi-Fi sustav | Prijenos vrijednosti volumena na vanjsku točku |
| Memorijski | NVS (Non-Volatile storage) memorija | Trajna memorija za spremanje najvišeg dosegnutog rezultata |

## Mrežni protokoli
U svrhu slanja vrijednosti volumena ovaj projekt koristi MTQQ protokol. Koristi se jer je jednostavan i brz, te često korišten u manjim uređajima niske potrošnje. Za njegovu implementaciju se koristi biblioteka PubSubClient.

## Dijagram stanja
<img width="616" height="415" alt="image" src="https://github.com/user-attachments/assets/274db9e3-9a55-47f1-8aa3-f4bc5582b6c4" />

## Dijagram aktivnosti
<img width="877" height="955" alt="image" src="https://github.com/user-attachments/assets/87238175-c6cf-4787-aa8d-80c4080fdbe8" />
