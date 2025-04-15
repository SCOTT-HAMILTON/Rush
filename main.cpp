#include <iostream>
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "CollisionManager.h"
#include <string>
#include <random>
#include "BoomAnim.h"
#include "Menu.h"
#include "Config.h"
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <memory>
#include <fstream>
#include <cxxopts.hpp>

std::random_device rd;
std::mt19937 rng(rd());
std::uniform_int_distribution<int> boat_pos_dist(1000,1100);
std::uniform_real_distribution<> airport_pos_dist(3000, 4000);// 3000
std::uniform_int_distribution<int> missile_boat_cible(0, 1000);

void createBoat(std::vector<std::shared_ptr<Entity>> &boats, CollisionManager &collisioner, bool random = true, const sf::Vector2f &pos = {0, 0});
void createMissileBoat(const sf::Vector2f &pos, const sf::Vector2f &perso_pos, std::vector<std::shared_ptr<Entity>> &missilesBoats, CollisionManager &collisioner);
float distance(const sf::Vector2f &p1, const sf::Vector2f &p2);
void updateFont(sf::Font &font);
void createBombPerso(const sf::Vector2f &pos, std::vector<std::shared_ptr<Entity>> &bombsPerso, CollisionManager &collisioner, bool send = true);
void createMissilePerso(std::vector<std::shared_ptr<Entity>> &missilesPerso, CollisionManager &collisioner, const sf::Vector2f &pos, bool send = true);

void receive();
int synchonize(float &airport_distance);
bool continue_receiving = true;

float ground_speed = 0.5f;
float missile_perso_speed = 10.0f;
float missile_boat_speed = missile_perso_speed*0.1f;

unsigned short port_secondary = 60000;
unsigned short port_primary = 50000;

unsigned short port_receiver = port_primary;
unsigned short port_sender = port_primary;
sf::IpAddress remote_addr = sf::IpAddress("192.168.1.99");

const int repetition_send_important = 10;

sf::Mutex mutex;

sf::UdpSocket receiver;
sf::UdpSocket sender;

std::vector<std::shared_ptr<Entity>> bombPerso;
std::vector<std::shared_ptr<Entity>> missilesPerso;
std::vector<std::shared_ptr<Entity>> missileBoats;
std::vector<std::shared_ptr<Entity>> boats;
CollisionManager collisioner;

std::shared_ptr<Entity> perso;
std::shared_ptr<Entity> partner;

bool partner_won = false;
bool partner_lose = false;
bool first = false;
const int max_boats = 30;
const int max_missileboats = 100;
bool continue_wait = false;
GameMode mode;

