#ifndef ZERO_WEIGHT_H
#define ZERO_WEIGHT_H

class ZeroWeight : public State {
public:
  ZeroWeight(struct DisplayGlobal displayGlobal);
  void render() const override;

private:
  Logger logger;

  void retry();
  void override();
  void cancel();
}

#endif
