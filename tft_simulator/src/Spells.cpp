#include "Spells.hpp"
#include <vector>

void castBlitzcrankSpell(Champion* caster, Champion* target, Board& board) {
    float dmg1 = 150.0f;
    float dmg2 = 175.0f;
    if (caster->getStarLevel() == 2) { dmg1 = 225.0f; dmg2 = 265.0f; }
    else if (caster->getStarLevel() == 3) { dmg1 = 999.0f; dmg2 = 5000.0f; }
    
    double actualDmg1 = target->takeDamage(dmg1, false, true);
    caster->addDamageDealt(actualDmg1);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 3, target->getOwnerId());
    for (Champion* enemy : enemies) {
        double actualDmg2 = enemy->takeDamage(dmg2, false, true);
        caster->addDamageDealt(actualDmg2);
    }
}

void castFioraSpell(Champion* caster, Champion* target, Board& board) {
    float dmgBase = 40.0f;
    float healBase = 200.0f;
    if (caster->getStarLevel() == 2) { dmgBase = 60.0f; healBase = 250.0f; }
    else if (caster->getStarLevel() == 3) { dmgBase = 777.0f; healBase = 999.0f; }
    
    float totalDamage = dmgBase * 6.0f;
    
    double actualDmg = target->takeDamage(totalDamage, true, true);
    caster->addDamageDealt(actualDmg);
    
    caster->heal(totalDamage * 0.15f);
    
    std::vector<Champion*> allies = board.getChampionsInRange(caster->getX(), caster->getY(), 2, caster->getOwnerId());
    for (Champion* ally : allies) {
        ally->heal(healBase);
    }
}

void castJhinSpell(Champion* caster, Champion* target, Board& board) {
    float dmg = 45.0f;
    if (caster->getStarLevel() == 2) dmg = 68.0f;
    else if (caster->getStarLevel() == 3) dmg = 688.0f;
    
    double actualDmg1 = target->takeDamage(dmg * 3.0f, true, true);
    caster->addDamageDealt(actualDmg1);
    
    double actualDmg2 = target->takeDamage(dmg * 2.44f, true, true);
    caster->addDamageDealt(actualDmg2);
}

void castMorganaSpell(Champion* caster, Champion* target, Board& board) {
    float shield = 250.0f;
    float dotDmg = 50.0f;
    float burstDmg = 240.0f;
    if (caster->getStarLevel() == 2) { shield = 300.0f; dotDmg = 75.0f; burstDmg = 360.0f; }
    else if (caster->getStarLevel() == 3) { shield = 4000.0f; dotDmg = 1500.0f; burstDmg = 4000.0f; }
    
    caster->heal(shield); 
    
    std::vector<Champion*> enemies = board.getChampionsInRange(caster->getX(), caster->getY(), 3, target->getOwnerId());
    int count = 0;
    for (Champion* enemy : enemies) {
        if (count >= 3) break;
        double actualDmg = enemy->takeDamage((dotDmg * 5.0f) + burstDmg, false, true);
        caster->addDamageDealt(actualDmg);
        count++;
    }
}

void castShenSpell(Champion* caster, Champion* target, Board& board) {
    float shield = 321.0f + (caster->getMaxHp() * 0.08f);
    float dmg = 40.0f;
    if (caster->getStarLevel() == 2) { shield = 473.0f + (caster->getMaxHp() * 0.08f); dmg = 60.0f; }
    else if (caster->getStarLevel() == 3) { shield = 3767.0f + (caster->getMaxHp() * 0.08f); dmg = 777.0f; }
    
    caster->heal(shield);
    
    double actualDmg = target->takeDamage(dmg * 3.0f, false, true);
    caster->addDamageDealt(actualDmg);
}

void castVexSpell(Champion* caster, Champion* target, Board& board) {
    float dmg = 130.0f;
    if (caster->getStarLevel() == 2) dmg = 195.0f;
    else if (caster->getStarLevel() == 3) dmg = 1000.0f;
    
    double actualDmg = target->takeDamage(dmg * 3.0f, false, true);
    caster->addDamageDealt(actualDmg);
}

