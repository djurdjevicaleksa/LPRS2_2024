# AC-Logger - LPRS2_2024 Projekat

## Opis
U ovom projektu implementirali smo sistem za belezenje podataka o naizmenicnoj struji (AC) pomocu Arduina i dodatnog shield-a na kom smo prikacili RTC modul i modul za SD karticu. Sistem omogucava merenje vrednosti analognog signala, njihovo vremensko oznacavanje i skladistenje na SD karticu. Na ovaj nacin smo pruzili pouzdan i precizan nacin za pracenje karakteristika naizmenicne struje, sto moze biti korisno u razlicitim aplikacijama kao sto su energetski nadzor, optimizacija potrosnje elektricne energije i analiza kvaliteta struje.


## Struktura
* U okviru `SineReadingTest` direktorijuma postoje sve potrebne biblioteke
* Takodje, nalazi se i glavni `SineReadingTest.ino` fajl


## Setup
* Otvori se `SineReadingTest.ino` u *ArduinoIDE* i uploaduje se na Arduino koji je povezan na racunar
* Nakon sto je odradjen upload, ocitavanje vrednosti i upis na SD karticu se odvija automatski
* Na *Serial Monitoru* je moguce debagovati greske i pratiti vrednosti koje se upisuju na SD karticu