int main(int argc, char *argv[])
{

    // Default configuration
    std::string address = "0.0.0.0";
    try {
        cxxopts::Options options(argv[0], "Rush\nA solo or multiplayer LAN naval game\nFor the multiplayer mode, please open ports 50 000 or 60 000on both sides");
        options.add_options()
            ("a,address", "IP address of the remote partner to play with in multiplayer", 
             cxxopts::value<std::string>(address)->default_value("0.0.0.0"))
            ("h,help", "Print this help message");
        auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }
        // Validate address
        remote_addr = sf::IpAddress(address);
        if (remote_addr == sf::IpAddress::None) {
            throw std::runtime_error("Invalid IP address: " + address);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n\n";
        std::cerr << "Use --help for usage information" << std::endl;
        return 1;
    }
    std::cout << "[DEBUG] remote address : `" << remote_addr << '`' << std::endl;

    sf::Clock myclock;
    sf::Clock boat_creation_cooldown;
    sf::Clock gameOver_cooler;
    float dt = 0;
    bool won = false;
    bool lose = false;
    const int delay_boat_missile_creat = 1000;
    bool land_launched = false;
    int delay_restart = 3000;
    bool got_focus = true;
    sf::Packet pack_s;
    bool go_to_menu = false;

    sf::RenderWindow fenetre(sf::VideoMode(800, 600), "Rush ");
    sf::Event event;
    fenetre.setFramerateLimit(144);

    Menu menu(fenetre);

    std::shared_ptr<sf::ConvexShape> perso_base = std::make_shared<sf::ConvexShape>(10);
    perso_base->setPointCount(10);
    float x_scale = 1, y_scale = 1;
    perso_base->setPoint(0, sf::Vector2f(0*x_scale, 15*y_scale));
    perso_base->setPoint(1, sf::Vector2f(18*x_scale, 23*y_scale));
    perso_base->setPoint(2, sf::Vector2f(3*x_scale, 36*y_scale));
    perso_base->setPoint(3, sf::Vector2f(65*x_scale, 36*y_scale));
    perso_base->setPoint(4, sf::Vector2f(77*x_scale, 22*y_scale));
    perso_base->setPoint(5, sf::Vector2f(68*x_scale, 13*y_scale));
    perso_base->setPoint(6, sf::Vector2f(54*x_scale, 4*y_scale));
    perso_base->setPoint(7, sf::Vector2f(37*x_scale, 0*y_scale));
    perso_base->setPoint(8, sf::Vector2f(23*x_scale, 1*y_scale));
    perso_base->setPoint(9, sf::Vector2f(11*x_scale, 7*y_scale));

    std::shared_ptr<sf::ConvexShape> partner_base = std::make_shared<sf::ConvexShape>(10);
    partner_base->setPointCount(10);
    x_scale = 1, y_scale = 1;
    partner_base->setPoint(0, sf::Vector2f(0*x_scale, 15*y_scale));
    partner_base->setPoint(1, sf::Vector2f(18*x_scale, 23*y_scale));
    partner_base->setPoint(2, sf::Vector2f(3*x_scale, 36*y_scale));
    partner_base->setPoint(3, sf::Vector2f(65*x_scale, 36*y_scale));
    partner_base->setPoint(4, sf::Vector2f(77*x_scale, 22*y_scale));
    partner_base->setPoint(5, sf::Vector2f(68*x_scale, 13*y_scale));
    partner_base->setPoint(6, sf::Vector2f(54*x_scale, 4*y_scale));
    partner_base->setPoint(7, sf::Vector2f(37*x_scale, 0*y_scale));
    partner_base->setPoint(8, sf::Vector2f(23*x_scale, 1*y_scale));
    partner_base->setPoint(9, sf::Vector2f(11*x_scale, 7*y_scale));

    perso_base->setFillColor(sf::Color(0, 0, 255));
    perso = std::make_shared<Entity>(perso_base, sf::Vector2f(300, 300), sf::Vector2f(5, 5), 0.0f, 1000.0f);
    perso->setType("perso");

    partner_base->setFillColor(sf::Color(255, 0, 0));
    partner = std::make_shared<Entity>(partner_base, sf::Vector2f(300, 300), sf::Vector2f(5, 5), 0.0f, 1000.0f);
    partner->setType("perso");
    partner->dontApplyGravity();

    float airport_distance;

    std::shared_ptr<sf::RectangleShape> airport_base = std::make_shared<sf::RectangleShape>(sf::Vector2f(250, 30));
    airport_base->setOutlineColor(sf::Color::White);
    airport_base->setOutlineThickness(5);
    airport_base->setFillColor(sf::Color::Transparent);
    std::shared_ptr<Entity> airport = std::make_shared<Entity>(airport_base, sf::Vector2f(airport_distance, 300), sf::Vector2f(ground_speed, 0));
    airport->setType("airport");
    std::vector<std::shared_ptr<BoomAnim>> boomanims;

    sf::RectangleShape ground(sf::Vector2f(800, 50));
    ground.setPosition({0, 550});
    ground.setFillColor(sf::Color(0, 255, 0));

    sf::Thread threadreceiver(&receive);

    /*FONTS - TEXT*/
    sf::Font font_flyinfo;
    if (!font_flyinfo.loadFromFile(FILE_BEBASNEUE)){
        std::cerr << "[DEBUG] Cant load font BebasNeue-Regular.otf, exitting..." << std::endl;
        return 0;
    }

    sf::Text TextAirportDist;
    TextAirportDist.setFont(font_flyinfo);
    TextAirportDist.setCharacterSize(20);
    TextAirportDist.setString("AIRPORT DIST : "+std::to_string(static_cast<int>(airport_distance)));
    TextAirportDist.setFillColor(sf::Color(0, 255, 255));
    TextAirportDist.setPosition(50, 50);

    sf::Font font_youlose;
    updateFont(font_youlose);

    sf::Text YouLoseText;
    YouLoseText.setFont(font_youlose);
    YouLoseText.setCharacterSize(30);
    YouLoseText.setString("You lose !");
    YouLoseText.setFillColor(sf::Color(255, 255, 0));
    YouLoseText.setPosition(400-YouLoseText.getGlobalBounds().width/2, 300-YouLoseText.getGlobalBounds().height/2);

    sf::Text YouWonText;
    YouWonText.setFont(font_youlose);
    YouWonText.setCharacterSize(30);
    YouWonText.setString("You Won !");
    YouWonText.setFillColor(sf::Color(255, 20, 255));
    YouWonText.setPosition(400-YouWonText.getGlobalBounds().width/2, 300-YouWonText.getGlobalBounds().height/2);

    sf::Text airport_text;
    airport_text.setFont(font_flyinfo);
    airport_text.setCharacterSize(30);
    airport_text.setString("airport");
    airport_text.setFillColor(sf::Color(255, 255, 255));
    sf::Vector2f pos_airport_text;

    sf::Text Nb_Boats_text;
    Nb_Boats_text.setFont(font_flyinfo);
    Nb_Boats_text.setCharacterSize(20);
    Nb_Boats_text.setFillColor(sf::Color(255, 120, 0));
    Nb_Boats_text.setString("boats : "+std::to_string(boats.size()));
    Nb_Boats_text.setPosition(800-50-Nb_Boats_text.getGlobalBounds().width, 50);

    sf::Text pos_text;
    pos_text.setFont(font_flyinfo);
    pos_text.setCharacterSize(30);
    pos_text.setFillColor(sf::Color(255, 255, 255));
    pos_text.setString("pos : "+std::to_string((int)perso->getPos().x)+", "+std::to_string((int)perso->getPos().y) );
    pos_text.setPosition(350, 15);
    /*FONTS - TEXT*/

    /*  AUDIO  */

    sf::SoundBuffer shoot_sndbuffer;
    shoot_sndbuffer.loadFromFile(FILE_SHOOT);
    sf::Sound shoot_snd;
    shoot_snd.setBuffer(shoot_sndbuffer);
    shoot_snd.setVolume(50);

    sf::SoundBuffer land_sndbuffer;
    land_sndbuffer.loadFromFile(FILE_LAND);
    sf::Sound land_snd;
    land_snd.setBuffer(land_sndbuffer);

    sf::SoundBuffer fly_sndbuffer;
    fly_sndbuffer.loadFromFile(FILE_FLY);
    sf::Sound fly_snd;
    fly_snd.setBuffer(fly_sndbuffer);
    fly_snd.setLoop(true);

    sf::SoundBuffer boat_touch_shoot_sndbuffer;
    boat_touch_shoot_sndbuffer.loadFromFile(FILE_TOUCH_SHOOT);
    sf::Sound boat_touch_shoot_snd;
    boat_touch_shoot_snd.setBuffer(boat_touch_shoot_sndbuffer);

    sf::SoundBuffer boat_touch_bomb_sndbuffer;
    boat_touch_bomb_sndbuffer.loadFromFile(FILE_BOMB2);
    sf::Sound boat_touch_bomb_snd;
    boat_touch_bomb_snd.setBuffer(boat_touch_bomb_sndbuffer);

    /*  AUDIO  */

    sf::Texture home_texture;
    home_texture.loadFromFile(FILE_HOME);
    sf::Sprite home;
    home.setTexture(home_texture);
    home.setScale(0.15, 0.15);
    home.setColor(sf::Color(220, 0, 0));
    home.setPosition(400-home.getGlobalBounds().width/2, 50);

    /*---------- INIT FINISHED ----------*/

    float airport_start_pos;
    airport_distance = airport_pos_dist(rng);
    airport_start_pos = airport_distance;

    while (fenetre.isOpen()){
        partner->restart();
        perso->restart();
        bombPerso.clear();
        missileBoats.clear();
        missilesPerso.clear();
        boats.clear();

        mode = menu.play();

        airport_distance = airport_start_pos;
        bool synched = false;
        continue_wait = true;
        port_receiver = port_primary;
        while (mode == MULTI && !synched){
            //receiver.unbind();
            std::cout << "binding 1 : " << port_receiver << std::endl;
            if(receiver.bind(port_receiver, sf::IpAddress::getLocalAddress()) != sf::Socket::Done){
                port_receiver = port_secondary;
                std::cout << "binding 2 : " << port_receiver << std::endl;
                if(receiver.bind(port_receiver, sf::IpAddress::getLocalAddress()) != sf::Socket::Done){
                    std::cout << "erreur !! connection to player refused !!" << std::endl;
                }
            }

            std::cout << "sync..." << std::endl;
            int sync_res = synchonize(airport_distance);
            std::cout << "sync res : " << sync_res << std::endl;
            if (sync_res == 2){
                receiver.unbind();
                std::cout << "binding 3 : " << port_receiver << std::endl;
                if(receiver.bind(port_receiver, sf::IpAddress::getLocalAddress()) != sf::Socket::Done){
                    std::cout << "erreur !! connection to player refused !!" << std::endl;
                }
                std::cout << "sync..." << std::endl;
                int sync_res = synchonize(airport_distance);
                std::cout << "sync res : " << sync_res << std::endl;
            }

            if (sync_res){
                std::cout << "synchonisation failed !!" << std::endl;
            }else{
                synched = true;
            }


            if (!synched){
                std::cout << "sync failed !!" << std::endl;
                mode = menu.play();
            }
        }

        if (!first){
            perso->setPos({400, 300});
            perso->setWantedPos({400, 300});
        }

        std::cout << "synchonized !!" << std::endl;

        std::cout << "ARE WE FIRST? : " << (int) first << std::endl;

        airport->restart();
        airport->setPos({airport_distance, airport->getPos().y});
        airport->setWantedPos(airport->getPos());


        std::cout << "airport distance : " << airport_distance << std::endl;

        if (mode == MULTI){

            threadreceiver.launch();
        }

        if (airport->getPos().x>1800){
            int min_rand = airport->getPos().x-1500;
            if (min_rand<800)min_rand = 801;
            int max_rand = min_rand+1000;
            boat_pos_dist = std::uniform_int_distribution<int>(min_rand, max_rand);
        }

        mutex.lock();
        collisioner.addEntity(perso);
        if (mode == MULTI)collisioner.addEntity(partner);
        collisioner.addEntity(airport);
        mutex.unlock();

        dt = 0;
        won = false;
        lose = false;
        land_launched = false;
        delay_restart = 3000;
        got_focus = true;
        myclock.restart();
        boat_creation_cooldown.restart();
        gameOver_cooler.restart();
        partner_lose = false;
        partner_won = false;

        go_to_menu = false;

        fly_snd.play();

        while (fenetre.isOpen() && !go_to_menu){
            airport_distance = airport->getPos().x/100;

            dt = myclock.restart().asMicroseconds()*0.0001;

            while (fenetre.pollEvent(event)){
                if (event.type == sf::Event::Closed)fenetre.close();
                else if (event.type == sf::Event::LostFocus){
                    got_focus = false;
                }
                else if (event.type == sf::Event::GainedFocus)got_focus = true;
                else if (event.type == sf::Event::KeyPressed && got_focus){
                    if (event.key.code == sf::Keyboard::Space){
                        createMissilePerso(missilesPerso, collisioner, perso->getPos());
                    }else if (event.key.code == sf::Keyboard::X){
                        createBombPerso({perso->getPos().x+50, perso->getPos().y+100}, bombPerso, collisioner);
                    }
                }else if (event.type == sf::Event::MouseMoved && got_focus){
                    sf::Vector2f mousepos(event.mouseMove.x, event.mouseMove.y);
                    if (home.getGlobalBounds().contains(mousepos)){
                        home.setColor(sf::Color(100, 0, 100));
                    }else{
                        home.setColor(sf::Color(220, 0, 0));
                    }
                }else if (event.type == sf::Event::MouseButtonPressed && got_focus){
                    if (event.mouseButton.button == sf::Mouse::Left){
                        if (home.getColor().b == 100){
                            mutex.lock();
                            fly_snd.stop();
                            shoot_snd.stop();
                            land_snd.stop();
                            boat_touch_bomb_snd.stop();
                            boat_touch_shoot_snd.stop();
                            menu.play();
                            fly_snd.play();
                            lose = false;
                            won = false;
                            boats.clear();
                            missileBoats.clear();
                            missilesPerso.clear();
                            airport->restart();
                            airport->restart();
                            airport_distance = airport_start_pos;
                            airport->setPos({airport_distance, airport->getPos().y});
                            airport->setWantedPos(airport->getPos());
                            land_launched = false;
                            fly_snd.stop();
                            fly_snd.play();
                            home.setColor(sf::Color(220, 0, 0));
                            mutex.unlock();
                            go_to_menu = true;
                        }
                    }
                }
            }

            if (perso->isDead()){
                gameOver_cooler.restart();
                lose = true;
                delay_restart = 3000;
                perso->restart();
                if (mode == MULTI){
                    for (int i = 0; i < repetition_send_important; i++){
                        pack_s.clear();
                        pack_s << "ILOSE" << perso->getPos().x << perso->getPos().y;
                        if (sender.send(pack_s, remote_addr, port_sender) != sf::Socket::Done){
                            std::cout << "error sending msg !!" << std::endl;
                        }
                    }
                }
            }
            if (partner_lose && mode == MULTI){
                gameOver_cooler.restart();
                lose = true;
                delay_restart = 3000;
                perso->restart();
                partner_lose = false;
                std::cout << "lose !! " << std::endl;
            }

            if (perso->getTypeCollided() == "airport" && perso->getPos().y<=550-36-30 && !land_launched){
                fly_snd.stop();
                land_snd.play();
                land_launched = true;
            }

            if (perso->getTypeCollided() == "airport" && perso->getPos().x >= airport->getPos().x+77 && !won){
                gameOver_cooler.restart();
                won = true;
                std::cout << "we won !!" << std::endl;
                delay_restart = 5000;

                perso->setTypeCollided("");

                if (mode == MULTI){
                    for (int i = 0; i < repetition_send_important; i++){
                        pack_s.clear();
                        pack_s << "IWON" << perso->getPos().x << perso->getPos().y;
                        if (sender.send(pack_s, remote_addr, port_sender) != sf::Socket::Done){
                            std::cout << "error sending msg !!" << std::endl;
                        }
                    }
                }

            }

           // std::cout << "gameOver_cooler : " << gameOver_cooler.getElapsedTime().asMilliseconds()

            if ((lose || ((won && mode != MULTI) || (won && partner_won && mode == MULTI))) && gameOver_cooler.getElapsedTime().asMilliseconds()>delay_restart){
                mutex.lock();
                boats.clear();
                missileBoats.clear();
                missilesPerso.clear();
                bombPerso.clear();
                airport->restart();
                airport_distance = airport_start_pos;
                airport->setPos({airport_distance, airport->getPos().y});
                airport->setWantedPos(airport->getPos());
                std::cout << "airport restarted !!" << std::endl;
                land_launched = false;
                fly_snd.stop();
                fly_snd.play();
                mutex.unlock();
                partner_lose = false;
                partner_won = false;
                partner->restart();
                dt = 0;
                won = false;
                lose = false;
                land_launched = false;
                delay_restart = 3000;
                got_focus = true;
                myclock.restart();
                boat_creation_cooldown.restart();
                gameOver_cooler.restart();
                if (first && mode == MULTI){
                    partner->setPos({400, 300});
                    partner->setWantedPos({400, 300});
                }else if (mode == MULTI){
                    perso->setPos({400, 300});
                    perso->setWantedPos({400, 300});
                }
            }

            //Boat Creation
            if (airport->getPos().x>2000){
                int min_rand = 1000;
                int max_rand = min_rand+1000;
                if (max_rand+1000>=airport->getPos().x)max_rand = airport->getPos().x-1000;
                boat_pos_dist = std::uniform_int_distribution<int>(min_rand, max_rand);
                if (boat_creation_cooldown.getElapsedTime().asMilliseconds()>1000 && boats.size()<max_boats){
                    boat_creation_cooldown.restart();
                    if ((first && mode == MULTI) || mode != MULTI){
                        createBoat(boats, collisioner);
                        std::cout << "create boat sended !! " << std::endl;
                    }
                }
            }

            /*------------ MOVES ------------*/
            if (!lose && !won){
                if (got_focus){
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
                        perso->up(dt);
                    }if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
                        perso->right(dt);
                    }if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
                        perso->down(dt);
                    }if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
                        perso->left(dt);
                    }
                }

                if (perso->getTypeCollided() == "missile-boat"){
                    perso->decreaseLife(100.0f);
                    boomanims.push_back(std::make_shared<BoomAnim>(50, perso->getPos(), 0.01, 0, 10.0f));
                }
                mutex.lock();
                for (int i = missilesPerso.size()-1; i >= 0 ; i--){
                    if (missilesPerso[i]->isDead() || missilesPerso[i]->getTypeCollided() != ""){
                        missilesPerso.erase(missilesPerso.begin()+i);
                        continue;
                    }
                    if (missilesPerso[i]->cooler.getElapsedTime().asMilliseconds()>500){
                        missilesPerso.erase(missilesPerso.begin()+i);
                        continue;
                    }
                    missilesPerso[i]->right(dt);
                }
                for (int i = bombPerso.size()-1; i >= 0; i--){
                    if (bombPerso[i]->isDead() || bombPerso[i]->getTypeCollided() != ""){
                        bombPerso.erase(bombPerso.begin()+i);
                        continue;
                    }
                }

                for (int i = boats.size()-1; i >= 0 ; i--){
                    if (boats[i]->getPos().x<=0){
                        boats.erase(boats.begin()+i);
                        continue;
                    }
                    if (boats[i]->isDead() || boats[i]->getTypeCollided() == "airport"){
                        boats.erase(boats.begin()+i);
                        continue;
                    }
                    if (boats[i]->getTypeCollided() == "missile-perso"){
                        boats[i]->decreaseLife(100.0f);
                        boat_touch_shoot_snd.stop();
                        boat_touch_shoot_snd.play();
                        boomanims.push_back(std::make_shared<BoomAnim>(50, boats[i]->getPos(), 0.01, 0, 10.0f));
                    }
                    if (boats[i]->getTypeCollided() == "bomb-perso"){
                        boats[i]->decreaseLife(500.0f);
                        boomanims.push_back(std::make_shared<BoomAnim>(350, boats[i]->getPos(), 0.01, 0, 10.0f));
                        std::cout << "bomb !!" << std::endl;
                        boat_touch_bomb_snd.stop();
                        boat_touch_bomb_snd.play();
                    }

                    if (boats[i]->getPos().y>460)boats[i]->left(dt);

                    if (boats[i]->cooler.getElapsedTime().asMilliseconds()>delay_boat_missile_creat && distance(boats[i]->getPos(), perso->getPos()) < 600 ){
                        boats[i]->cooler.restart();
                        if ((first && mode == MULTI) || mode != MULTI)createMissileBoat({boats[i]->getPos().x, boats[i]->getPos().y-50} , perso->getPos(), missileBoats, collisioner);
                        shoot_snd.stop();
                        shoot_snd.play();
                    }
                }
                for (int i = missileBoats.size()-1; i >= 0 ; i--){
                    if (missileBoats[i]->isDead() || missileBoats[i]->getTypeCollided() != ""){
                        missileBoats.erase(missileBoats.begin()+i);
                        continue;
                    }
                    if (missileBoats[i]->cooler.getElapsedTime().asMilliseconds()>5000){
                        missileBoats.erase(missileBoats.begin()+i);
                        continue;
                    }
                    if (missileBoats[i]->getTypeCollided() == "perso"){
                        missileBoats.erase(missileBoats.begin()+i);
                        continue;
                    }
                    sf::Vector2f offset;
                    offset.x = std::cos(missileBoats[i]->getDirection())*missile_boat_speed*dt;
                    offset.y = std::sin(missileBoats[i]->getDirection())*missile_boat_speed*dt;
                    missileBoats[i]->move(offset.x, offset.y);

                }
                mutex.unlock();

                if (airport_distance>0 && !lose && !won && !partner_won && !partner_lose)airport->left(dt);

                for (int i = boomanims.size()-1; i >= 0; i--){
                    if (boomanims[i]->isFinished()){
                        boomanims.erase(boomanims.begin()+i);
                        continue;
                    }
                    boomanims[i]->update(dt);
                }
            }
            /*------------ MOVES ------------*/

            airport_distance = airport->getPos().x/100;
            TextAirportDist.setString("AIRPORT DIST : "+std::to_string(static_cast<int>(airport_distance)));
            pos_text.setString("pos : "+std::to_string((int)perso->getPos().x)+", "+std::to_string((int)perso->getPos().y) );
            mutex.lock();
            if (!lose && !won)collisioner.updateEntities(dt);
            if (!lose && !won)collisioner.updateCollisions();

            if (mode == MULTI){
                pack_s.clear();
                pack_s << "perso_pos" << perso->getPos().x << perso->getPos().y;
                if (sender.send(pack_s, remote_addr, port_sender) != sf::Socket::Done){
                    std::cout << "error sending msg !!" << std::endl;
                }
            }
            mutex.unlock();

            if (mode == MULTI){
                if (partner_won){
                    partner->setPos({airport->getPos().x+50, airport->getPos().y-36});
                    partner->setWantedPos(partner->getPos());
                }
            }

            fenetre.clear();
            perso->draw(fenetre);
            if (mode == MULTI)partner->draw(fenetre);
            fenetre.draw(ground);
            for (int i = missilesPerso.size()-1; i >= 0; i--){
                missilesPerso[i]->draw(fenetre);
            }
            for (int i = bombPerso.size()-1; i >= 0; i--){
                bombPerso[i]->draw(fenetre);
            }
            for (int i = missileBoats.size()-1; i >= 0; i--){
                missileBoats[i]->draw(fenetre);
            }
            for (int i = boats.size()-1; i >= 0; i--){
                boats[i]->draw(fenetre);
            }

            pos_airport_text = airport->getPos();
            pos_airport_text.x += airport->getRect().width/2-airport_text.getGlobalBounds().width/2;
            pos_airport_text.y += airport->getRect().height/2-airport_text.getGlobalBounds().height/2-10;
            airport_text.setPosition(pos_airport_text);

            airport->draw(fenetre);
            if (lose)fenetre.draw(YouLoseText);
            if ((won && partner_won && mode == MULTI) || (won && mode != MULTI))fenetre.draw(YouWonText);
            Nb_Boats_text.setString("boats : "+std::to_string(boats.size()));
            Nb_Boats_text.setPosition(800-50-Nb_Boats_text.getGlobalBounds().width, 50);
            fenetre.draw(TextAirportDist);
            fenetre.draw(airport_text);
            fenetre.draw(Nb_Boats_text);
            fenetre.draw(pos_text);
            fenetre.draw(home);

            for (int i = boomanims.size()-1; i >= 0; i--){
                boomanims[i]->draw(fenetre);
            }

            fenetre.display();

        }

        fly_snd.stop();
        shoot_snd.stop();
        land_snd.stop();
        boat_touch_bomb_snd.stop();
        boat_touch_shoot_snd.stop();

        if (mode == MULTI){
            continue_receiving = false;
            threadreceiver.wait();
            receiver.unbind();
        }

    }
    return 0;
}

