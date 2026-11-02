-- Shield Slam (all ranks): patch 3.3.2 "Threat from Shield Slam was increased by 30%"
UPDATE `spell_threat` SET `pctMod` = 1.3 WHERE `entry` IN (23922, 23923, 23924, 23925, 25258, 30356, 47487, 47488);
