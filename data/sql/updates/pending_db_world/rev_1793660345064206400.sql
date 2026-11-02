-- Black Arrow ticks scale with 0.023 RAP (wowsims sim/hunter/black_arrow.go:63, 11.5 % over 5 ticks)
UPDATE `spell_bonus_data` SET `ap_dot_bonus` = 0.023 WHERE `entry` IN (3674, 63668, 63669, 63670, 63671, 63672);
-- Explosive Shot hits scale with 0.14 RAP (wowsims sim/hunter/explosive_shot.go:67, tooltip $RAP*0.14)
UPDATE `spell_bonus_data` SET `ap_bonus` = 0.14 WHERE `entry` = 53352;
