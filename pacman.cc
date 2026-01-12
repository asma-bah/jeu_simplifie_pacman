#include "pacman.hh"

position::position(coord x, coord y)
    :_x(x), _y(y) {}

coord position::x() const{
    return _x;
}

coord position::y() const {
    return _y;
}

std::string position::tostring() const{
    return "(" + std::to_string(_x)  +"," + std::to_string(_y) + ")";
}

std::ostream & operator<<(std::ostream & os, position const & p) {
    return os << p.tostring();
}

taille::taille(coord hauteur, coord largeur)
    :_largeur(largeur), _hauteur(hauteur){}

coord taille::h() const{
    return _hauteur;
}

coord taille::w() const{
    return _largeur;
}

std::string taille::tostring() const{
    return std::to_string(_largeur) + "," + std::to_string(_hauteur);
}

std::ostream & operator<<(std::ostream & os, taille const & t){
    return os << t.tostring();
}

element::element(position const & p)
    :_pos(p) {}

void element::sortie_flux(std::ostream & os) const{
    os << typeobjet() << _pos.tostring() << "-" << tai().tostring();
}

std::ostream & operator<<(std::ostream & os, element const & e) {
    e.sortie_flux(os);

    return os;
}

position element::pos() const{
    return _pos;
}

enmouvement::enmouvement(position const & p, direction dir)
    :element(p), _dir(dir){}

void enmouvement::sortie_flux(std::ostream & os) const{
    element::sortie_flux(os);
    os << " ";
    switch(_dir){
    case direction::stop:
        os << "stop";
        break;
    case direction::bas:
        os << "bas";
        break;
    case direction::droite:
        os << "droite";
        break;
    case direction::gauche:
        os << "gauche";
        break;
    case direction::haut:
        os << "haut";
        break;
    default:
        break;
    }
}

pacman::pacman(position const & p,direction dir)
    :enmouvement(p, dir), _niveau(0){}

bool pacman::invincible() const{
    return _niveau > 0;
}
void pacman::decrementerinvincible() {
    if(invincible()){
        _niveau--;
    }
}
void pacman::devenirinvincible() {
    _niveau += 200;
}

taille pacman::tai() const{
    return taille(13,13);
}

void pacman::sortie_flux(std::ostream & os) const{
    enmouvement::sortie_flux(os);
    os << " niveau d'invincibilité : " << _niveau;
}

fantome::fantome(position const & p, direction dir)
    :enmouvement(p, dir){}

taille fantome::tai() const{
    return taille(20,20);
}

pacgomme::pacgomme(position const & p)
    :element(p){}

taille pacgomme::tai() const{
    return taille(3,3);
}

mur::mur(position const & p, taille const & t)
    :element(p),
    _tai(t.h() >= 10 && t.w() >= 10 ? t : taille(10, 10)){

}

taille mur::tai() const{
    return _tai;
}