void createBoat(std::vector<std::shared_ptr<Entity>> &boats, CollisionManager &collisioner, bool random, const sf::Vector2f &pos){
    if (boats.size()<max_boats){
        mutex.lock();
        std::shared_ptr<sf::ConvexShape> boat_base = std::make_shared<sf::ConvexShape>(16);
        boat_base->setPointCount(16);
        float x_scale = 0.2;
        float y_scale = 0.2;

        boat_base->setPoint(0, sf::Vector2f(100*x_scale,230*y_scale));
        boat_base->setPoint(1, sf::Vector2f(355*x_scale,230*y_scale));
        boat_base->setPoint(2, sf::Vector2f(456*x_scale,148*y_scale));
        boat_base->setPoint(3, sf::Vector2f(321*x_scale,148*y_scale));
        boat_base->setPoint(4, sf::Vector2f(351*x_scale,15*y_scale));
        boat_base->setPoint(5, sf::Vector2f(320*x_scale,16*y_scale));
        boat_base->setPoint(6, sf::Vector2f(290*x_scale,148*y_scale));
        boat_base->setPoint(7, sf::Vector2f(246*x_scale,148*y_scale)); //CELUI LA
        boat_base->setPoint(8, sf::Vector2f(253*x_scale,0*y_scale));
        boat_base->setPoint(9, sf::Vector2f(200*x_scale,3*y_scale));
        boat_base->setPoint(10, sf::Vector2f(218*x_scale,148*y_scale));
        boat_base->setPoint(11, sf::Vector2f(165*x_scale,148*y_scale));
        boat_base->setPoint(12, sf::Vector2f(110*x_scale,16*y_scale));
        boat_base->setPoint(13, sf::Vector2f(80*x_scale,15*y_scale));
        boat_base->setPoint(14, sf::Vector2f(135*x_scale,200*y_scale));
        boat_base->setPoint(15, sf::Vector2f(0*x_scale,148*y_scale));
        sf::Vector2f tmppos = pos;
        if (random)tmppos.x = boat_pos_dist(rng);
        boat_base->setFillColor(sf::Color::Blue);

        boats.push_back(std::make_shared<Entity>( boat_base, tmppos, sf::Vector2f(ground_speed, 0) ));
        boats[boats.size()-1]->setType("boat");
        collisioner.addEntity(boats[boats.size()-1]);
        if (first && mode == MULTI){
            sf::Packet pack_s;
            pack_s << "newboat" << tmppos.x << tmppos.y;
            for (int i = 0; i < 1; i++){
                if (sender.send(pack_s, remote_addr, port_sender) != sf::Socket::Done){
                    std::cout << "error sending msg !!" << std::endl;
                }
            }

        }
        mutex.unlock();
    }
}

