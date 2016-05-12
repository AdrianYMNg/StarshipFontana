#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0),score(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetPosition(player_pos);

  const int number_of_aliens = 10;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2(((canvas_w/number_of_aliens) * i)+30, 200.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

const int number_of_wall = 5;
  for(int i=0; i<number_of_wall; i++) {
    // place an wall at width/number_of_wall * i
    auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
    auto pos   = Point2(((canvas_w/number_of_wall) * i)+65, 100.0f);
    wall->SetPosition(pos);
    walls.push_back(wall);
  }

  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto pos  = Point2((canvas_w/2), 300);
  coin->SetPosition(pos);
  coins.push_back(coin);
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    for(auto w : walls) {
    if(player->CollidesWith(w)) {
        player->GoEast();
        }
     }
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    for(auto w : walls) {
    if(player->CollidesWith(w)) {
        player->GoWest();
        }
     }
    break;
case SFEVENT_PLAYER_UP:
    player->GoNorth();
    for(auto w : walls) {
    if(player->CollidesWith(w)) {
        player->GoSouth();
        }
     }
    break;
  case SFEVENT_PLAYER_DOWN:
    player->GoSouth();
    for(auto w : walls) {
    if(player->CollidesWith(w)) {
        player->GoNorth();
        }
     }
    break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }

  for(auto c: coins) {
    // do something here
  }

  // Update enemy positions
  for(auto a : aliens) {
    // do something here
  }
  
  // Update wall positions
  for(auto a : walls) {
    // do something here
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
        score = score +1;
        std::wcout<<score<<std::endl;
      }
    }
  }
  for(auto p : projectiles) {
    for(auto w : walls) {
      if(p->CollidesWith(w)) {
        p->HandleCollision();
        w->HandleCollision();
      }
    }
  }
  for(auto p : projectiles) {
    for(auto c : coins) {
      if(p->CollidesWith(c)) {
        p->HandleCollision();
        c->HandleCollision();
        std::wcout<<"Your Score is: "<<std::endl;
        std::wcout<<score<<std::endl;
        is_running = false;
    break;
    }
      }
  }
  
  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);

  list<shared_ptr<SFAsset>> tmp1;
  for(auto c : coins) {
    if(c->IsAlive()) {
      tmp1.push_back(c);
    }
  }
  coins.clear();
  coins = list<shared_ptr<SFAsset>>(tmp1);
  
   list<shared_ptr<SFAsset>> tmp2;
  for(auto p : projectiles) {
    if(p->IsAlive()) {
      tmp2.push_back(p);
    }
  }
  projectiles.clear();
  projectiles = list<shared_ptr<SFAsset>>(tmp2);
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
  player->OnRender();

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    if(c->IsAlive()) {c->OnRender();}
  }

  for(auto w: walls) {
    if(w->IsAlive()) {w->OnRender();}
  }	
  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
