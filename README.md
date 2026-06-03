# Pokémon Emerald – Worped Ex v1.0

This is a personal fork of **`pokeemerald-expansion` 1.16.0** by the ROM Hacking Hideout (RHH). 

**🧪 Testing On:** New Nintendo 3DS w/ `open_agb_firm` & Steam Deck w/ `mGBA`

---

## 🧬 Project Overview
The original *Modern Emerald – Worped* was built on top of Resetes12's Modern Emerald project. This repository represents a complete port and overhaul, shifting everything over to the modern, highly optimized **`pokeemerald-expansion`** engine. 

The goal isn't to create a massive, bloated overhaul, but to build my absolute ideal version of Hoenn. This includes clean quality-of-life systems, a fully personalized UI, and a small roster of late-generation Pokémon that I personally like. This isn't a difficulty-hack, it's a fun-hack! 

I grew up playing Gen 3, and it remains my favorite generation to this day (I actually never played Gen 4 through 7 and am only just recently playing Platinum with a plan to move on to Black after). The main reason this repository exists is so I can easily version manage the changes I'm making to the region I love. This will also eventually serve as the launchpad for two separate projects I have planned down the road: a Sinnoh demake, and an entirely original custom region inspired by the world-building of hacks like *Pokémon Unbound*.

---

## ✅ Currently Implemented Features

This section tracks everything that has been successfully coded into the active **Worped Ex** build. 

### ⚙️ Engine Baselines & QoL Mechanics
* **Gen 4 Poison Survival:** Poisoned Pokémon drop down to 1 HP in the overworld and are cured, preventing blackouts while exploring. *(Native Expansion engine configuration)*
* **Smart HMs & Automatic Field Triggers:** Bypasses the need to clutter active battle movesets with utility actions. If the player possesses the physical HM item and has defeated the required Gym Leader, any compatible Pokémon in the party can instantly perform the move in the overworld! Also removed the prompt menus for Cut, Strength, and Rock Smash. Simply walking up to an obstacle and pressing A automatically triggers the action seamlessly if you're able to. *(Custom Framework).*
* **Text Decapitalization:** Every town name, map layout, string array identifier, possessive marker, and intro dialogue line has been systematically decapitalized into clean, modern Title Case. *(Work in Progress)*

---

## 🗺️ Feature Architecture & Implementation Wish List

To keep the project scope focused, the following architecture serves as the active roadmap and development wish list for **Worped Ex**. This outlines the specific features I hope to fully implement, blending established systems from the original *Modern Emerald* project with custom, ground-up mechanics unique to *Worped*.

> 💡 **Note on Engine Features:** I am fully aware that a few mechanics from `pokeemerald-worped` are already natively included as baseline code structures inside `pokeemerald-expansion`. This list is more of a reference for me to keep track of my progress.

### 🌟 Worped Ex Additions (Planned Features)
* **1-Floor Pokémon Center:** Total removal of the multiplayer second level, re-mapping the space into a custom NPC counter for simulated offline Wonder Trades.
* **Purple Battle HUD:** A custom battle hud with purple themeing.
* **Advanced Battle Info Pane:** Real-time type effectiveness indicators (Arrows/X), target types, and STAB tracking.
* **Main Menu UI:** A custom startup dashboard that dynamically displays your active party (Archie & Mudskip).
* **Custom UI Screens:** Porting over my custom FRLG-inspired Summary Screen (with active IV/EV trackers), and custom PokéDex layout.
* **Expanded Starter Selection:** Custom selection logic at the start of the game. (Archie & Mudskip)
* **Catch Mode & DexNav:** Custom DexNav logic, Modern Emerald Worped focused on hunting shinies, but I do want to change this to also include finding good abilities. (FlashLucky & ghoulslash) (Also wondering if I can hook DexNav into the overworld Pokémon spawns so that if you're 'hunting' a specific mon, ONLY that mon will spawn...)
* **Item Pickup Headers:** Pop-up header notifications when items are found or received.
* **Map Changes:** Map layout changes across select routes and cities to smooth out overworld navigation and add a bit of character in places.
* **Mega Evolutions:** Integration of Mega Evolution mechanics.

### 🔄 Modern Emerald Ports (Planned Feature Adaptations)
* **QoL Movement & Atmosphere:** Run everywhere automatically, faster Nurse Joy text cycles, skippable Walley Catch Tutorial, dynamic ambient lighting (windows/lamps).
* **Unique Surf Animations:** Custom overworld sprites and unique animations while surfing across Hoenn's waterways. (Not actually sure how to do this one).
* **Dynamic Pokéballs:** Ability to swap a caught Pokémon's Pokéball directly from the bag interface.
* **Choosable Shiny Odds:** Initial options menu selection to lock in custom shiny rates (1/512, 1/1024, 1/2048, 1/4096, 1/8192).
* **Level Cap & Difficulty:** Opt-in progression caps & difficulty to keep encounters engaging (Expansion has hooks for this).
* **Enhanced Rewards:** Rock Smash drops updated to regularly yield Shards and Heart Scales.
* **Unified Bike System:** Merging the Mach and Acro bikes into a single, seamless key item experience.
* **World Events:** Custom map changes and event overhauls for the Regis, Hoenn Starters, Rival naming, and buffed legendary encounters.
---

## 📍 Planned World Distribution (Upcoming)

*Note: The data structures for these items and Pokémon are natively handled by the RHH expansion engine, but their physical locations, shop integrations, and overworld event scripts are currently being mapped out for implementation below.*

### 🗺️ Planned Wild Encounters & Regional Reworks
* **Shoal Cave & Safari Zone Updates:** Reworking Shoal Cave and the Safari Zone into dedicated mini-hubs to act as natural habitats for some out-of-region(Kanto, Johto, Alola, Paldea, etc.) Pokémon. These reworks will require me to figure out how to add new tilesets.... yay.

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
| **Goomy** | *TBD* | Tracking for future route mapping. |
| **Tinkatink** | *TBD* | Tracking for future route mapping. |
| **Morpeko** | *TBD* | Tracking for future route mapping. |
| **Pachirisu** | *TBD* | Tracking for future route mapping. |
| **Alolan Sandshrew** | *TBD* | Tracking for future route mapping. |
| **Espurr** | *TBD* | Tracking for future route mapping. |

### 🛍️ Planned PokéBall Availability
* **Dream Ball:** Will be obtainable once per day from a unique Nurse NPC stationed on the Lilycove City Shoreline after showing her a Pokémon that knows a sleep-inducing move.
* **Heal Ball:** To be added as a custom inventory line item at the Lavaridge Town PokéMart.
* **Love Ball:** To be added as a custom inventory line item at the Verdanturf Town PokéMart.
* **Other Balls:** Might add other balls since expansion adds quite a lot.

---

## 🤝 Credits

This project would be impossible without the massive foundations provided by the decompilation community:

* **`pokeemerald-expansion` (v1.16.0)** - Developed and maintained by the **ROM Hacking Hideout (RHH)** group.
* **Modern Emerald (v3.3.1)** - Developed by **Resetes12**.
* **Feature Branches & Coding References:** Very grateful to **ghoulslash**, **flashlucky**, **TheXaman**, **RavePossum**, and **Team Aqua's Hideout** for their public code repositories and documentation templates for features I've pulled in.
