# Pokémon Emerald Worped-Ex

This is a personal fork of **`pokeemerald-expansion` 1.16.0**, a decompilation of Pokémon Emerald by the ROM Hacking Hideout (RHH). 

**🧪 Testing On:** New Nintendo 3DS w/ `open_agb_firm` & Steam Deck w/ `mGBA`

---

## 🧬 Overview
This is a successor to my first foray into ROM hacking, **Modern Emerald - Worped**, a project I built on top of Resete12's **Modern Emerald**. I found the old vanilla `pokeemerald` codebase to be too limiting, and have decided to move it all over to `pokeemerald-expansion`.

My project goal isn't to make some kind of difficulty hack or definitive version of Emerald, it's to have fun messing around with a game I adore. I played Sapphire & Emerald as a kid and still have my GBA cartridges. Hoenn is and always will be my favorite region. Honestly up until this year I had never even played Gen IV-VII(I played Sw&Sh briefly). But working on my hack really has brought back my love for Pokémon and I've recently started playing Platinum. If this project goes well, I even have plans to use what I do here as a base to make a Sinnoh demake and maybe even a custom game like ``Unbound``(which is amazing and everyone should play it).

Much like with **Modern Emerald - Worped**, this repository is mainly for me to track my changes, but I also hope it's able to help other people getting into ROM hacking.

---

## ✅ Current Features

I'll try to call out from where a feature was inspired/pulled from. Major feature branches will be linked. Everything will be properly contributed!