void createMissileBoat(const sf::Vector2f &pos, const sf::Vector2f &perso_pos, std::vector<std::shared_ptr<Entity>> &missilesBoats, CollisionManager &collisioner){
    if (missilesBoats.size() < max_missileboats){
        int myrand = missile_boat_cible(rng);
        mutex.lock();
        std::shared_ptr<sf::RectangleShape> base = std::make_shared<sf::RectangleShape>(sf::Vector2f(10, 5));
        base->setFillColor(sf::Color(255, 0, 0));
        missilesBoats.push_back(std::make_shared<Entity>(base, pos, sf::Vector2f(missile_boat_speed*0.2, 5)));
        missilesBoats[missilesBoats.size()-1]->setType("missile-boat");

        sf::Vector2f cible_pos = partner->getPos();
        if (myrand>500)cible_pos = perso->getPos();

        float direction = std::atan2(cible_pos.y-pos.y, cible_pos.x-pos.x);
        missilesBoats[missilesBoats.size()-1]->setDirection(direction);

        collisioner.addEntity(missilesBoats[missilesBoats.size()-1]);
        if (first && mode == MULTI){
            //std::cout << "sending new boat's missile : " << pos.x  << ", " << pos.y << std::endl;;
            sf::Packet pack_s;
            if (myrand>500)pack_s << "missile_boat_me" << pos.x << pos.y;
            else pack_s << "missile_boat_u" << pos.x << pos.y;
            for (int i = 0; i < 1; i++){
                if (sender.send(pack_s, remote_addr, port_sender) != sf::Socket::Done){
                    std::cout << "error sending msg !!" << std::endl;
                }
            }
        }
        mutex.unlock();
    }
}

