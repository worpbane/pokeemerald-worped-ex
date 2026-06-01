# Pokémon Emerald – Worped Ex v1.0

This is a personal fork of **`pokeemerald-expansion` 1.16.0** by the ROM Hacking Hideout (RHH). 

**🧪 Testing On:** New Nintendo 3DS w/ `open_agb_firm` & Steam Deck (`mGBA`)

---

## 🧬 Project Overview
The original *Modern Emerald – Worped* was built on top of Resetes12's Modern Emerald project. This repository represents a complete port and overhaul, shifting everything over to the modern, highly optimized **`pokeemerald-expansion`** engine. 

The goal isn't to create a massive, bloated overhaul, but to build my absolute ideal version of Hoenn. This includes clean quality-of-life systems, a fully personalized battle UI, and a small roster of late-generation Pokémon that I personally think are cute and belong in Hoenn.

---

## 🗺️ Feature Architecture & Implementation Wish List

To keep the project scope focused, the following architecture serves as the active roadmap and development wish list for **Worped Ex**. This outlines the specific features I hope to fully implement, blending established systems from the original *Modern Emerald* project with custom, ground-up mechanics unique to *Worped*.

> 💡 **Note Engine Features:** I am fully aware that many of these mechanics (such as Mega Evolutions, the Physical/Special split, and following Pokémon) are already natively included as baseline code structures inside `pokeemerald-expansion`. My goal with this project isn't to re-write them, but to capitalize on them, configure the engine switches properly, and write the custom overworld event scripts to integrate them into my vision of Hoenn!

### 🌟 Worped Ex Additions (Planned Features)
* **1-Floor Pokémon Center:** Total removal of the multiplayer second level, re-mapping the space into a custom NPC counter for simulated offline Wonder Trades.
* **Purple Battle HUD:** A custom battle hud with purple themeing.
* **Advanced Battle Info Pane:** Real-time type effectiveness indicators (Arrows/X), target types, and STAB tracking.
* **Main Menu UI:** A custom startup dashboard that dynamically displays your active party (Archie & Mudskip).
* **Custom UI Screens:** Porting over my custom FRLG-inspired Summary Screen (with active IV/EV trackers), and custom PokéDex layout.
* **Expanded Starter Selection:** Custom selection logic at the start of the game. (Archie & Mudskip)
* **Catch Mode & DexNav:** Streamlined catching sequences and custom DexNav logic. (FlashLucky & ghoulslash)
* **Item Pickup Headers:** Pop-up header notifications when items are found or received.
* **Map Changes:** Map re-layouts across select routes and cities to smooth out overworld navigation.
* **Mega Evolutions:** Integration of Mega Evolution mechanics.

### 🔄 Modern Emerald Ports (Planned Feature Adaptations)
* **QoL Field Movements & Atmosphere:** Run everywhere automatically, faster Nurse Joy text cycles, day/night cycles with dynamic ambient lighting (windows/lamps), and footstep footprints in grass and sand.
* **Unique Surf Animations:** Custom overworld sprites and unique animations while surfing across Hoenn's waterways.
* **Smart HMs:** Natively utilize HM field moves without needing to clutter party movesets (requires the HM and a capable Pokémon in party).
* **Dynamic Pokéballs:** Ability to swap a caught Pokémon's Pokéball directly from the bag interface.
* **Choosable Shiny Odds:** Initial options menu selection to lock in custom shiny rates (1/512, 1/1024, 1/2048, 1/4096, 1/8192).
* **Level Cap & Difficulty:** Built-in progression caps to keep boss encounters engaging.
* **Enhanced Rewards:** Rock Smash drops updated to regularly yield Shards and Heart Scales.
* **Unified Bike System:** Merging the Mach and Acro bikes into a single, seamless key item experience.
* **World Events:** Custom map changes and event overhauls for the Regis, Hoenn Starters, Rival naming, and buffed legendary encounters.
---

## 📍 Planned World Distribution (Upcoming)

*Note: The data structures for these items and Pokémon are natively handled by the RHH expansion engine, but their physical locations, shop integrations, and overworld event scripts are currently being mapped out for implementation below.*

### 🗺️ Planned Wild Encounters & Regional Reworks
* **Shoal Cave & Safari Zone Updates:** Reworking Shoal Cave and the Safari Zone into dedicated mini-hubs to act as natural habitats for some out-of-region Pokémon.

| Pokémon | Planned Location (Base Form) | Notes |
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
* **Feature Branches & Coding References:** Huge thanks to **ghoulslash**, **flashlucky**, **TheXaman**, **RavePossum**, and **Team Aqua's Hideout** for their public code repositories and documentation templates.