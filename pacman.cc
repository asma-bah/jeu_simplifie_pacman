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

//appliquer le deplacement sur une position selon la direction entrer en parametre
void position::appliquerdeplacement(direction dir){
    switch (dir) {
    case direction::droite:
        ++_x;
        break;
    case direction::gauche:
        --_x;
        break;
    case direction::bas:
        ++_y;
        break;
    case direction::haut:
        --_y;
        break;
    case direction::stop:
        break;
    default:
        break;
    }
}

std::ostream & operator<<(std::ostream & os, position const & p) {
    return os << p.tostring();
}

taille::taille(coord largeur, coord hauteur)
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

pacman::pacman(position const & p)
    :enmouvement(p, direction::stop), _niveau(0){}

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

std::unique_ptr<element> pacman::clone() const{
    return std::make_unique<pacman>(*this);
}


fantome::fantome(position const & p)
    :enmouvement(p, direction::stop){}

taille fantome::tai() const{
    return taille(20,20);
}
std::unique_ptr<element> fantome::clone() const{
    return std::make_unique<fantome>(*this);
}


pacgomme::pacgomme(position const & p)
    :element(p){}

taille pacgomme::tai() const{
    return taille(3,3);
}
std::unique_ptr<element> pacgomme::clone() const{
    return std::make_unique<pacgomme>(*this);
}

mur::mur(position const & p, taille const & t)
    :element(p),
    _tai(t){

}

taille mur::tai() const{
    return _tai;
}

std::unique_ptr<element> mur::clone() const{
    return std::make_unique<mur>(*this);
}

std::unique_ptr<mur> mur::fabrique(position const & p, taille const & t){
    if(t.w() >= 10 && t.h() >= 10){
        return std::unique_ptr<mur>(new mur(p, t));
    }
    else{
        return std::unique_ptr<mur>();
    }
}

bool element::contient(element const & o) const {
    return (o.pos().x()>=pos().x()) && (o.pos().y()>=pos().y()) && ((o.pos().x()+o.tai().w()) <= pos().x()+tai().w()) && ((o.pos().y()+o.tai().h()) <= pos().y()+tai().h());
}

bool intersectionintervalle(unsigned int a1, unsigned int a2, unsigned int b1, unsigned int b2) {
    return b1 < a2 && a1 < b2;
}
bool element::intersection(const element &o) const {
    return intersectionintervalle(pos().x(), pos().x()+tai().w(), o.pos().x(), o.pos().x()+o.tai().w()) && intersectionintervalle(pos().y(), pos().y()+tai().h(), o.pos().y(), o.pos().y()+o.tai().h());
}


jeu::jeu(jeu const & j)
    :_etat(j._etat){
    for(auto const & i : j._elements){
        _elements.push_back(i->clone());
    }
}

jeu & jeu::operator=(jeu const & j) {
    if(this != &j){
        _etat = j._etat;
        _elements.clear();
        for(auto const & e : j._elements){
            _elements.push_back(e->clone());
        }
    }
    return *this;
}

void jeu::afficher(std::ostream & os) const{
    os << "Etat du jeu : ";
    switch(_etat){
    case type::encours:
        os << " encours";
        break;
    case type::defaite:
        os << "defaite";
        break;
    case type::victoire:
        os << "victoire";
        break;
    default :
        break;
    }

    os << " ";
    for(auto const & e : _elements){
        e->sortie_flux(os);
        os << "\n";
    }
}

char const* exceptionjeu::what() const noexcept{
    return _nature.c_str();
}

//ajout d'un element dans un
void jeu::ajouter(std::unique_ptr<element> e) {
   auto f = std::find_if(
               _elements.begin(),
               _elements.end(),
               [&e](auto const & i) {
       return i->intersection(*e);
   });
   if(f == _elements.end()){
        _elements.push_back(std::move(e));
   }
   else{
        throw exceptionjeu("Collision : impossible d'ajouter ici");
   }
}

//ajout un nombre e de fantomes au jeu à des positions aleatoires
void jeu::ajouterfantomes(int nbf) {
    while(nbf != 0){
        try {
           ajouter(std::make_unique<fantome>(position(static_cast<unsigned int>(rand()%320), static_cast<unsigned int>(rand()%200))));
            --nbf;
        } catch (exceptionjeu const &) {
        }
    }
}

//ajout un nombre e de fantomes au jeu à des positions aleatoires
void jeu::ajouterpacgommes(int nbg) {
    while(nbg != 0){
        try {
           ajouter(std::make_unique<pacgomme>(position(static_cast<unsigned int>(rand()%320), static_cast<unsigned int>(rand()%200))));
           --nbg;
        } catch (exceptionjeu const &) {
        }
    }
}