void createBombPerso(const sf::Vector2f &pos, std::vector<std::shared_ptr<Entity>> &bombsPerso, CollisionManager &collisioner, bool send){
    mutex.lock();
    std::shared_ptr<sf::CircleShape> base = std::make_shared<sf::CircleShape>(10);
    base->setFillColor(sf::Color(0, 255, 0));
    bombsPerso.push_back(std::make_shared<Entity>(base, pos, sf::Vector2f(20, 5)));
    bombsPerso[bombsPerso.size()-1]->setType("bomb-perso");
    collisioner.addEntity(bombsPerso[bombsPerso.size()-1]);
    if (send && mode == MULTI){
        sf::Packet pack_s;
        pack_s << "bomb_perso" << pos.x << pos.y;
        for (int i = 0; i < 1; i++){
            if (sender.send(pack_s, remote_addr, port_sender) != sf::Socket::Done){
                std::cout << "error sending msg !!" << std::endl;
            }
        }
    }
    mutex.unlock();
}

void createMissilePerso(std::vector<std::shared_ptr<Entity>> &missilesPerso, CollisionManager &collisioner, const sf::Vector2f &pos, bool send){
    mutex.lock();
    missilesPerso.push_back(std::make_shared<Entity>(std::make_shared<sf::RectangleShape>(sf::Vector2f(10, 5)), pos, sf::Vector2f(missile_perso_speed, 5)));
    missilesPerso[missilesPerso.size()-1]->setType("missile-perso");
    collisioner.addEntity(missilesPerso[missilesPerso.size()-1]);
    if (send && mode == MULTI){
        sf::Packet pack_s;
        pack_s << "missile_perso" << pos.x << pos.y;
        for (int i = 0; i < 1; i++){
            if (sender.send(pack_s, remote_addr, port_sender) != sf::Socket::Done){
                std::cout << "error sending msg !!" << std::endl;
            }
        }
    }
    mutex.unlock();
}

