
# SyntaxView

SyntaxView je plugin pre **Open Salamander**, ktorý umožňuje prehliadanie textových súborov so zvýrazňovaním syntaxe. Na zvýrazňovanie používa **TextMate** gramatiky a témy.

## Požiadavky

Plugin je kompatibilný s verziami:

 - Open Salamander 5.0 a novšie
 - Altap Salamander 4.0

## Návod na inštaláciu
Používateľ môže plugin získať stiahnutím jeho skompilovanej verzie z GitHub repozitára. Stiahnutý archív `syntaxview-plugine.zip` môže byť rozbalený do ľubovolnej lokácie, jeho obsah ale musí pre správne fungovanie ostať v celku.

### Obsah archívu

 ```text
syntaxview-plugin/
├── syntaxview.spl        	  # Súbor pluginu
├── extension_bindings.conf   # Konfigurácia väzieb prípon súborov na gramatiky
│
├── grammars/                 # Definície TextMate gramatík
│   ├── c.json
│   ├── cpp.json
│   ├── java.json
│   └── ...                   # Ďalšie gramatiky
│
├── lang/                     # Lokalizačné súbory
│   └── english.slg           # Anglický jazykový balík
│
└── themes/                   # Definície TextMate tém
    ├── dark-plus.json
    ├── light-plus.json
    └── ...                   # Ďalšie témy

    
syntaxview-plugin-x86/		 # 32-bitová verzia pluginu (rovnaký obsah ako uvedený vyššie)
```

### Inštalácia a použitie
Pre pridanie pluginu do Open Salamandra je potrebné sa cez menu navigovať: 
**Plugins** -> **Plugin Manager...** -> **Add...** 

V dialógu pre výber súboru následne zvoliť súbor `.spl` rovnakej architektúry ako Open Salamander. Nepoužitú verziu pluginu (pre druhú architektúru) je možné odstrániť.

Plugin je po pridaní automaticky priradený na otváranie (niektorých) podporovaných typov súborov a je ho možné využívať. Súbor sa otvára stlačením klávesy **F3**.

Počet typov súborov priradených k prehliadaču je v programe Open Salamander obmedzený, preto je potrebné vykonať vlastnú konfiguráciu podľa toho, aké programovacie jazyky používate.
Ak chcete, aby plugin otváral všetky typy súborov, môžete použiť *.*

## Konfigurácia pluginu

Zmena typov súborov, ktoré sú prehliadané pomocou pluginu sa nastavuje priamo v menu Open Salamandra: 
**Options** -> **Configuration** -> **Viewers and Editors** -> **Viewers**. 

Na tomto mieste sa nastavujú otvárané typy súborov a aj priorita použitia vôči ostatným prehliadačom.

### Nastavenia pluginu
Nastavenia pluginu sú dostupné cez hlavné menu Open Salamandra: **Options** -> **Plugin Configuration** -> **SyntaxView**. 
 
 Dostupné sú tiež priamo z otvoreného okna prehliadača pluginu: **Options** -> **Configuration**. 

V tomto konfiguračnom okna je možné nastaviť nasledujúce vlastnosti:

 - Použitá téma
 - Font a jeho veľkosť
 - Veľkosť tabulátora
 - Rýchlosť scrollovania
 - Východzie nastavenie číslovania riadkov
 - Pozícia na ktorej sa otvára okno prehliadača

## Voľba použitej gramatiky
Pri prehliadaní súboru vidí uživateľ v hornej menu lište, aká gramatika sa použila na parsovanie súboru. Po kliknutí môže v rozbaľovacom menu vybrať inú gramatiku, ktorá sa okamžite aplikuje.

### Automatický výber gramatiky
Pri otvorení súboru sa použitá gramatika sa vyberá na základe obsahu súboru `extension_bindings.conf`. Tento súbor obsahuje priradenia prípon súborov ku gramatikám.
  
Každý riadok má formát:  
  
```text  
<prípona> = <grammar_súbor>  
```  
  
Príklad:  
  
```text  
c = c.json  
cpp = cpp.json  
py = python.json  
js = javascript.json  
```  

Pravidlá:
- každý záznam musí byť na samostatnom riadku  
- jedna prípona môže byť priradená iba k jednej gramatike  
- grammar súbor musí existovať v priečinku `grammars/`

## Pridanie nového jazyku

 1. Do adresáru `grammars/` pridať súbor s definícou gramatiky.*
 2. Do súboru `extension_bindings.conf` pridať záznam priradzujúci novú príponu k danej gramatike.
 3. V konfigurácii Open Salamander nastaviť aby bol daný typ súboru otváraný v plugine.

*Gramatika musí byť vo formáte `JSON`.

## Pridanie novej témy

 1. Do adresáru `themes/` pridať súbor s definícou témy.*
 2. V konfigurácii pluginu zvoliť danú tému.

*Téma musí byť vo formáte `JSON`. V jej definicií musí byť v koreňovej úrovni definovaný aspoň jeden z atribútov `displayName` alebo `name`, bez nich nebude téma v zozname ponúknutá.

## Klávesové skratky

### V programe Open Salamander:

 - Prehliadanie súboru pomocou pluginu -- `F3` po zvolení súboru

### V okne prehliadača pluginu:

 - Otvorenie iného súboru -- `CTRL+O`
 - Otvorenie prehliadaného súboru v editore --  `CTRL+E`
 - Otvorenie nasledujúceho súboru (v adresári) -- `SPACE`
 - Otvorenie predchádzajúceho súboru (v adresári) -- `BACKSPACE`
 - Otvorenie dialógu pre prechod na riadok -- `CTRL+G`
 - Kopírovanie označeného textu -- `CTRL+C`
 - Označenie celého súboru -- `CTRL+A`
 - Otvorenie dialógu pre vyhľadávanie v súbore -- `CTRL+F`
 - Nasledujúci výsledok vyhľadávania -- `CTRL+N`
 - Predchodzí výsledok vyhľadávania -- `CTRL+P`
 - Pokračovanie vyhľadávania s nezmenenými parametrami -- `F3`
 - Maximalizácia a revertovanie maximalizácie okna -- `F11`
 - Zapnutie alebo vypnutie zobrazenia číslovania riadkov -- `CTRL+X`
 - Vertikálne posúvanie -- `SCROLL`
 - Horizontálne posúvanie -- `ALT+SCROLL`
 - Zmena velkosti fontu -- `CTRL+SCROLL`
 - Úprava aktívnej selekcie -- `SHIFT+ARROWS`

## Právne informácie  
  
SyntaxView je open-source softvér licencovaný pod GPLv3. Niektoré jednotlivé súbory a knižnice používajú odlišné, ale kompatibilné licencie.
  
  
## Tretie strany  
  
Tento projekt používa knižnice a dátové zdroje tretích strán:  
  
### Knižnice  
  
- JSONCpp -- MIT licencia  
- Onigmo -- BSD štýl licencia  
- nvim-textmate (icedman) -- licencia kompatibilná s GPLv3  
- tinyxml2 -- Zlib licencia
  
### Gramatiky témy  
  
Tento projekt obsahuje TextMate gramatiky a témy z nasledovného zdroja:  
  
- Zdroj: https://github.com/shikijs/textmate-grammars-themes  
- Licencia: distribuované pod ich vlastnými licenciami (MIT, Apache 2.0, BSD štýl atď.)