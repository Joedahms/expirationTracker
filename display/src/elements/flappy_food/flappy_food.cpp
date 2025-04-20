FlappyFood::FlappyFood() {
  this->logger.log("Constructing flappy food");

  this->logger.log("Constructing bird");
  SDL_Rect birdRect          = {20, 0, 32, 32};
  birdRect.y                 = this->windowSurface->h - birdRect.h;
  std::unique_ptr<Bird> bird = std::make_unique<Bird>(this->displayGlobal, birdRect);
  birdPtr                    = bird.get();
  rootElement->addElement(std::move(bird));
  this->logger.log("Bird constructed");

  initializeObstacles();

  this->logger.log("Constructing score text");
  const std::string scoreTextContent = "0";
  const SDL_Color scoreTextColor     = {0, 255, 0, 255}; // Green
  const SDL_Rect scoreTextRect       = {0, 150, 0, 0};
  this->scoreText = std::make_shared<Text>(this->displayGlobal, scoreTextRect,
                                           DisplayGlobal::futuramFontPath,
                                           scoreTextContent, 24, scoreTextColor);
  this->scoreText->setCenteredHorizontal();
  this->rootElement->addElement(scoreText);
  this->logger.log("Score text constructed");

  this->logger.log("Flappy food constructed");
}
