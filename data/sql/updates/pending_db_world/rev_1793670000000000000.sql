-- [core670] Imp Firebolt: 0.79 of the Imp's spell power per Rawr 2.3.23 Rawr.Warlock Pets.cs:478
-- (Imp: SpecialDamagePerSpellPower = .79f; was 0.714 = the DBC EffectBonusMultiplier). b266 rfc-h 5E: the
-- destruction warlock's Firebolt per cast was 0.85 of Rawr's (coefficient, base and Improved Imp together).
UPDATE `spell_bonus_data` SET `direct_bonus` = 0.79
WHERE `entry` IN (3110, 7799, 7800, 7801, 7802, 11762, 11763, 27267, 47964);
