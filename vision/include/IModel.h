#ifndef IMODEL_H
#define IMODEL_H

#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <string>

#include "../../food_item.h"
#include "../../logger.h"
#include "helperFunctions.h"

enum TaskType { unknown, CLS, OCR, EXP };

class IModel {
public:
  IModel(const std::string& logFileName, zmqpp::context& context)
      : logger(logFileName), requestSocket(context, zmqpp::socket_type::request),
        replySocket(context, zmqpp::socket_type::reply) {}
  virtual ~IModel() = default;

protected:
  Logger logger;
  zmqpp::socket requestSocket;
  zmqpp::socket replySocket;

  virtual std::string runModel(const std::filesystem::path&)      = 0;
  virtual bool handleClassification(const std::filesystem::path&) = 0;

  std::string sendRequest(const TaskType&, const std::filesystem::path&);
  std::string taskTypeToString(const TaskType&) const;
};

#endif