void castKarmaSpell(Champion* caster, Champion* target, Board& board) {
    float dmgSplit = 570.0f;
    float dmgBonus = 150.0f;
    int targetsCount = 2;
    if (caster->getStarLevel() == 2) { dmgSplit = 855.0f; dmgBonus = 225.0f; }
    else if (caster->getStarLevel() == 3) { dmgSplit = 5000.0f; dmgBonus = 1000.0f; targetsCount = 4; }
    
    double actualDmg1 = target->takeDamage((dmgSplit / (targetsCount + 1)) + dmgBonus, false, true);
    caster->addDamageDealt(actualDmg1);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 4, target->getOwnerId());
    int hitCount = 0;
    for (Champion* enemy : enemies) {
        if (enemy == target) continue;
        if (hitCount >= targetsCount) break;
        
        double actualDmg2 = enemy->takeDamage(dmgSplit / (targetsCount + 1), false, true);
        caster->addDamageDealt(actualDmg2);
        
        hitCount++;
    }
}

void castKindredSpell(Champion* caster, Champion* target, Board& board) {
    float wolfDmg = 125.0f;
    float arrowDmg = caster->getAd() * 0.6f;
    int arrowTargets = 3;
    if (caster->getStarLevel() == 2) { wolfDmg = 190.0f; arrowDmg = caster->getAd() * 0.9f; }
    else if (caster->getStarLevel() == 3) { wolfDmg = 1000.0f; arrowDmg = caster->getAd() * 6.0f; arrowTargets = 5; }
    
    double actualWolfDmg = target->takeDamage(wolfDmg, false, true);
    caster->addDamageDealt(actualWolfDmg);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(caster->getX(), caster->getY(), 6, target->getOwnerId());
    int hitCount = 0;
    for (Champion* enemy : enemies) {
        if (hitCount >= arrowTargets) break;
        
        double actualArrowDmg = enemy->takeDamage(arrowDmg, true, true);
        caster->addDamageDealt(actualArrowDmg);
        
        hitCount++;
    }
}

void castNamiSpell(Champion* caster, Champion* target, Board& board) {
    float mainDmg = 410.0f;
    float bounceDmg = 110.0f;
    if (caster->getStarLevel() == 2) { mainDmg = 615.0f; bounceDmg = 165.0f; }
    else if (caster->getStarLevel() == 3) { mainDmg = 5000.0f; bounceDmg = 1000.0f; }
    
    double actualMain = target->takeDamage(mainDmg, false, true);
    caster->addDamageDealt(actualMain);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 4, target->getOwnerId());
    int hitCount = 0;
    for (Champion* enemy : enemies) {
        if (enemy == target) continue;
        if (hitCount >= 3) break;
        
        double actualBounce = enemy->takeDamage(bounceDmg, false, true);
        caster->addDamageDealt(actualBounce);
        
        hitCount++;
    }
}

void castRivenSpell(Champion* caster, Champion* target, Board& board) {
    float shield = 100.0f;
    float dmg = 90.0f;
    if (caster->getStarLevel() == 2) { shield = 150.0f; dmg = 135.0f; }
    else if (caster->getStarLevel() == 3) { shield = 1200.0f; dmg = 1000.0f; }
    
    caster->heal(shield);
    
    double actualDmg = target->takeDamage(dmg, true, true);
    caster->addDamageDealt(actualDmg);
}

void castAuroraSpell(Champion* caster, Champion* target, Board& board) {
    float mainDmg = 80.0f;
    float splitDmg = 370.0f;
    if (caster->getStarLevel() == 2) { mainDmg = 120.0f; splitDmg = 555.0f; }
    else if (caster->getStarLevel() == 3) { mainDmg = 190.0f; splitDmg = 890.0f; }
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 2, target->getOwnerId());
    int count = enemies.size();
    if (count == 0) count = 1;
    float dmgPerTarget = mainDmg + (splitDmg / count);
    
    for (Champion* enemy : enemies) {
        double actualDmg = enemy->takeDamage(dmgPerTarget, false, true);
        caster->addDamageDealt(actualDmg);
    }
}

