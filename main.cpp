 #include<iostream>
 #include"SFML\Graphics.hpp"
 #include"SFML\Window.hpp"
 #include"SFML\System.hpp"
 #include<math.h>
 #include<cstdlib>
 #include<vector>
 using namespace sf;
 
 
 
 class Bullet
 {
 public:
 	Sprite shape;
     
 
 	Bullet(Texture *texture, Vector2f pos)
 	{
 		this->shape.setTexture(*texture);
 
 		this->shape.setScale(0.07f, 0.07f);
 
 		this->shape.setPosition(pos);
 	}
 
 	~Bullet(){}
 };
 
 class Player
 {
 public:
 	Sprite shape;
 	Texture *texture;
 	
 	int HP;
 	int HPMax;
 
 	std::vector<Bullet> bullets;
 
 	Player(Texture *texture)
 	{
 		this->HPMax = 10;
 		this->HP = this->HPMax;
 
 		this->texture = texture;
 		this->shape.setTexture(*texture);
 
 		this->shape.setScale(0.15f, 0.15f);
 	}
 
 	~Player() {}
 
 };
 
 class Enemy
 {
 public:
 	Sprite shape;
 
 	int HP;
 	int HPMax;
 
 	Enemy(Texture *texture, Vector2u windowSize)
 	{
 		this->HPMax = rand() % 3 + 1;
 		this->HP = this->HPMax;
 
 		this->shape.setTexture(*texture);
 
 		this->shape.setScale(0.1f, 0.1f);
 
 		this->shape.setPosition(windowSize.x - this->shape.getGlobalBounds().width, rand()% (int)(windowSize.y - this->shape.getGlobalBounds().height));
 	}
 
 	~Enemy(){}
 };
 
 int main()
 {
      start:
         srand(time(NULL));

         RenderWindow window(VideoMode(800, 600), "STAR WARS: behind enemy lines!", Style::Default);
         window.setFramerateLimit(60);


         //init background
         Texture background;
         background.loadFromFile("Background2.png");
         Sprite bg;
         bg.setTexture(background);
         //bg.setTextureRect(IntRect(0, 0, 800, 600));



         // Init text
         Font font;
         font.loadFromFile("Metropolian-Display.ttf");

         // Init textures
         Texture playerTex;
         playerTex.loadFromFile("ship.png");

         Texture enemyTex;
         enemyTex.loadFromFile("enemy.png");

         Texture bulletTex;
         bulletTex.loadFromFile("missleTex01.png");

         // UI init
         Text scoreText;
         scoreText.setFont(font);
         scoreText.setCharacterSize(20);
         scoreText.setFillColor(Color::White);
         scoreText.setPosition(10.f, 10.f);

         Text gameOverText;
         gameOverText.setFont(font);
         gameOverText.setCharacterSize(35);
         gameOverText.setFillColor(Color::Yellow);
         gameOverText.setPosition(75.f, window.getSize().y / 2);
         gameOverText.setString("GAME OVER! press enter to restart");

         // Player init
         int score = 0;
         Player player(&playerTex);
         int shootTimer = 20;
         Text hpText;
         hpText.setFont(font);
         hpText.setCharacterSize(12);
         hpText.setFillColor(Color::White);

         // Enemy init
         int enemySpawnTimer = 0;
         std::vector<Enemy> enemies;

         Text eHpText;
         eHpText.setFont(font);
         eHpText.setCharacterSize(12);
         eHpText.setFillColor(Color::White);

         while (window.isOpen())
         {

             Event event;
             while (window.pollEvent(event))
             {
                 if (event.type == Event::Closed)
                     window.close();
             }
             if (player.HP < 3)     //give warning by turning background red
             {
                 bg.setColor(Color(255, 0, 0));
             }

             if (player.HP > 0)
             {
                 // Player
                 if (Keyboard::isKeyPressed(Keyboard::W))
                     player.shape.move(0.f, -10.f);
                 if (Keyboard::isKeyPressed(Keyboard::A))
                     player.shape.move(-10.f, 0.f);
                 if (Keyboard::isKeyPressed(Keyboard::S))
                     player.shape.move(0.f, 10.f);
                 if (Keyboard::isKeyPressed(Keyboard::D))
                     player.shape.move(10.f, 0.f);

                 hpText.setPosition(player.shape.getPosition().x, player.shape.getPosition().y - hpText.getGlobalBounds().height);
                 hpText.setString(std::to_string(player.HP) + "/" + std::to_string(player.HPMax));

                 //	 Collision with window // if player is going out of window space then its position is set to far most corner of window from where it was going out
                 if (player.shape.getPosition().x <= 0)  //Left
                     player.shape.setPosition(0.f, player.shape.getPosition().y);
                 if (player.shape.getPosition().x >= window.getSize().x - player.shape.getGlobalBounds().width) // Right
                     player.shape.setPosition(window.getSize().x - player.shape.getGlobalBounds().width, player.shape.getPosition().y);
                 if (player.shape.getPosition().y <= 0) // Top
                     player.shape.setPosition(player.shape.getPosition().x, 0.f);
                 if (player.shape.getPosition().y >= window.getSize().y - player.shape.getGlobalBounds().height) // Bottom
                     player.shape.setPosition(player.shape.getPosition().x, window.getSize().y - player.shape.getGlobalBounds().height);

                 // Update Controls
                 if (shootTimer < 15)
                     shootTimer++;

                 if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= 15) // Shooting
                 {
                     player.bullets.push_back(Bullet(&bulletTex, player.shape.getPosition()));
                     shootTimer = 0; // reset timer
                 }

                 //	 Bullets
                 for (size_t i = 0; i < player.bullets.size(); i++)
                 {
                     // Move
                     player.bullets[i].shape.move(20.f, 0.f);

                     // Out of window bounds
                     if (player.bullets[i].shape.getPosition().x > window.getSize().x)
                     {
                         player.bullets.erase(player.bullets.begin() + i);
                         break;
                     }

                     //	 Enemy collision
                     for (size_t k = 0; k < enemies.size(); k++)
                     {
                         if (player.bullets[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds()))
                         {
                             if (enemies[k].HP <= 1)
                             {
                                 score += enemies[k].HPMax;
                                 enemies.erase(enemies.begin() + k);
                             }
                             else
                                 enemies[k].HP--; // ENEMY TAKE DAMAGE

                             player.bullets.erase(player.bullets.begin() + i);
                             break;
                         }
                     }
                 }

                 // Enemy
                 if (enemySpawnTimer < 50)
                     enemySpawnTimer++;

                 // enemy spawn
                 if (enemySpawnTimer >= 50)
                 {
                     enemies.push_back(Enemy(&enemyTex, window.getSize()));
                     enemySpawnTimer = 0; // reset timer
                 }

                 for (size_t i = 0; i < enemies.size(); i++)
                 {
                     enemies[i].shape.move(-6.f, 0.f);

                     if (enemies[i].shape.getPosition().x <= 0 - enemies[i].shape.getGlobalBounds().width)
                     {
                         enemies.erase(enemies.begin() + i);
                         break;
                     }

                     if (enemies[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()))
                     {
                         enemies.erase(enemies.begin() + i);

                         player.HP--;  // PLAYER TAKE DAMAGE
                         break;
                     }
                 }


                 //	 UI Update
                 scoreText.setString("Score: " + std::to_string(score));
             }

             // Draw ===================================================================== DRAW //this is where images and texts are called
             window.clear();
             //background
             window.draw(bg);


             // player
             window.draw(player.shape);

             // Bullets
             for (size_t i = 0; i < player.bullets.size(); i++)
             {
                 window.draw(player.bullets[i].shape);
             }

             //// enemy
             for (size_t i = 0; i < enemies.size(); i++)
             {
                 eHpText.setString(std::to_string(enemies[i].HP) + "/" + std::to_string(enemies[i].HPMax));
                 eHpText.setPosition(enemies[i].shape.getPosition().x, enemies[i].shape.getPosition().y - eHpText.getGlobalBounds().height);
                 window.draw(eHpText);
                 window.draw(enemies[i].shape);
             }

             // UI
             window.draw(scoreText);
             window.draw(hpText);

             if (player.HP <= 0)
             {
                 window.draw(bg);
                 scoreText.setPosition(350.f, 250.f);
                 scoreText.setFillColor(Color::Yellow);
                 window.draw(scoreText);
                 window.draw(gameOverText);
             }

             window.display();

             if (Keyboard::isKeyPressed(Keyboard::Enter))
             {
                 goto start;
             }
         }

        
 }
 