//recherche du pacaman du jeu
const pacman & jeu::accespacman() const{
    auto f = std::find_if(
       _elements.begin(),
        _elements.end(),
            [](auto const & e){
        return (dynamic_cast<pacman const *> (e.get()) != nullptr);
    });
    if(f != _elements.end()){
       return *dynamic_cast<pacman const *>(f->get());
    }
    else{
        throw exceptionjeu("Pacgman introuvable");
    }

}

//changer la direction du pacman si trouver dans le plateau de jeu
void jeu::directionjoueur(direction dir) {
    auto f = std::find_if(
       _elements.begin(),
        _elements.end(),
            [](auto const & e){
        return (dynamic_cast<pacman const *> (e.get()) != nullptr);
    });

    if(f == _elements.end()){
        throw exceptionjeu("Pacman introuvable");
    }
    else{
        dynamic_cast<pacman *>(f->get())->setdir(dir);
    }
}

//deplacement des elements actifs dans la direction choisie s'il ya collision avec un mur
void jeu::appliquerdeplacementcollisionmur(enmouvement & em){
    if(em.deplacement() != direction::stop){
        auto ancienpos(em.pos()), newpos(ancienpos);
        newpos.appliquerdeplacement(em.deplacement());
        em.setpos(newpos);
        auto f = std::find_if(
                    _elements.begin(),
                    _elements.end(),
                    [&em](std::unique_ptr<element> const & i) {
            return (dynamic_cast<mur*>(i.get()) != nullptr) && i->intersection(em);

        });
        if(f != _elements.end()){
            em.setpos(ancienpos); //pas de collision
        }
    }
}

//verifie si le pacman n'entre pas en collision avec un fantome
void jeu::appliquerdeplacementcontact(pacman & pm){
    //s'il est invincible,
    if(!pm.invincible()){
        auto f = std::find_if(
                    _elements.begin(),
                    _elements.end(),
                    [&pm](auto const & i){
            return dynamic_cast<fantome *>(i.get()) != nullptr && i->intersection(pm);
        });
        // on a collision avec un fantome,game over
        if(f != _elements.end()){
            _etat = type::defaite;
        }
    }
    //pacman invincible
    else{
        //on supprime le fantome s'il y a collision
        _elements.erase(
                    std::remove_if(
                        _elements.begin(),
                        _elements.end(),
                        [&pm](auto const & i){
            return (dynamic_cast<fantome *>(i.get()) != nullptr) && i->intersection(pm);

        }), _elements.end());
    }

}

//pacman mange les pacgommes si ils sont dans le meme rectangle
void jeu::appliquerdeplacementmanger(pacman & pm){
    auto it(_elements.begin());
    while(it != _elements.end()){
        //s'ils sont en collisions
        if((dynamic_cast<pacgomme const *>(it->get()) != nullptr) && pm.contient(**it)){
            it = _elements.erase(it); //le pacgomme disparaait
            pm.devenirinvincible(); //le pacman gagne en invincibilité
        }
        else{
            ++it;
        }
    }
}

//fonction qui permet de choisir au hasard une direction
direction randomdirection(){
    switch(rand() % 4){
    case 0 : return direction::haut;
    case 1: return direction::bas;
    case 2: return direction::droite;
    case 3: return direction::gauche;
    }
    return direction::stop;
}

//changer la direction d'un fantome
void fantome::changerdirection() {
    setdir(randomdirection());
}
//changer aleatoirement la direction des fantomes
void jeu::changerdirectionfantomes(){
    for(auto & e : _elements){
        auto f(dynamic_cast<fantome *>(e.get()));

        if(f) {
            f->changerdirection();
        }
    }
}

//tour d'un tour de jeu (victoire quand y a plus de pacgommes)
void jeu::tourdejeu() {
    //appliquer les deplacements pour les elements actifs
    for(auto & e : _elements){
        auto em(dynamic_cast<enmouvement *>(e.get()));
        if(em){
            appliquerdeplacementcollisionmur(*em);
        }
    }
    for(auto & e : _elements){
        auto pm(dynamic_cast<pacman *>(e.get()));
        if(pm){
            appliquerdeplacementcontact(*pm);
            appliquerdeplacementmanger(*pm);
            pm->decrementerinvincible();
        }
    }

    //verifier s'il reste des pacmans
    if(std::none_of(_elements.begin(), _elements.end(), [](auto const & e){return dynamic_cast<pacgomme const *>(e.get()) != nullptr;})) {
        _etat = type::victoire;
    }
}