void castIllaoiSpell(Champion* caster, Champion* target, Board& board) {
    float shield = 450.0f;
    float drain = 55.0f;
    float slamDmg = 80.0f;
    if (caster->getStarLevel() == 2) { shield = 525.0f; drain = 85.0f; slamDmg = 120.0f; }
    else if (caster->getStarLevel() == 3) { shield = 650.0f; drain = 130.0f; slamDmg = 180.0f; }
    
    caster->heal(shield + (drain * 3.0f));
    
    std::vector<Champion*> enemies = board.getChampionsInRange(caster->getX(), caster->getY(), 2, target->getOwnerId());
    for (Champion* enemy : enemies) {
        double actualDmg = enemy->takeDamage(slamDmg, false, true);
        caster->addDamageDealt(actualDmg);
    }
}

void castKaisaSpell(Champion* caster, Champion* target, Board& board) {
    float missileDmg = 35.0f;
    if (caster->getStarLevel() == 2) missileDmg = 53.0f;
    else if (caster->getStarLevel() == 3) missileDmg = 84.0f;
    
    float totalDmg = missileDmg * 16.0f;
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 2, target->getOwnerId());
    int count = enemies.size();
    
    if (count == 0) {
        double actualDmg = target->takeDamage(totalDmg, true, true);
        caster->addDamageDealt(actualDmg);
    } else {
        float dmgPerTarget = totalDmg / count;
        for (Champion* enemy : enemies) {
            double actualDmg = enemy->takeDamage(dmgPerTarget, true, true);
            caster->addDamageDealt(actualDmg);
        }
    }
}

void castMaokaiSpell(Champion* caster, Champion* target, Board& board) {
    float dmg = 100.0f;
    if (caster->getStarLevel() == 2) dmg = 150.0f;
    else if (caster->getStarLevel() == 3) dmg = 225.0f;
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 1, target->getOwnerId());
    for (Champion* enemy : enemies) {
        double actualDmg = enemy->takeDamage(dmg, false, true);
        caster->addDamageDealt(actualDmg);
    }
}

void castOrnnSpell(Champion* caster, Champion* target, Board& board) {
    float shield = 100.0f;
    float dmg = 180.0f;
    if (caster->getStarLevel() == 2) { shield = 150.0f; dmg = 270.0f; }
    else if (caster->getStarLevel() == 3) { shield = 500.0f; dmg = 430.0f; }
    
    caster->heal(shield);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(caster->getX(), caster->getY(), 2, target->getOwnerId());
    for (Champion* enemy : enemies) {
        double actualDmg = enemy->takeDamage(dmg, false, true);
        caster->addDamageDealt(actualDmg);
    }
}

void castSamiraSpell(Champion* caster, Champion* target, Board& board) {
    float dmg = 360.0f;
    if (caster->getStarLevel() == 2) dmg = 540.0f;
    else if (caster->getStarLevel() == 3) dmg = 860.0f;
    
    double actualDmg = target->takeDamage(dmg, true, true);
    caster->addDamageDealt(actualDmg);
}

void castAkaliSpell(Champion* caster, Champion* target, Board& board) {
    float mainDmg = 38.0f;
    float secondaryDmg = 15.0f;
    if (caster->getStarLevel() == 2) { mainDmg = 56.0f; secondaryDmg = 22.0f; }
    else if (caster->getStarLevel() == 3) { mainDmg = 89.0f; secondaryDmg = 36.0f; }
    
    double actualMain = target->takeDamage(mainDmg, true, true);
    caster->addDamageDealt(actualMain);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 3, target->getOwnerId());
    int hits = 0;
    for (Champion* enemy : enemies) {
        if (enemy == target) continue;
        if (hits >= 4) break;
        
        double actualSecondary = enemy->takeDamage(secondaryDmg, true, true);
        caster->addDamageDealt(actualSecondary);
        
        hits++;
    }
}

