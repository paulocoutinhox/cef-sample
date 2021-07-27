#pragma once

#include <string>

const std::string APP_NAME = "My App";
const std::string ASSETS_PATH = "assets";

// use this config when develop and test webapp locally (cd webapp && npm run serve)
// const std::string APP_CONFIG_START_URL = "http://localhost:8080/index.html";
// const std::string APP_ORIGIN = "http://localhost:8080";
// const bool USE_LOCAL_RESOURCES = false;

const std::string APP_CONFIG_START_URL = "http://__app/index.html";
const std::string APP_ORIGIN = "http://__app/";
const bool USE_LOCAL_RESOURCES = true;
