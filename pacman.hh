#pragma once
#include<iostream>
#include<list>
#include<memory>
#include<exception>
#include<algorithm>
#include<random>

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

    void appliquerdeplacement(direction dir);

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
    virtual void sortie_flux(std::ostream & os) const;
    virtual char typeobjet() const = 0;
    bool contient(element const & e) const;
    bool intersection(element const & e) const;
    virtual direction deplacement() const{
        return direction::stop;
    }
    virtual std::unique_ptr<element> clone() const = 0;

private:
    position _pos;

};

std::ostream & operator<<(std::ostream & os, element const & e);

//les elments pouvant se deplacer
class enmouvement : public element{
public:
    enmouvement(position const & p, direction);
    direction deplacement() const override{
        return _dir;
    };
    void setdir(direction dir){
        _dir = dir;
    }
    void sortie_flux(std::ostream & os) const override;
private:
    direction _dir;
};



//les pacman
class pacman : public enmouvement{
public:
    pacman(position const & p);
    bool invincible() const;
    void decrementerinvincible();
    void devenirinvincible();
    taille tai() const override;
    char typeobjet() const override{
        return 'P';
    };
    void sortie_flux(std::ostream & os) const override;
    virtual std::unique_ptr<element> clone() const override;
private:
    int _niveau;
};


//les fantomes
class fantome : public enmouvement{
public:
    fantome(position const & p);
    taille tai() const override;
    char typeobjet() const override{
        return 'F';
    };
    virtual std::unique_ptr<element> clone() const override;
    void changerdirection();
};


//les pacgommes
class pacgomme : public element {
public:
    pacgomme(position const & p);
    taille tai() const override;
    char typeobjet() const override{
        return 'G';
    }
    virtual std::unique_ptr<element> clone() const override;

};


//les murs
class mur : public element{
public:
    mur(position const & p, taille const & t);
    taille tai() const override;
    char typeobjet() const override{
        return 'M';
    }
    virtual std::unique_ptr<element> clone() const override;
    static std::unique_ptr<mur> fabrique(position const & p, taille const & t);
private:
   taille _tai;
};



//la classe execption jeu
class exceptionjeu : public std::exception{
public:
    exceptionjeu(std::string const & nature)
        :std::exception(), _nature(nature){}

    char const* what() const noexcept override;
private:
    std::string _nature;
};


//la classe jeu
class jeu{
public:
    enum class type{
        encours, defaite, victoire
    };
public:
    jeu() : _etat(type::encours){};
    jeu(jeu const & j);
    jeu & operator=(jeu const & j);
    void afficher(std::ostream & os) const;
    void ajouter(std::unique_ptr<element> e);
    void ajouterfantomes(int nbf);
    void ajouterpacgommes(int nbg);
    const pacman & accespacman() const;
    void directionjoueur(direction dir);
    void changerdirectionfantomes();
    void tourdejeu();
    type etatjeu(){
        return _etat;
    }
    std::list<std::unique_ptr<element>> const & elements() const{
        return _elements;
    }
private:
    void appliquerdeplacementcollisionmur(enmouvement & em);
    void appliquerdeplacementcontact(pacman & pm);
    void appliquerdeplacementmanger(pacman & pm);
private:
    std::list<std::unique_ptr<element>> _elements;
    type _etat;
};



