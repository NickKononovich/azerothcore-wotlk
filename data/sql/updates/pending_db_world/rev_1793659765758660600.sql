-- Hot Streak / Ignite: let the triggered Living Bomb explosion proc them (spell_mage_hot_streak/spell_mage_ignite CheckProc keep other triggered spells out)
UPDATE `spell_proc` SET `AttributesMask` = `AttributesMask` | 0x2 WHERE `SpellId` IN (-44445, -11119);