### Gameplay Mechanics:
* **Flash1Lucky's [Catch Mode](https://github.com/Flash1Lucky/pokeemerald-expansion/tree/catch-mode-toggle):** It's a Catch Mode, for catching Pokémon easier, you can turn it off in the Options menu.  
* **QoL HMs:** HM moves don't need to be taught, as long as you have the HM, the Badge, and a mon who can use it, you can perform the field move. (Custom Code)
* **Gen IV Poison:** Pokémon will survive poison with 1 hp outside of battles. (Expansion Setting)  
### UI & Visuals:
* **Modified Montblanc's [SwSh Party Menu](https://github.com/montmoguri/pokeemerald-expansion/tree/swsh_party_menu):** I really like how SMOOTH Mont's SwSh stuff is, so I gave his SwSh Party Menu a makeover to fit more with the Gen3/Gen4 look I go for. Also added a Dynamic PokéBall next to each Pokémon, displaying the correct ball they are in. I'm in love with how this looks and works. I also added a small tab that pokes out from under the PokéBall to indicate when a Pokémon is infected with Pokérus.  
* **Modified Montblanc's [SwSh Storage System](https://github.com/montmoguri/pokeemerald-expansion/tree/swsh_storage_system):** Another great feature that I liked the utility of. I've modified it quite a bit, visually matching it to my own FRLG-inspired UI, adding an indicator for current cursor mode, and backporting the Platinum wallpapers. Due to how the Info Panel was coded, I wasn't able to modify that too much, but I also feel like the information it provides is really useful, so I did spice it up a little, even adding a Pokérus indicator to it so you can tell at a glance which Pokémon in your box are infected!!  
* **Modified Montblanc's [SwSh Summary Screen](https://github.com/montmoguri/pokeemerald-expansion/tree/swsh_summary_screen):** My original **Modern Emerald - Worped** project utilized a modified version of Rave's Summary Screen, and I really liked Mont's updates to that. This one is MUCH BETTER than my old one! I'm really happy with how this came out, it has support for Dynamaxed pokemon and mint natures. I did slim it down from Mont's default, removing the Memo page and folding that information into the Info and Skills pages. It really completes the look though!  
* **Modified [Options Plus Menu](https://github.com/pret/pokeemerald/wiki/New-Options-Plus-%E2%80%90-Multipage-Options-Menu-with-Faster-Text,-HP%E2%80%90EXP-Bar-Speeds-and-Metric):** To facilitate some of my desired features, I ported the Options Plus menu over to Expansion, thanks to Miri from Pokémon Hearth for pointing me towards that repo since I kept getting compile errors. I used Archie and Mudskip's version as my base, and modified the menu options and tileset to match my other stuff.(Currently Difficulty, Match Call, and Auto Run don't do anything.)
    * I also used this as a base for the New Game Options menu. (Currently none of the Game Rules do anything.)  
* **Text Decapitalization:** Working on making text across the game decapped. (Ongoing WIP)  
### Other QoL:
* **PokéBall Swapping:** You can use Pokéballs in the bag to rehome your Pokémon. (Lunos Tutorial)  

---

## 🗺️ Future Features
This is a neatened up version of my awful Google Doccy that was just one big list of bullet points. This is more of "wish list" than things I can actually commit to lol.

### Features that are either new to `pokeemerald-worped-ex` or are from `pokeemerald-worped`:
* **1-Floor Pokémon Center:** Total removal of the multiplayer second level, re-mapping the space into a custom NPC counter for simulated offline Wonder Trades using Modern Emerald's set up. (In progress)
* **Purple Battle HUD:** I'm going to make the battle hud purple again guys.
* **Advanced Battle Info Pane:** Real-time type effectiveness indicators, target types, and STAB tracking.
* **Main Menu UI:** Archie & Mudskip's awesome main menu that shows your party.
* **Custom UI Screens:** Porting over my custom FRLG-inspired PokéDex layout.
* **Expanded Starter Selection:** Archie & Mudskip's beautiful feature branch with some custom code to use the chosen shiny odds and make gender 50/50.
* **Custom DexNav:** Custom DexNav logic, Modern Emerald Worped focused on hunting shiny Pokémon, but I do want to change this to also include finding good abilities. (ghoulslash) (Also wondering if I can hook DexNav into the overworld Pokémon spawns so that if you're 'hunting' a specific mon, ONLY that mon will spawn...)
* **Item Pickup Headers:** Pop-up header notifications when items are found or received. (Expansion has hooks for this)
* **Map Changes:** Learning Porymap to add little environmental changes to Hoenn.
* **Mega Evolutions:** Integration of Mega Evolution mechanics.
* **Delta-ish Episode:** I WANT TO GO TO SPACE AND FIGHT THAT BITCH ASS ALIEN. This will be a very LIGHT telling of the Delta Episode.

### Features I like from **Modern Emerald**:
* **QoL Movement & Atmosphere:** Skippable Walley Catch Tutorial, dynamic ambient lighting (windows/lamps).
* **Nature Mints:** Already implemented in Expansion, need to make these purchasable at the Pretty Petal Flower Shop.
* **Unique Surf Animations:** Custom overworld sprites and unique animations while surfing across Hoenn's waterways.
* **Choosable Shiny Odds:** Initial options menu selection to lock in custom shiny rates (1/512(Fun), 1/1024, 1/2048, 1/4096(Modern), 1/8192(GenIII))
* **Level Cap & Difficulty:** Opt-in progression caps & difficulty to keep encounters engaging (Expansion has hooks for this, just need to learn how to use em)
* **Enhanced Rewards:** Rock Smash drops updated to regularly yield Shards and Heart Scales.
* **Unified Bike System:** Merging the Mach and Acro bikes into a single, seamless key item experience. (There is a Lunos tutorial for this!)
* **World Events:** Custom map changes and event overhauls for the Regis, Hoenn Starters, Rival naming, and the new extra legendary encounters.
---

## 📍 Pokémon & Items
This section details my plans for Pokémon additions, pulling from how I had things in `pokeemerald-worped` and how **Modern Emerald** does things. I'll have to make a big document with detailed lists for each route eventually...

### **Modern Emerald** Wild Encounters:
**Modern Emerald** added really cool Modern encounters that sprinkled Gen I & II Pokémon around Hoenn. I really liked that and plan on bringing that over eventually when I start working on maps properly.

### `pokeemerald-worped` Wild Envounters:
This first table contains Pokémon I had manually added to `pokeemerald-worped`. With `pokeemerald-expansion` they're already in the game with their proper learnsets. They'll be added when I start working on maps properly.

| Pokémon Family | Planned Location (Base Form) | Notes |
| :--- | :--- | :--- |
| **Buneary** | Routes 104 & 117 | |
| **Emolga** | Routes 110 & 116 | |
| **Minccino** | Petalburg Woods & Route 110 | |
| **Stufful** | Route 123 | |
| **Mimikyu** | Mt. Pyre (Exterior & Summit) | Planned restriction to Night-Only via custom engine flags. |
| **Hatenna** | Routes 114 & 117 | |
| **Snom** | Shoal Cave (Ice Room) | Part of the Ice Room regional mini-hub overhaul. |
| **Lechonk** | Routes 101 & 102 | |
| **Paldean Wooper** | Route 113 | Custom Description |

This second table contains Pokémon I wanted to add, but it got to be a pain in the ass and instead I decided to move my project over to `pokeemerald-expansion` because that was so much easier... I really like how even when a Pokémon is disabled in `expansion`, the PokéDex number is still taken, so there won't be any wonky numbers like in `pokeemerald-worped`!

| Pokémon Family | Planned Location (Base Form) | Notes |
| :--- | :--- | :--- |
| **Goomy** | *TBD* | Tracking for future route mapping. |
| **Tinkatink** | *TBD* | Tracking for future route mapping. |
| **Morpeko** | *TBD* | Tracking for future route mapping. |
| **Pachirisu** | *TBD* | Tracking for future route mapping. |
| **Alolan Sandshrew** | *TBD* | Tracking for future route mapping. |
| **Espurr** | *TBD* | Tracking for future route mapping. |

### PokéBalls:
* **Dream Ball:** Will be obtainable once per day from a unique Nurse NPC stationed on the Lilycove City Shoreline after showing her a Pokémon that knows a sleep-inducing move.
* **Heal Ball:** To be added as a custom inventory line item at the Lavaridge Town PokéMart.
* **Love Ball:** To be added as a custom inventory line item at the Verdanturf Town PokéMart.
* **Other Balls:** Might add other balls since expansion adds quite a lot.

### Map Changes:
The Safari Zone and Shoal Cave are going to be completely redone so they can act as lil hubs for non-Hoenn Pokémon. This is a very abstract idea though, nothing I'm commited to.

---

## 🤝 Credits

This project would be impossible without the massive foundations provided by the decompilation community:

* **`pokeemerald-expansion` (v1.16.1)** - Developed and maintained by the **ROM Hacking Hideout (RHH)** group.
* **Modern Emerald (v3.3.1)** - Developed by **Resetes12**.
* **Feature Branches & Coding References:** Very grateful to **ghoulslash**, **Flash1Lucky**, **TheXaman**, **RavePossum**, **Team Aqua's Hideout**, and **many more** for their public code repositories and documentation templates for features I've pulled in.
* **RHH Discord:** The Discord for RHH is full of incredibly helpful people and working together on stuff has been really fun!

* I don't mind people pulling features I've modified (Mont's SwSh stuff) but please do credit myself(worpbane/Brendan) for the layout and Mont for the base features(and Rave for the Summary Screen).  

---

## 📝 Note on Generative AI

*   **100% Human Code & (Bad) Design:** Every script edit, C function, map change, and feature in **`pokeemerald-worped-ex`** is entirely custom-configured / hand-coded by me. No generative AI models are utilized for code creation or game asset generation.

---

Based off RHH's pokeemerald-expansion 1.16.1 https://github.com/rh-hideout/pokeemerald-expansion/