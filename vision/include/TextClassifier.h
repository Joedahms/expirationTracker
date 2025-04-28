#ifndef TEXT_CLASSIFICATION_H
#define TEXT_CLASSIFICATION_H

#include "IModel.h"

class TextClassifier : public IModel {
public:
  TextClassifier(zmqpp::context&, const std::string&);
  void notifyServer(const std::string&);

  OCRResult runModel(const std::filesystem::path&) override;
  OCRResult handleClassification(const std::filesystem::path&) override;

private:
  void handleSideImage(const std::filesystem::path& sideImagePath);
};

#endif
