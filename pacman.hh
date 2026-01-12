#pragma once
#include<iostream>

enum class direction{
    stop, droite, gauche, haut, bas
};

using coord = unsigned int;

class position {
public:
    position(coord x, coord y);
    coord x() const;
    coord y() const;
    std::string tostring() const;
    bool operator==(position const & p) const {
        return (_x == p._x) && (_y == p._y);
    };
    bool operator!=(position const & p) const {
        return !operator==(p);
    };

private:
    coord _x;
    coord _y;
};

std::ostream & operator<<(std::ostream & os, position const & p);


//la classe taille
class taille {
public:
    taille(coord largeur, coord hauteur);
    coord w() const;
    coord h() const;
    std::string tostring() const;

private:
    coord _largeur;
    coord _hauteur;
};
std::ostream & operator<<(std::ostream & os, taille const & t);


//la classe element



class element {
public:
    element(position const & pos);
    virtual ~element() = default;

    position pos() const;
   virtual  taille tai() const = 0;
    void setpos(position const & p){
        _pos = p;
    };

    virtual char typeobjet() const = 0;
private:
    position _pos;

};



//les elments pouvant se deplacer
class enmouvement : public element{
public:
    enmouvement(position const & p, direction dir);
    direction deplacement() const{
        return _dir;
    };
    void setdir(direction dir){
        _dir = dir;
    }
private:
    direction _dir;
};



//les pacman
class pacman : public enmouvement{
public:
    pacman(position const & p, direction dir);
    bool invincible() const;
    void decrementerinvincible();
    void devenirinvincible();
    taille tai() const override;
    char typeobjet() const override{
        return 'P';
    };
private:
    int _niveau;
};




