float distance(const sf::Vector2f &p1, const sf::Vector2f &p2){
    return std::sqrt( (p2.y-p1.y)*(p2.y-p1.y)+(p2.x-p1.x)*(p2.x-p1.x)  );
}

void updateFont(sf::Font &font){
    if (time(NULL)%2 == 0){
        if (!font.loadFromFile(FILE_PIXEL)){
            std::cout << "error when loading font !" << std::endl;
        }
    }else{
        if (!font.loadFromFile(FILE_FIPPS)){
            std::cout << "error when loading font !" << std::endl;
        }
    }
}

void receive(){
    sf::SocketSelector selector;
    selector.add(receiver);

    sf::Packet pack_r;
    std::string type;
    sf::Vector2f pos;

    while (continue_receiving){
        if (selector.wait(sf::milliseconds(200))){
            if (selector.isReady(receiver)){
                receiver.setBlocking(false);
                sf::Clock c;
                sf::IpAddress addr_received;
                unsigned short port_received;
                while (c.getElapsedTime().asMilliseconds()<200){
                    pack_r.clear();
                    if ( receiver.receive(pack_r, addr_received, port_received) == sf::Socket::Done)break;
                }
                receiver.setBlocking(true);
                pack_r >> type >> pos.x >> pos.y;
                //std::cout << "received " << type << " pos " << pos.x << ", " << pos.y << " from " << addr_received << " at port " << port_received << std::endl;
                if (type == "missile_perso"){
                    //std::cout << " type is missile perso" << std::endl;
                    createMissilePerso(missilesPerso, collisioner, pos, false);
                }else if (type == "IWON"){
                    partner_won = true;
                    std::cout << "partneraire won!!!!" << std::endl;
                }else if (type == "ILOSE"){
                    partner_lose = true;
                }
                else if (type == "perso_pos"){
                    partner->setPos(pos);
                    //std::cout << "partner pos !! : " << pos.x << ", " << pos.y << std::endl;
                    partner->setWantedPos(pos);
                }else if (type == "bomb_perso"){
                    //std::cout _<< " type is bomb perso" << std::endl;
                    createBombPerso(pos, bombPerso, collisioner, false);
                }else if (type == "newboat"){
                    //std::cout << " type is new boat" << std::endl;
                    createBoat(boats, collisioner, false, pos);
                }else if (type == "missile_boat_me"){
                    //std::cout << "me missile" << std::endl;
                    createMissileBoat(pos, partner->getPos(), missileBoats, collisioner);
                }else if (type == "missile_boat_u"){
                    //std::cout << "u missile" << std::endl;
                    createMissileBoat(pos, perso->getPos(), missileBoats, collisioner);
                }else{
                    //std::cout << "type unknow : " << type << std::endl;
                }
            }
        }
    }
    std::cout << "thread ended !!!" << std::endl;
}

