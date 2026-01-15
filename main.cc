#include "pacman.hh"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <array>

int main() {
    sf::RenderWindow window(sf::VideoMode(640, 400), "PAC-MAN");
    window.setKeyRepeatEnabled(false);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    sf::Texture texturesprites;
    if (!texturesprites.loadFromFile("sprites.png")) {
        std::cerr << "Fichier sprites.png introuvable.\n";
        return 1;
    }
    std::array<sf::Sprite, 4> spritespacman;
    for (unsigned int i=0; i<4; ++i) {
        spritespacman[i].setTexture(texturesprites);
        spritespacman[i].setTextureRect(sf::IntRect(static_cast<int>(i)*13,0,13,13));
        spritespacman[i].setScale(2,2);
    }
    std::array<sf::Sprite, 4> spritesfantome;
    for (unsigned int i=0; i<2; ++i) {
        spritesfantome[i].setTexture(texturesprites);
        spritesfantome[i].setTextureRect(sf::IntRect(4*13+static_cast<int>(i)*20,0,20,20));
        spritesfantome[i].setScale(2,2);
    }

    jeu j;
    try{
        j.ajouter(mur::fabrique(position(0,0), taille(320,10)));
        j.ajouter(mur::fabrique(position(0,190), taille(320,10)));
        j.ajouter(mur::fabrique(position(0,10), taille(10,180)));
        j.ajouter(mur::fabrique(position(310,10), taille(10,180)));

        j.ajouter(std::make_unique<pacman>(position(150,30)));

        j.ajouterfantomes(4);
        j.ajouterpacgommes(10);
    }
    catch(exceptionjeu const & e){
        std::cerr << e.what() << std::endl;
    }

    j.afficher(std::cout);

    unsigned int decompte(0);
    while (window.isOpen() && (j.etatjeu() != jeu::type::victoire) && (j.etatjeu() != jeu::type::defaite)) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left)
                    j.directionjoueur(direction::gauche);
                else if (event.key.code == sf::Keyboard::Right)
                    j.directionjoueur(direction::droite);
                if (event.key.code == sf::Keyboard::Up)
                    j.directionjoueur(direction::haut);
                else if (event.key.code == sf::Keyboard::Down)
                    j.directionjoueur(direction::bas);
                else if (event.key.code == sf::Keyboard::Space)
                    j.directionjoueur(direction::stop);
            }
        }
        if (decompte == 0) {
            j.changerdirectionfantomes();
           decompte = 50;
        }
        else
            decompte--;

        j.tourdejeu();
        window.clear(sf::Color::Black);
        try {
            auto pm(j.accespacman());

            for (auto const & i : j.elements()) {
                if (dynamic_cast<fantome const *>(i.get())) {
                    spritesfantome[pm.invincible() ? 1 : 0].setPosition(static_cast<float>(i->pos().x()*2), static_cast<float>(i->pos().y()*2));
                    window.draw(spritesfantome[pm.invincible() ? 1 : 0]);
                }
                else if (dynamic_cast<mur const *>(i.get()) || dynamic_cast<pacgomme const *>(i.get())) {
                    sf::RectangleShape smur(sf::Vector2f(static_cast<float>(i->tai().w()*2),static_cast<float>(i->tai().h()*2)));
                    if (dynamic_cast<mur const *>(i.get()))
                        smur.setFillColor(sf::Color::Blue);
                    else
                        smur.setFillColor(sf::Color::Green);
                    smur.setPosition(static_cast<float>(i->pos().x()*2), static_cast<float>(i->pos().y())*2);
                    window.draw(smur);
                }
            }
            unsigned int ispritepacman;
            switch (pm.deplacement()) {
                case direction::stop: ispritepacman=0; break;
                case direction::droite: ispritepacman=0; break;
                case direction::gauche: ispritepacman=1; break;
                case direction::haut: ispritepacman=2; break;
                case direction::bas: ispritepacman=3; break;
            }

            spritespacman[ispritepacman].setPosition(static_cast<float>(pm.pos().x()*2), static_cast<float>(pm.pos().y()*2));
            window.draw(spritespacman[ispritepacman]);
        }
        catch(exceptionjeu const &){}



        window.display();

    }
    j.afficher(std::cout);
    return 0;
}
