/*
 * Copyright 2019 Autoware Foundation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * v1.0 Masaya Kataoka
 */

#include <node_status_publisher/value_manager.h>

ValueManager::ValueManager(ros::NodeHandle nh, ros::NodeHandle pnh) {
  nh_ = nh;
  pnh_ = pnh;
  nh_.getParam("/health_checker", diag_params_);
  ros_ok_ = true;
  default_value_exist_ = false;
}

ValueManager::~ValueManager() { ros_ok_ = false; }

void ValueManager::run() {
  boost::thread update_thread =
      boost::thread(boost::bind(&ValueManager::updateParams, this));
}

void ValueManager::setDefaultValue(std::string key, std::string type,
                                   double warn_value, double error_value,
                                   double fatal_value) {
  if (default_value_exist_) {
    return;
  }
  data_[{{key, type}, autoware_health_checker::LEVEL_WARN}] = warn_value;
  data_[{{key, type}, autoware_health_checker::LEVEL_ERROR}] = error_value;
  data_[{{key, type}, autoware_health_checker::LEVEL_FATAL}] = fatal_value;
  default_value_exist_ = true;
  return;
}

bool ValueManager::foundParamKey(std::string key, std::string type,
                                 uint8_t level, double &value) {
  for (auto itr = diag_params_.begin(); itr != diag_params_.end(); itr++) {
    if (itr->first != key) {
      continue;
    }
    for (auto params_itr = itr->second.begin(); params_itr != itr->second.end();
         params_itr++) {
      if (params_itr->first != type) {
        continue;
      }
      for (auto value_itr = params_itr->second.begin();
           value_itr != params_itr->second.end(); value_itr++) {
        if (autoware_health_checker::LEVEL_OK == level ||
            autoware_health_checker::LEVEL_UNDEFINED == level) {
          continue;
        }
        if (value_itr->first == "warn" &&
            autoware_health_checker::LEVEL_WARN == level) {
          XmlRpc::XmlRpcValue data = value_itr->second;
          value = data;
          return true;
        }
        if (value_itr->first == "error" &&
            autoware_health_checker::LEVEL_ERROR == level) {
          XmlRpc::XmlRpcValue data = value_itr->second;
          value = data;
          return true;
        }
        if (value_itr->first == "fatal" &&
            autoware_health_checker::LEVEL_FATAL == level) {
          XmlRpc::XmlRpcValue data = value_itr->second;
          value = data;
          return true;
        }
      }
    }
  }
  return false;
}

double ValueManager::getValue(std::string key, std::string type,
                              uint8_t level) {
  double ret;
  mtx_.lock();
  if (level == autoware_health_checker::LEVEL_WARN) {
    std::string key_str;
    if (foundParamKey(key, type, level, ret)) {
    } else {
      ret = data_[{{key, type}, level}];
    }
  }
  mtx_.unlock();
  return ret;
}

void ValueManager::updateParams() {
  ros::Rate rate(1);
  while (ros_ok_) {
    mtx_.lock();
    nh_.getParam("health_checker", diag_params_);
    mtx_.unlock();
    rate.sleep();
  }
  return;
}