int synchonize(float &distance_airport){
    sf::Packet pack_r, pack_s;
    sf::IpAddress addr_received;
    unsigned short port_received;
    std::string data;
    std::string type;
    sf::Clock c;
    c.restart();
    port_received = 0;
    receiver.setBlocking(false);
    first  = true;
    bool test_mode = (remote_addr == sf::IpAddress::getLocalAddress())||(remote_addr == sf::IpAddress::getPublicAddress());

    std::cout << "test mode == " << (int) test_mode << std::endl;
    std::cout << "partner addr : " << remote_addr << ", local addr : " << sf::IpAddress::getLocalAddress() << ", public adr : " << sf::IpAddress::getPublicAddress() << std::endl;

    port_receiver = receiver.getLocalPort();
    if (test_mode && receiver.getLocalPort() == port_secondary){
        first = false;
        port_sender = port_primary;
    }
    if (port_receiver == port_primary && test_mode){
        port_sender = port_secondary;
    }

    std::cout << "Ask your partner to connect ." << std::endl;
    std::cout << "port_receiver : " << receiver.getLocalPort() << std::endl;


    for (int i = 0; i < repetition_send_important; i++){
        pack_s << "connect" << "hello-"+std::to_string(receiver.getLocalPort());
        std::cout << "sending  2 : " << "sender " << port_sender << ", receiver : " << port_receiver << std::endl;
        if (sender.send(pack_s, remote_addr, port_sender) != sf::Socket::Done){
            std::cout << "error sending hello !!" << std::endl;
            return 1;
        }
    }
    pack_s.clear();

    while (c.getElapsedTime().asMilliseconds()<30000){
        pack_r.clear();
        if ( receiver.receive(pack_r, addr_received, port_received) == sf::Socket::Done){
            pack_r >> type >> data;
            port_received = port_received-(port_received%10000);
            if (type == "connect")std::cout << "recevied !!  : " << port_received << ", type : " << type << ", data : " << data << ", sender : " << port_sender << std::endl;
            if (test_mode && data == "hello-"+std::to_string(receiver.getLocalPort()))continue;
            if (type == "connect"){
                if ( data.find("hello") == 0){
                    std::cout << "hello" << std::endl;
                    first = true;
                    break;
                }else if (data == "unblock"){
                    std::cout << "unblock : " << std::endl;
                    first = false;
                    break;
                }
            }
        }
    }
    std::cout << "first : " << (int) first << std::endl;
    if (first){
        for (int i = 0; i < repetition_send_important; i++){
            pack_s << "connect" << "unblock";
            if (sender.send(pack_s, remote_addr, port_sender) != sf::Socket::Done){
                std::cout << "error sending unblock !!" << std::endl;
                return 1;
            }
        }
        pack_s.clear();
    }
    //sending airport distance  !!
    if (first){
        for (int i = 0; i < repetition_send_important; i++){
            pack_s << "distance" << distance_airport;
            if (sender.send(pack_s, remote_addr, port_sender) != sf::Socket::Done){
                std::cout << "error sending hello !!" << std::endl;
                return 1;
            }
        }
        pack_s.clear();

    }else{
        c.restart();
        port_received = 0;
        receiver.setBlocking(false);
        float distance;
        while (c.getElapsedTime().asMilliseconds()<200){
            pack_r.clear();
            if ( receiver.receive(pack_r, addr_received, port_received) == sf::Socket::Done){
                port_received = port_received-(port_received%10000);
                if (port_received == 0)continue;
                pack_r >> type >> distance;
                if (type == "distance"){
                    distance_airport = distance;
                }
            }
        }
    }

    std::cout << "distance airport received : " << distance_airport << std::endl;

    std::cout << "distance airport : " << distance_airport << std::endl;

    return 0;
}