void castGwenSpell(Champion* caster, Champion* target, Board& board) {
    float dmg = 145.0f;
    float coneDmg = 75.0f;
    if (caster->getStarLevel() == 2) { dmg = 220.0f; coneDmg = 110.0f; }
    else if (caster->getStarLevel() == 3) { dmg = 380.0f; coneDmg = 190.0f; }
    
    double actualDmg = target->takeDamage(dmg, false, true);
    caster->addDamageDealt(actualDmg);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 2, target->getOwnerId());
    for (Champion* enemy : enemies) {
        if (enemy != target) {
            double actualCone = enemy->takeDamage(coneDmg, false, true);
            caster->addDamageDealt(actualCone);
        }
    }
}

void castJinxSpell(Champion* caster, Champion* target, Board& board) {
    float rocketDmg = 32.0f;
    if (caster->getStarLevel() == 2) rocketDmg = 49.0f;
    else if (caster->getStarLevel() == 3) rocketDmg = 72.0f;
    
    float totalDmg = rocketDmg * 16.0f;
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 3, target->getOwnerId());
    int count = enemies.size();
    
    if (count == 0) {
        double actualDmg = target->takeDamage(totalDmg, true, true);
        caster->addDamageDealt(actualDmg);
    } else {
        float dmgPerTarget = totalDmg / count;
        for (Champion* enemy : enemies) {
            double actualDmg = enemy->takeDamage(dmgPerTarget, true, true);
            caster->addDamageDealt(actualDmg);
        }
    }
}

void castMilioSpell(Champion* caster, Champion* target, Board& board) {
    float dmg1 = 255.0f;
    float dmg2 = 85.0f;
    if (caster->getStarLevel() == 2) { dmg1 = 380.0f; dmg2 = 130.0f; }
    else if (caster->getStarLevel() == 3) { dmg1 = 575.0f; dmg2 = 190.0f; }
    
    double actualDmg1 = target->takeDamage(dmg1, false, true);
    caster->addDamageDealt(actualDmg1);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 4, target->getOwnerId());
    for (Champion* enemy : enemies) {
        if (enemy != target) {
            double actualDmg2 = enemy->takeDamage(dmg2, false, true);
            caster->addDamageDealt(actualDmg2);
            break; 
        }
    }
}

void castMordekaiserSpell(Champion* caster, Champion* target, Board& board) {
    float shield = 300.0f + (75.0f * 4.0f); 
    float aoeDmg = 45.0f * 4.0f;
    if (caster->getStarLevel() == 2) { shield = 375.0f + (90.0f * 4.0f); aoeDmg = 70.0f * 4.0f; }
    else if (caster->getStarLevel() == 3) { shield = 500.0f + (105.0f * 4.0f); aoeDmg = 100.0f * 4.0f; }
    
    caster->heal(shield); 
    
    std::vector<Champion*> enemies = board.getChampionsInRange(caster->getX(), target->getY(), 1, target->getOwnerId());
    for (Champion* enemy : enemies) {
        double actualAoe = enemy->takeDamage(aoeDmg, false, true);
        caster->addDamageDealt(actualAoe);
    }
}

void castPantheonSpell(Champion* caster, Champion* target, Board& board) {
    float shield = 275.0f + (caster->getMaxHp() * 0.06f);
    float aoeDmg = 30.0f * 4.0f; 
    if (caster->getStarLevel() == 2) { shield = 300.0f + (caster->getMaxHp() * 0.06f); aoeDmg = 45.0f * 4.0f; }
    else if (caster->getStarLevel() == 3) { shield = 500.0f + (caster->getMaxHp() * 0.06f); aoeDmg = 70.0f * 4.0f; }
    
    caster->heal(shield);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(caster->getX(), caster->getY(), 1, target->getOwnerId());
    for (Champion* enemy : enemies) {
        double actualAoe = enemy->takeDamage(aoeDmg, true, true);
        caster->addDamageDealt(actualAoe);
    }
}

