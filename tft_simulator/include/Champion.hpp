#ifndef CHAMPION_HPP
#define CHAMPION_HPP

#include "Item.hpp"
#include <string>
#include <vector>

enum class Role
{
    Tank,
    Fighter,
    Assassin,
    Marksman,
    Caster,
    Specialist
};

class Champion
{
private:
    std::string name;
    Role role;
    int cost;
    int range;
    int starLevel;

    double hp;
    double maxHp;
    double mana;
    double manaMax;
    double manaRegenSec;
    double manaRegenAtk;
    double ad;
    double ap;
    double armor;
    double mr;
    double as;

    double shield;
    double stunTimer;
    double manaLockTimer;
    double buffTimer;
    double baseCastTime;

    std::vector<std::string> traits;

    int x;
    int y;
    int ownerId;

    std::vector<Item> inventory;

    double omnivamp;
    double damageAmp;
    double durability;
    double critChance;
    double adPercentItemBonus;

    double totalDamageDealt;


public:
    Champion(std::string name, int cost, std::vector<std::string> traits, double maxHp, double manaMax, double manaInitial, double ad, double ap, double armor, double mr, double as, int range, Role role);

    void promote();

    double performBasicAttack();
    void applyPassiveMana();

    void updateTimers(double tick);
    bool canAct() const;
    bool canCast() const;
    void castSpell();

    double takeDamage(double rawDamage, bool isTrueDamage = false, bool isTargeted = false);
    void applyStun(double duration);
    void heal(double amount);

    std::string getName() const;
    Role getRole() const;
    int getCost() const;
    int getRange() const;
    int getStarLevel() const;
    double getHp() const;
    double getMaxHp() const;
    double getMana() const;
    double getRealAttackSpeed() const;
    double getShield() const;
    const std::vector<Item>& getInventory() const;
    double getAd() const;
    double getArmor() const;
    double getTotalDamageDealt() const;

    int getX() const;
    int getY() const;
    int getOwnerId() const;

    void setPosition(int newX, int newY);
    void setOwnerId(int id);


    std::vector<std::string> getTraits() const;

    void addBonusStats(double bHp, double bAd, double bAp, double bArmor, double bMr, double bAs);
    void addShield(double amount);

    void triggerOvertime();
    bool equipItem(const Item& item);

    void addMana(double amount);

    void addDamageDealt(double amount);

    void addDurability(double amount);
    void addOmnivamp(double amount);
    void addDamageAmp(double amount);
    void addCritChance(double amount);
    void multiplyArmorAndMr(double factor);
    
};

#endif