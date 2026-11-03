-- Rogue per Rawr 2.3.23 Rawr.Rogue
-- Seal Fate: a crit of either Mutilate hand gives the extra combo point (CalculationsRogue.cs:549); the mask held
-- only the main-hand half (48665, [1] 0x2), now also the off-hand half (48664, [1] 0x4); the 500 ms cooldown stays
UPDATE `spell_proc` SET `SpellFamilyMask1` = 6 WHERE `SpellId` = -14186;
-- Instant Poison: 0.2 procs per weapon second / 1.4 = 8.57 PPM, the rate Rawr's damage uses
-- (RogueRotationCalculator.cs:299); the 8.53 of core312 (CalculationsRogue.cs:963) only feeds a trinket estimate
UPDATE `spell_enchant_proc_data` SET `PPMChance` = 8.57 WHERE `entry` IN (323, 324, 325, 623, 624, 625, 2641, 3768, 3769);