void castAatroxSpell(Champion* caster, Champion* target, Board& board) {
    float healAmt = 375.0f;
    float dmg = 242.0f;
    if (caster->getStarLevel() == 2) { healAmt = 575.0f; dmg = 362.0f; }
    else if (caster->getStarLevel() == 3) { healAmt = 800.0f; dmg = 500.0f; }
    
    caster->heal(healAmt);
    
    double actualDmg = target->takeDamage(dmg, true, true);
    caster->addDamageDealt(actualDmg);
}

void castBriarSpell(Champion* caster, Champion* target, Board& board) {
    float dmg = 130.0f;
    if (caster->getStarLevel() == 2) dmg = 195.0f;
    else if (caster->getStarLevel() == 3) dmg = 320.0f;
    
    if (target->getArmor() >= 40) {
        dmg *= 1.5f;
    }
    
    double actualDmg = target->takeDamage(dmg, true, true);
    caster->addDamageDealt(actualDmg);
}

void castCaitlynSpell(Champion* caster, Champion* target, Board& board) {
    float headshotDmg = 190.0f;
    if (caster->getStarLevel() == 2) headshotDmg = 285.0f;
    else if (caster->getStarLevel() == 3) headshotDmg = 555.0f;
    
    double actualDmg = target->takeDamage(headshotDmg, true, true);
    caster->addDamageDealt(actualDmg);
}

void castChogathSpell(Champion* caster, Champion* target, Board& board) {
    float dmg = 266.0f;
    float hpBuff = 30.0f;
    if (caster->getStarLevel() == 2) { dmg = 391.0f; hpBuff = 40.0f; }
    else if (caster->getStarLevel() == 3) { dmg = 601.0f; hpBuff = 70.0f; }
    
    Champion* lowestHpEnemy = target;
    std::vector<Champion*> enemies = board.getChampionsInRange(caster->getX(), caster->getY(), 2, target->getOwnerId());
    for (Champion* enemy : enemies) {
        if (enemy->getHp() < lowestHpEnemy->getHp()) {
            lowestHpEnemy = enemy;
        }
    }
    
    double actualDmg = lowestHpEnemy->takeDamage(dmg, false, true);
    caster->addDamageDealt(actualDmg);
    
    caster->heal(hpBuff * 2.0f); 
}

void castEzrealSpell(Champion* caster, Champion* target, Board& board) {
    float dmg = 174.0f;
    if (caster->getStarLevel() == 2) dmg = 261.0f;
    else if (caster->getStarLevel() == 3) dmg = 397.0f;
    
    double actualDmg = target->takeDamage(dmg, true, true);
    caster->addDamageDealt(actualDmg);
}

void castLissandraSpell(Champion* caster, Champion* target, Board& board) {
    float dmg = 250.0f;
    float aoeDmg = 50.0f;
    if (caster->getStarLevel() == 2) { dmg = 375.0f; aoeDmg = 75.0f; }
    else if (caster->getStarLevel() == 3) { dmg = 600.0f; aoeDmg = 115.0f; }
    
    double actualSpellDamage = target->takeDamage(dmg, false, true);
    caster->addDamageDealt(actualSpellDamage);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(target->getX(), target->getY(), 1, target->getOwnerId());
    for (Champion* enemy : enemies) {
        if (enemy != target) {
            double actualAoe = enemy->takeDamage(aoeDmg, false, true);
            caster->addDamageDealt(actualAoe);
        }
    }
}

void castNasusSpell(Champion* caster, Champion* target, Board& board) {
    float hpBuff = 250.0f;
    float aoeDmg = 44.0f * 6.0f; 
    if (caster->getStarLevel() == 2) { hpBuff = 350.0f; aoeDmg = 70.0f * 6.0f; }
    else if (caster->getStarLevel() == 3) { hpBuff = 550.0f; aoeDmg = 115.0f * 6.0f; }
    
    caster->heal(hpBuff);
    
    std::vector<Champion*> enemies = board.getChampionsInRange(caster->getX(), caster->getY(), 1, target->getOwnerId());
    for (Champion* enemy : enemies) {
        double actualAoe = enemy->takeDamage(aoeDmg, false, true);
        caster->addDamageDealt(actualAoe);
    }
}