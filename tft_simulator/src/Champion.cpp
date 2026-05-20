#include "Champion.hpp"
#include <algorithm>

Champion::Champion(std::string name, int cost, std::vector<std::string> traits, double maxHp, double manaMax, double manaInitial, double ad, double ap, double armor, double mr, double as, int range, Role role)
    : name(name), cost(cost), traits(traits), maxHp(maxHp), hp(maxHp), manaMax(manaMax), mana(manaInitial), ad(ad), ap(ap), armor(armor), mr(mr), as(as), range(range), role(role), starLevel(1), x(-1), y(-1), ownerId(-1), shield(0.0), stunTimer(0.0), manaLockTimer(0.0), buffTimer(0.0), adPercentItemBonus(0.0), omnivamp(0.0), damageAmp(0.0), durability(0.0), critChance(0.0), totalDamageDealt(0.0)

{

    if (role == Role::Tank)
    {
        manaRegenAtk = 5.0;
        manaRegenSec = 0.0;
    }
    else if (role == Role::Caster)
    {
        manaRegenAtk = 5.0;
        manaRegenSec = 2.0;
    }
    else if (role == Role::Marksman)
    {
        manaRegenAtk = 10.0;
        manaRegenSec = 0.0;
    }
    else if (role == Role::Fighter)
    {
        manaRegenAtk = 10.0;
        manaRegenSec = 0.0;
        this->as = this->as * 1.15;
    }
    else if (role == Role::Assassin)
    {
        manaRegenAtk = 10.0;
        manaRegenSec = 0.0;
    }
    else
    {
        manaRegenAtk = 10.0;
        manaRegenSec = 0.0;
    }

    if (cost == 5 || name == "Ornn" || name == "Illaoi")
    {
        baseCastTime = 1.0;
    }
    else if (role == Role::Marksman || role == Role::Assassin || name == "Gwen")
    {
        baseCastTime = 0.3;
    }
    else
    {
        baseCastTime = 0.6;
    }
}

void Champion::promote()
{
    if (starLevel < 3)
    {
        starLevel++;
        maxHp *= 1.8;
        hp = maxHp;
        ad *= 1.5;
    }
}

double Champion::takeDamage(double rawDamage, bool isTrueDamage, bool isTargeted)
{
    double finalDamage = rawDamage;

    if (role == Role::Assassin && !isTargeted)
    {
        finalDamage *= 0.85;
    }

    if (!isTrueDamage)
    {
        double mitigation = 100.0 / (100.0 + armor);
        finalDamage *= mitigation;
    }

    if (shield > 0)
    {
        if (shield >= finalDamage)
        {
            shield -= finalDamage;
            finalDamage = 0;
        }
        else
        {
            finalDamage -= shield;
            shield = 0;
        }
    }

    hp -= finalDamage;
    if (hp < 0)
        hp = 0;

    if (role == Role::Tank && manaLockTimer <= 0)
    {
        mana += std::min(finalDamage * 0.01, 42.5);
    }
    return finalDamage;
}

void Champion::heal(double amount)
{
    hp += amount;
    if (hp > maxHp)
        hp = maxHp;
}

double Champion::performBasicAttack()
{
    if (manaLockTimer <= 0)
    {
        mana += manaRegenAtk;
    }
    return ad;
}

void Champion::applyPassiveMana()
{
    if (manaLockTimer <= 0)
    {
        mana += manaRegenSec;
    }
}

void Champion::updateTimers(double tick)
{
    if (stunTimer > 0)
        stunTimer -= tick;
    if (manaLockTimer > 0)
        manaLockTimer -= tick;
    if (shield > 0)
        shield -= (shield * 0.1 * tick);
    if (buffTimer > 0)
        buffTimer -= tick;
}

bool Champion::canAct() const
{
    return stunTimer <= 0;
}

bool Champion::canCast() const
{
    return mana >= manaMax && manaLockTimer <= 0;
}

void Champion::castSpell()
{
    mana = 0;
    manaLockTimer = baseCastTime;
}

void Champion::applyStun(double duration)
{
    stunTimer = std::max(stunTimer, duration);
}

int Champion::getX() const { return x; }
int Champion::getY() const { return y; }
int Champion::getOwnerId() const { return ownerId; }

void Champion::setPosition(int newX, int newY)
{
    x = newX;
    y = newY;
}

void Champion::setOwnerId(int id)
{
    ownerId = id;
}

std::string Champion::getName() const { return name; }
Role Champion::getRole() const { return role; }
int Champion::getCost() const { return cost; }
int Champion::getRange() const { return range; }
int Champion::getStarLevel() const { return starLevel; }

double Champion::getAd() const { return ad; }
double Champion::getArmor() const { return armor; }
double Champion::getMaxHp() const { return maxHp; }
double Champion::getHp() const { return hp; }
double Champion::getMana() const { return mana; }
double Champion::getShield() const { return shield; }

double Champion::getRealAttackSpeed() const
{
    return as;
}

std::vector<std::string> Champion::getTraits() const
{
    return traits;
}

void Champion::addBonusStats(double bHp, double bAd, double bAp, double bArmor, double bMr, double bAs)
{
    maxHp += bHp;
    hp += bHp;
    ad += bAd;
    ap += bAp;
    armor += bArmor;
    mr += bMr;
    as += bAs;
}

void Champion::addShield(double amount)
{
    shield += amount;
}

void Champion::triggerOvertime()
{
    as *= 3.0;
    ad *= 2.0;
    ap *= 2.0;
    manaLockTimer = 0;
}

bool Champion::equipItem(const Item &item)
{
    if (inventory.size() >= 3)
    {
        return false;
    }

    inventory.push_back(item);

    double baseAd = ad / (1.0 + adPercentItemBonus); 
    
    adPercentItemBonus += item.adPercent;
    
    ad = baseAd * (1.0 + adPercentItemBonus);
    
    ap += item.ap;
    armor += item.armor;
    mr += item.mr;

    double hpBonus = item.hp;
    double hpPercentBonus = maxHp * item.hpPercent;
    maxHp += (hpBonus + hpPercentBonus);
    hp += (hpBonus + hpPercentBonus);

    as *= (1.0 + item.asPercent);
    critChance += item.critChance;
    omnivamp += item.omnivamp;
    damageAmp += item.damageAmp;
    durability += item.durability;

    if (item.manaRegenPercent > 0)
    {
        manaRegenAtk *= (1.0 + item.manaRegenPercent);
    }

    return true;
}

const std::vector<Item>& Champion::getInventory() const
{
    return inventory;
}

void Champion::addMana(double amount)
{
    if (manaLockTimer <= 0)
    {
        mana += amount;
        if (mana > manaMax)
            mana = manaMax;
    }
}

double Champion::getTotalDamageDealt() const {
    return totalDamageDealt;
}

void Champion::addDamageDealt(double amount) {
    totalDamageDealt += amount;
}

void Champion::addDurability(double amount) { durability += amount; }
void Champion::addOmnivamp(double amount) { omnivamp += amount; }
void Champion::addDamageAmp(double amount) { damageAmp += amount; }
void Champion::addCritChance(double amount) { critChance += amount; }
void Champion::multiplyArmorAndMr(double factor) {
    armor *= factor;
    mr *= factor;
}